#ifndef CONTROLS_HPP_
#define CONTROLS_HPP_

#include <array>
#include <cstdint>

#include "Limit.hpp"
#include "Waveform.hpp"
#include "Param.hpp"
#include "Filter2.hpp"

enum OscillatorModes // Only for Osc 2 for now?
{
    OSC_ADD,
    OSC_FM_1,
    OSC_AM_1,
    OSC_CUTOFF_1
};
enum EnvelopeModes
{
    ENV_LEVEL_1,
    ENV_PITCH_1,
    ENV_CUTOFF_1,
    ENV_LEVEL_2
};
enum FilterTypes
{
    FILTER_LOWPASS,
    FILTER_HIGHPASS
};
enum VoiceModes
{
    VOICE_POLY,
    VOICE_PORTA,
    VOICE_PORTA_LEGATO,
    VOICE_AUTO_PORTA,
    VOICE_AUTO_PORTA_LEGATO
};

class Controls {
    private:
        std::array<float, P_NUM_CONTROLS> control;
        std::array<float, P_NUM_CONTROLS> moddedControl;
        double position; // For waveform 2
        double rate;

    public:
        Controls() :
            Controls (44100)
        {
        }

        Controls(double rt) :
            rate (rt)
        {
            control.fill(0.0f);
            moddedControl.fill(0.0f);
        }

        inline void updateValue(int index, float value) {
            /* copy and validate control port values */
            bool updated = false;
            if (value != control[index]) {
                control[index] = limit<float> (value, PARAM_LIMIT[index].first, PARAM_LIMIT[index].second);
            }
        }
        inline float get(Param index) {
            return getAbsolute(index) + getModded(index);
        }
        inline void set(Param index, float value) {
            control[index] = value;
        }
        inline float getModded(Param index) {
            return moddedControl[index];
        }
        inline float getAbsolute(Param index) {
            return control[index];
        }
};

#endif