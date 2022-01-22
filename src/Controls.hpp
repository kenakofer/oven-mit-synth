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

class Controls {
    public:
        Filter filter;

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
                control[index] = limit<float> (value, controlLimit[index].first, controlLimit[index].second);
                // if (i == P_LEVEL) controlLevel.set (control[i], 0.01 * rate);
                if (i == P_WAVEFORM) {
                    filter.setWaveform(static_cast<Waveform>(value));
                }
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
        // inline double proceed() {
            // //MONO stuff
            // float freq = control[P_PITCH_2];
            // position += freq / rate;
            // if (control[P_WAVEFORM_2] == WAVEFORM_SQUARE) {
            //     if (control[P_WAVEFORM_2_TARGET] == LEVEL_1) {
            //         float p = fmod(position, 1.0f);
            //         float val = (p < 0.5f ? 1.0f : -1.0f);
            //         val *= control[P_LEVEL_2];
            //         moddedControl[P_LEVEL] = val;
            //         // std::cout << "Mod control: " << val << std::endl;
            //     }
            // }
            // return position;
        // }
};

#endif