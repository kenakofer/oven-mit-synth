#include <iostream>
#include <limits.h>

#include "AudioPluginUtil.h"
#include "../Synth.hpp"
#include "../Param.hpp"
#include "NoteEventQueue.hpp"

namespace OvenMit
{
    const int MAX_KEYS = 1;
    const int MAX_INSTANCES = 32;

    // These are shared across instances, and help us do global logic exactly once per buffer.
    static double global_samples_per_beat = 22100; // Default value: 120 bpm at 44.1 khz. Can be changed whenever by API calls.
    static double global_beat = 0.0;    // Beat is tracked additively based on increases in the sample divided by global_samples_per_beat
    static UInt64 global_sample = 0;    // This is simply copied from state->currdsptick. We only keep track of it as a convenient flag for if we need to run the global update.
    static bool initialized[MAX_INSTANCES] = { false };

    // This parameter is just for the instance of the AudioEffect that's
    // attached to an audio bus in the Unity Editor, which shouldn't be confused
    // with any of the native synth instances itself. The AudioEffect is just a sink
    // for audio, which can receive from one of the synthesizers.
    enum UnityParameters {
        INSTANCE_INDEX, // Which native instance of OvenMit should this audio bus receive audio data from?
        UNITY_PARAM_NUM
    };

    struct EffectData
    {
        float parameters[UNITY_PARAM_NUM];
    };

    struct OvenMitInstance
    {
        // TODO are these recreated at runtime, or when Unity loads this file?
        Synth synth;

        // TODO we should use a better data structure that's indexed by both time and
        // note, (basically a variation of KeyMap with more intelligent insert)
        std::priority_queue<NoteEvent, std::vector<NoteEvent>, PrioritizeEarlierTime> note_event_queue;
    };

    inline OvenMitInstance* GetOvenMitInstance(int index)
    {
        // std::cout << "GetOvenMitInstance with index..." << index << std::endl;
        static OvenMitInstance instance[MAX_INSTANCES];
        if (index < 0 || index >= MAX_INSTANCES) {
            std::cout << "OvenMit Error: Can't fetch index: " << index << " Because the max is " << MAX_INSTANCES << std::endl;
            return NULL;
        }
        if (!initialized[index])
        {
            std::cout << "OvenMit: Instantiating new instance on demand: " << index << std::endl;
            initialized[index] = true;
            instance[index].synth = Synth(44100);

            // Make sure the queue is empty, e.g. if OvenMit_ResetPlugin was called.
            while (!instance[index].note_event_queue.empty()) instance[index].note_event_queue.pop();

            // Control values are all 0 to start (no sound except a small click on note start), so set the defaults
            for (int p=0; p<P_NUM_CONTROLS; p++) {
                instance[index].synth.setControl(p, PARAM_DEFAULT[p]);
            }
            std::cout << "    ...finished" << std::endl;
        }
        return &instance[index];
    }

    int InternalRegisterEffectDefinition(UnityAudioEffectDefinition& definition)
    {
        std::cout << "OvenMit: InternalRegisterEffectDefinition..." << std::endl;
        definition.paramdefs = new UnityAudioParameterDefinition[UNITY_PARAM_NUM];
        RegisterParameter( definition, "Synth Instance", "", 0, MAX_INSTANCES-1, 0, 1.0f, 1.0f, 0,
            "Which native instance of OvenMit should this audio bus receive audio data from?"
        );
        std::cout << "...finished" << std::endl;
        return UNITY_PARAM_NUM;
    }

    void globalProcess(UnityAudioEffectState* state) {
        global_beat += (state->currdsptick - global_sample) / global_samples_per_beat;
        global_sample = state->currdsptick;
        // std::cout << "OvenMit: Running globalProcess by instance# " << state->GetEffectData<EffectData>()->parameters[INSTANCE_INDEX] << std::endl;
        // std::cout << "Global sample: " << global_sample << std::endl;
        // std::cout << "Global beat: " << global_beat << std::endl;
    }

    // May not be accurate after tempo changes, but when used within the time of a buffer it will be fine.
    long beatToSample(double beat) {
        return global_sample + (long)(global_samples_per_beat * (beat - global_beat));
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int outchannels)
    {
        // TODO find a way for this method to work when two audioeffects share the same synth index

        // Run the globalProcess only once, no matter if we have 1 or 32 synths
        if (global_sample != state->currdsptick) {
            globalProcess(state);
        }

        EffectData* data = state->GetEffectData<EffectData>();
        // std::cout << "OvenMit: ProcessCallback for synth: " << data->parameters[INSTANCE_INDEX] << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(data->parameters[INSTANCE_INDEX]);
        Synth* synth = &instance->synth;


        UInt64 tick = global_sample; // Global time in samples plus frames within this buffer
        // std::cout << "  ...starting buffer at tick: " << tick << std::endl;
        int frame = 0;                    // Time within this buffer, range is [0, length]

        while (frame < length) {
            // Process any events that are ready
            long next_event_at = instance->note_event_queue.empty() ? LONG_MAX : beatToSample(instance->note_event_queue.top().wait_for_beat);
            // std::cout << " ...next_event_at set to" << next_event_at << std::endl;
            while (tick >= next_event_at) {
                // std::cout << "tick " << tick << " was greater than next_event_at" << next_event_at << std::endl;
                const NoteEvent top = instance->note_event_queue.top();
                std::cout << " ...Note Event with wait_for_beat: " << top.wait_for_beat << std::endl;
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
                next_event_at = instance->note_event_queue.empty() ? LONG_MAX : beatToSample(instance->note_event_queue.top().wait_for_beat);
                // std::cout << " ...2 next_event_at set to" << next_event_at << std::endl;
            }

            // Step forward to the end of the frame, or to the next event, whichever is sooner.
            int framesToNext = length - frame;
            if (framesToNext > next_event_at - tick)
                framesToNext = next_event_at - tick;

            // Synthesizer does MAGIC to compute samples
            // std::cout << "...outputSamples from " << frame << " to " << frame+framesToNext << std::endl;
            synth->outputSamples(outbuffer, frame, frame+framesToNext, outchannels);

            frame += framesToNext;
            tick += framesToNext;
        }

        // std::cout << "...finished" << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    /* I think the initial state is in the default state, NOT in the state of the
     * Unity GUI, so doing any operations or instantiating a particular
     * OvenMitInstance doesn't make sense in this method. Afterward it seems
     * Unity calls SetFloatParameterCallback for every parameter anyways, which
     * will create the instance.
     *
     *
     * This code is loaded into memory once when Unity loads, but CreateCallback
     * is called for every OvenMitSynth on every replay, so the static instance
     * counting doesn't work.
     *
     * [Later edit] This code is NOT called again on stopping and playing the
     * game within Unity. The first play calls it for every synth, then no more.
     */
    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state)
    {
        std::cout << "OvenMit: CreateCallback..." << std::endl;
        EffectData* effectdata = new EffectData;
        memset(effectdata, 0, sizeof(EffectData));
        state->effectdata = effectdata;

        // The instance index is assigned based on the next available based on instance_count.
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

    // TODO stopallsounds taking no parameters does so for all instances.
    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_StopAllSounds(int instance_index) {
        std::cout << "OvenMit_StopAllSounds..." << std::endl;
        GetOvenMitInstance(instance_index)->synth.stopAllSounds();
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_ScheduleNote(int instance_index, int midiNote, int velocity, double startbeat, double endbeat) {
        if (startbeat >= endbeat) endbeat = startbeat + 0.0001; // Don't allow 0 or negative width notes
        std::cout << "OvenMit_ScheduleNote..." << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(instance_index);
        std::cout << "  ...Push start" << instance_index << " " << midiNote << " " << velocity << " " << startbeat << " " << endbeat << std::endl;
        instance->note_event_queue.push(NoteEvent(NoteStart, startbeat, instance_index, midiNote, velocity));
        std::cout << "  ...Push end" << std::endl;
        instance->note_event_queue.push(NoteEvent(NoteRelease, endbeat, instance_index, midiNote, velocity));
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_SetSynthParameter(int instance_index, int parameter_index, float value) {
        std::cout << "OvenMit_SetSynthParameter, instance" << instance_index << " param " << parameter_index << " to " << value << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(instance_index);
        instance->synth.setControl(parameter_index, value);
        std::cout << "   ...finished." << std::endl;
    }

    extern "C" UNITY_AUDIODSP_EXPORT_API double OvenMit_GetGlobalBeat() {
        std::cout << "OvenMit_GetGlobalBeat (finished)" << std::endl;
        return global_beat;
    }
    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_SetGlobalSamplesPerBeat(double samples_per_beat) {
        std::cout << "OvenMit_SetGlobalSamplesPerBeat (finished)" << samples_per_beat << std::endl;
        global_samples_per_beat = samples_per_beat;
    }

    /* Force all instances to be reloaded from scratch, including event queues. */
    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_ResetPlugin() {
        std::cout << "OvenMit_ResetPlugin (finished)" << std::endl;
        for (int i=0; i<MAX_INSTANCES; i++) initialized[i] = false;
    }


    ////////////////////////////////////////////
    /* Boilerplate copied from Unity examples */
    ////////////////////////////////////////////

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state)
    {
        std::cout << "OvenMit: ReleaseCallback OvenMit function..." << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        delete data;
        std::cout << "...finished" << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        std::cout << "OvenMit: SetFloatParameterCallback on instance... " << (int)data->parameters[INSTANCE_INDEX] << " param index " << index << " to " << value << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(data->parameters[INSTANCE_INDEX]);;
        if (index >= UNITY_PARAM_NUM)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        data->parameters[index] = value;
        std::cout << "   ...finished." << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char *valuestr)
    {
        std::cout << "OvenMit: GetFloatParameterCallback OvenMit function..." << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= UNITY_PARAM_NUM)
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
        std::cout << "OvenMit: GetFloatBufferCallback (finished)" << std::endl;
        return UNITY_AUDIODSP_OK;
    }
}