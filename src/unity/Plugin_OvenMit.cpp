#include "AudioPluginUtil.h"
#include "Key.hpp"
#include "Filter2.hpp"
#include <iostream>

namespace OvenMit
{
    const int MAX_KEYS = 1;
    const int MAX_INSTANCES = 32;

    enum Param
    {
        P_ENV_MODE_1 = 0,
        P_ATTACK     = 1,
        P_DECAY      = 2,
        P_SUSTAIN    = 3,
        P_RELEASE    = 4,

        P_WAVEFORM   = 5,
        P_LEVEL      = 6,
        P_PITCH      = 7,
        P_CUTOFF     = 8,
        P_RES_HEIGHT = 9,
        P_RES_WIDTH  = 10,

        P_ENV_MODE_2 = 11,
        P_ENV_AMT_2  = 12,
        P_ATTACK_2   = 13,
        P_DECAY_2    = 14,
        P_SUSTAIN_2  = 15,
        P_RELEASE_2  = 16,

        P_WAVEFORM_2_MODE = 17,
        P_WAVEFORM_2 = 18,
        P_LEVEL_2    = 19,
        P_PITCH_2    = 20,
        P_INSTANCE   = 21,
        P_NUM         = 22
    };

    struct EffectData
    {
        float parameters[P_NUM];
    };

    struct OvenMitInstance
    {
        Key keys[MAX_KEYS];
        Controls controls;
        Filter filter;
    };

    inline OvenMitInstance* GetOvenMitInstance(int index)
    {
        // std::cout << "GetOvenMitInstance..." << std::endl;
        static bool initialized[MAX_INSTANCES] = { false };
        static OvenMitInstance instance[MAX_INSTANCES];
        if (index < 0 || index >= MAX_INSTANCES)
            return NULL;
        if (!initialized[index])
        {
            initialized[index] = true;
            instance[index].controls = Controls(44100);
            instance[index].filter = Filter();
            instance[index].filter.setWaveform(WAVEFORM_SINE);
            instance[index].keys[0] = Key(44100);
            instance[index].keys[0].press(68, 127, &(instance[index].controls), &(instance[index].filter));
        }
        // std::cout << "    ...finished" << std::endl;
        return &instance[index];
    }

    int InternalRegisterEffectDefinition(UnityAudioEffectDefinition& definition)
    {
        int numparams = P_NUM;
        definition.paramdefs = new UnityAudioParameterDefinition[numparams];
        RegisterParameter(definition, "EnvMode1", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f, P_ENV_MODE_1, "Envelope 1 mode");
        RegisterParameter(definition, "Attack1", "s", 0.001f, 4.0f, 0.001f, 1.0f, 1.0f, P_ATTACK, "Attack time for envelope 1");
        RegisterParameter(definition, "Decay1", "s", 0.001f, 4.0f, 0.001f, 1.0f, 1.0f, P_DECAY, "Decay time for envelope 1");
        RegisterParameter(definition, "Sustain1", "", 0.000f, 1.0f, 0.5f, 1.0f, 1.0f, P_SUSTAIN, "Sustain level for envelope 1");
        RegisterParameter(definition, "Release1", "s", 0.001f, 4.0f, 0.001f, 1.0f, 1.0f, P_RELEASE, "Release time for envelope 1");

        RegisterParameter(definition, "Waveform1", "", 0.000f, 4.0f, 0.000f, 1.0f, 1.0f, P_WAVEFORM, "Waveform type for oscillator 1");
        RegisterParameter(definition, "Level1", "", 0.000f, 1.0f, 0.100f, 1.0f, 1.0f, P_LEVEL, "Fader level for oscillator 1");
        RegisterParameter(definition, "Pitch1", "", -24.0f, 24.0f, 0.0f, 1.0f, 1.0f, P_PITCH, "Pitch offset in semitones of oscillator 1");
        RegisterParameter(definition, "Cutoff1", "", 0.0f, 60.0f, 30.0f, 1.0f, 1.0f, P_CUTOFF, "The low-pass cutoff frequency, in partials, of oscillator 1");
        RegisterParameter(definition, "Resonance1", "", 0.0f, 8.0f, 3.0f, 1.0f, 1.0f, P_RES_HEIGHT, "Height of resonance around cutoff frequency");
        RegisterParameter(definition, "ResonanceWidth1", "", 1.0f, 30.0f, 3.0f, 1.0f, 1.0f, P_RES_WIDTH, "Width of resonance around cutoff frequency, in partials");

        RegisterParameter(definition, "EnvMode2", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f, P_ENV_MODE_2, "Envelope 2 mode");
        RegisterParameter(definition, "EnvAmount2", "", 0.0f, 10.0f, 3.0f, 1.0f, 1.0f, P_ENV_AMT_2, "Amount envelope 2 should affect its target");
        RegisterParameter(definition, "Attack2", "s", 0.001f, 4.0f, 0.001f, 1.0f, 1.0f, P_ATTACK_2, "Attack time for envelope 2");
        RegisterParameter(definition, "Decay2", "s", 0.001f, 4.0f, 0.001f, 1.0f, 1.0f, P_DECAY_2, "Decay time for envelope 2");
        RegisterParameter(definition, "Sustain2", "", 0.000f, 1.0f, 0.5f, 1.0f, 1.0f, P_SUSTAIN_2, "Sustain level for envelope 2");
        RegisterParameter(definition, "Release2", "s", 0.001f, 4.0f, 0.001f, 1.0f, 1.0f, P_RELEASE_2, "Release time for envelope 2");

        RegisterParameter(definition, "Oscillator2Mode", "", 0.000f, 3.0f, 0.000f, 1.0f, 1.0f, P_WAVEFORM_2_MODE, "Target/mode of Oscillator 2");
        RegisterParameter(definition, "Waveform2", "", 0.000f, 4.0f, 0.000f, 1.0f, 1.0f, P_WAVEFORM_2, "Waveform type for oscillator 2");
        RegisterParameter(definition, "Level2", "", 0.000f, 1.0f, 0.100f, 1.0f, 1.0f, P_LEVEL_2, "Fader level for oscillator 2");
        RegisterParameter(definition, "Pitch2", "", -24.0f, 24.0f, -12.0f, 1.0f, 1.0f, P_PITCH_2, "Pitch offset in semitones of oscillator 2");

        RegisterParameter(definition, "Instance", "", 0.0f, (float)(MAX_INSTANCES-1), 0.0f, 1.0f, 1.0f, P_INSTANCE, "Which synth instance to communicate with");

        return numparams;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ProcessCallback(UnityAudioEffectState* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int outchannels)
    {
        // std::cout << "ProcessCallback..." << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        OvenMitInstance* instance = GetOvenMitInstance((int)data->parameters[P_INSTANCE]);
        // std::cout << "  Got oven mit instance" << std::endl;
        for (unsigned int n = 0; n < length; n++)
        {
            // std::cout << "  Get from keys..." << std::endl;
            float value = instance->keys[0].get();
            // std::cout << "  Got value " << value << std::endl;
            for (int i = 0; i < outchannels; i++)
            {
                // std::cout << "  Write to outchannel " << i << std::endl;
                outbuffer[n * outchannels + i] = value;
            }
            // std::cout << "  Proceeding key" << std::endl;
            instance->keys[0].proceed();
        }
        // std::cout << "  ...finished" << std::endl;

        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK CreateCallback(UnityAudioEffectState* state)
    {
        // std::cout << "CreateCallback..." << std::endl;
        EffectData* effectdata = new EffectData;
        memset(effectdata, 0, sizeof(EffectData));
        // std::cout << "  Creating an instance" << std::endl;
        for (int n = 0; n < MAX_KEYS; n++) {
            int instance_index = effectdata->parameters[P_INSTANCE];
            OvenMitInstance* instance = GetOvenMitInstance(instance_index);
            for (int k=0; k<P_NUM-1; k++) {
                instance->controls.set(static_cast<ControlPorts>(k), effectdata->parameters[k]);
            }
        }
        state->effectdata = effectdata;
        InitParametersFromDefinitions(InternalRegisterEffectDefinition, effectdata->parameters);
        // std::cout << "  ...finished" << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    /* API functions */

    extern "C" UNITY_AUDIODSP_EXPORT_API void OvenMit_TestKeyPress() {
        // std::cout << "OvenMit_TestKeyPress..." << std::endl;
        OvenMitInstance* instance = GetOvenMitInstance(0);
        instance->keys[0].press(69, 127, &(instance->controls), &(instance->filter));
        // std::cout << "   ...finished." << std::endl;
    }

    ////////////////////////////////////////////
    /* Boilerplate copied from Unity examples */
    ////////////////////////////////////////////

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK ReleaseCallback(UnityAudioEffectState* state)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        delete data;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK SetFloatParameterCallback(UnityAudioEffectState* state, int index, float value)
    {
        // std::cout << "SetFloatParameterCallback... " << index << " to " << value << std::endl;
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= P_NUM)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        data->parameters[index] = value;
        OvenMitInstance* instance = GetOvenMitInstance((int)data->parameters[P_INSTANCE]);
        instance->controls.set(static_cast<ControlPorts>(index), value);
        if (index == P_WAVEFORM) {
            instance->filter.setWaveform(
                static_cast<Waveform>((int)value)
            );
        }
        // std::cout << "   ...finished." << std::endl;
        return UNITY_AUDIODSP_OK;
    }

    UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK GetFloatParameterCallback(UnityAudioEffectState* state, int index, float* value, char *valuestr)
    {
        EffectData* data = state->GetEffectData<EffectData>();
        if (index >= P_NUM)
            return UNITY_AUDIODSP_ERR_UNSUPPORTED;
        if (value != NULL)
            *value = data->parameters[index];
        if (valuestr != NULL)
            valuestr[0] = 0;
        return UNITY_AUDIODSP_OK;
    }

    int UNITY_AUDIODSP_CALLBACK GetFloatBufferCallback(UnityAudioEffectState* state, const char* name, float* buffer, int numsamples)
    {
        return UNITY_AUDIODSP_OK;
    }
}