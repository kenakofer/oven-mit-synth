#include <iostream>
#include <limits.h>

#include "AudioPluginUtil.h"
#include "../Synth.hpp"
#include "../Param.hpp"
#include "NoteEventQueue.hpp"

namespace OvenMit
{
    const int MAX_KEYS = 1;
    const int MAX_INSTANCES = (int)PARAM_LIMIT[P_UNITY_INSTANCE].second+1;

    struct EffectData
    {
        float parameters[P_NUM_UNITY];
    };

    struct OvenMitInstance
    {
        Synth synth;

        // TODO we should use a better data structure that's indexed by both time and
        // note, (basically a variation of KeyMap with more intelligent insert)
        std::priority_queue<NoteEvent, std::vector<NoteEvent>, PrioritizeEarlierTime> note_event_queue;
    };

    inline OvenMitInstance* GetOvenMitInstance(int index)
    {
        std::cout << "GetOvenMitInstance with index..." << index << std::endl;
        static bool initialized[MAX_INSTANCES] = { false };
        static OvenMitInstance instance[MAX_INSTANCES];
        if (index < 0 || index >= MAX_INSTANCES)
            return NULL;
        if (!initialized[index])
        {
            initialized[index] = true;
            instance[index].synth = Synth(44100);
        }
        std::cout << "    ...finished" << std::endl;
        return &instance[index];
    }

    int InternalRegisterEffectDefinition(UnityAudioEffectDefinition& definition)
    {
        std::cout << "OvenMit: InternalRegisterEffectDefinition..." << std::endl;
        definition.paramdefs = new UnityAudioParameterDefinition[P_NUM_UNITY];
        for (int p = 0; p < P_NUM_UNITY; p++) {
            std::cout << "  ...Registering parameter " << p+1 << " out of " << P_NUM_UNITY << std::endl;
            std::cout << "   ..." << PARAM_NAME[p] << std::endl;
            std::cout << "   ..." << PARAM_LIMIT[p].first << std::endl;
            std::cout << "   ..." << PARAM_LIMIT[p].second << std::endl;
            std::cout << "   ..." << PARAM_DEFAULT[p] << std::endl;
            std::cout << "   ..." << p << std::endl;
            RegisterParameter(
                definition,
                PARAM_NAME[p],              // Name
                "",                         // Unit
                PARAM_LIMIT[p].first,       // Min
                PARAM_LIMIT[p].second,      // Max
                PARAM_DEFAULT[p],           // Default
                1.0f,                       // Scale?
                1.0f,                       // Another scale?
                p,                          // index
                ""                          // description      TODO
            );
        }

        std::cout << "...finished" << std::endl;
        return P_NUM_UNITY;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int outchannels)
    {
        std::cout << "ProcessCallback..." << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        OvenMitInstance* instance = GetOvenMitInstance((int)data->parameters[P_UNITY_INSTANCE]);
        Synth* synth = &instance->synth;

        UInt64 tick = state->currdsptick; // Global time in samples plus frames within this buffer
        std::cout << "  ...starting buffer at tick: " << tick << std::endl;
        int frame = 0;                    // Time within this buffer, range is [0, length]

        while (frame < length) {
            // Process any events that are ready
            long next_event_at = instance->note_event_queue.empty() ? LONG_MAX : instance->note_event_queue.top().wait_for_sample;
            while (tick >= next_event_at) {
                const NoteEvent top = instance->note_event_queue.top();
                std::cout << " ...Note Event with wait_for_sample: " << top.wait_for_sample << std::endl;
                switch (top.event_type) {
                    case NoteStart:
                        std::cout << " ...Start scheduled note: " << top.note << std::endl;
                        synth->startNote(top.note, top.velocity);
                        break;
                    case NoteRelease:
                        std::cout << " ...Release scheduled note: " << top.note << std::endl;
                        synth->releaseNote(top.note, top.velocity);
                        break;
                }

                instance->note_event_queue.pop();
                std::cout << "  ...New size of event queue: " << instance->note_event_queue.size() << std::endl;
                // If empty, place the next_event_at to basically infinity
                next_event_at = instance->note_event_queue.empty() ? LONG_MAX : instance->note_event_queue.top().wait_for_sample;
            }

            // Step forward to the end of the frame, or to the next event, whichever is sooner.
            int framesToNext = length - frame;
            if (framesToNext > next_event_at - tick)
                framesToNext = next_event_at - tick;

            // Synthesizer does MAGIC to compute samples
            std::cout << "...outputSamples from " << frame << " to " << frame+framesToNext << std::endl;
            synth->outputSamples(outbuffer, frame, frame+framesToNext, outchannels);

            frame += framesToNext;
            tick += framesToNext;
        }

        std::cout << "...finished" << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state)
    {
        std::cout << "CreateCallback..." << std::endl;

        EffectData* effectdata = new EffectData;
        memset(effectdata, 0, sizeof(EffectData));

        // std::cout << "  Creating an instance" << std::endl;
        for (int n = 0; n < MAX_KEYS; n++) {
            int instance_index = effectdata->parameters[P_UNITY_INSTANCE];
            OvenMitInstance* instance = GetOvenMitInstance(instance_index);
            for (int k=0; k<P_NUM_UNITY-1; k++) {
                instance->synth.setControl(k, effectdata->parameters[k]);
            }
        }
        state->effectdata = effectdata;
        InitParametersFromDefinitions(InternalRegisterEffectDefinition, effectdata->parameters);
        std::cout << "  ...finished" << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    /* API functions */

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_TestKeyPress() {
        std::cout << "OvenMit_TestKeyPress..." << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(0);
        instance->synth.startNote(69, 127);
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_StartNote(int instance_index, int midi_note, int velocity) {
        std::cout << "OvenMit_StartNote..." << std::endl;
        GetOvenMitInstance(instance_index)->synth.startNote(midi_note, velocity);
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_ReleaseNote(int instance_index, int midi_note) {
        std::cout << "OvenMit_ReleaseNote..." << std::endl;
        GetOvenMitInstance(instance_index)->synth.releaseNote(midi_note, 127);
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_StopAllNotes(int instance_index) {
        std::cout << "OvenMit_StopAllNotes..." << std::endl;
        GetOvenMitInstance(instance_index)->synth.stopAllNotes();
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_StopAllSounds(int instance_index) {
        std::cout << "OvenMit_StopAllSounds..." << std::endl;
        GetOvenMitInstance(instance_index)->synth.stopAllSounds();
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_ScheduleNote(int instance_index, int midiNote, int velocity, long start, long end) {
        std::cout << "OvenMit_ScheduleNote..." << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(instance_index);
        std::cout << "  ...Push start" << instance_index << " " << midiNote << " " << velocity << " " << start << " " << end << std::endl;
        instance->note_event_queue.push(NoteEvent(NoteStart, start, instance_index, midiNote, velocity));
        std::cout << "  ...Push end" << std::endl;
        instance->note_event_queue.push(NoteEvent(NoteRelease, end, instance_index, midiNote, velocity));
        std::cout << "   ...finished." << std::endl;
    }

    ////////////////////////////////////////////
    /* Boilerplate copied from Unity examples */
    ////////////////////////////////////////////

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state)
    {
        std::cout << "Other OvenMit function..." << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        delete data;
        std::cout << "...finished" << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value)
    {
        std::cout << "SetFloatParameterCallback... " << index << " to " << value << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= P_NUM_UNITY)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        data->parameters[index] = value;
        OvenMitInstance* instance = GetOvenMitInstance((int)data->parameters[P_UNITY_INSTANCE]);
        instance->synth.setControl(index, value);
        std::cout << "   ...finished." << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char *valuestr)
    {
        std::cout << "Other OvenMit function..." << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= P_NUM_UNITY)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        if (value != NULL)
            *value = data->parameters[index];
        if (valuestr != NULL)
            valuestr[0] = 0;
        return UNITY_AUDIODSP_OK;
        std::cout << "...finished" << std::endl;
    }

    int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState* state, const char* name, float* buffer, int numsamples)
    {
        return UNITY_AUDIODSP_OK;
    }
}