#ifndef CONTROLS_HPP_
#define CONTROLS_HPP_

#include <array>
#include <cstdint>

#include "Limit.hpp"
#include "Waveform.hpp"

enum ControlPorts
{
    CONTROL_ENV_MODE_1 = 0,
    CONTROL_ATTACK     = 1,
    CONTROL_DECAY      = 2,
    CONTROL_SUSTAIN    = 3,
    CONTROL_RELEASE    = 4,

    CONTROL_WAVEFORM   = 5,
    CONTROL_LEVEL      = 6,
    CONTROL_PITCH      = 7,
    CONTROL_CUTOFF  = 8,
    CONTROL_RES_HEIGHT= 9,
    CONTROL_RES_WIDTH = 10,

    CONTROL_ENV_MODE_2 = 11,
    CONTROL_ENV_AMT_2  = 12,
    CONTROL_ATTACK_2   = 13,
    CONTROL_DECAY_2    = 14,
    CONTROL_SUSTAIN_2  = 15,
    CONTROL_RELEASE_2  = 16,

    CONTROL_WAVEFORM_2_MODE = 17,
    CONTROL_WAVEFORM_2 = 18,
    CONTROL_LEVEL_2    = 19,
    CONTROL_PITCH_2    = 20,
    CONTROL_NR         = 21
};

constexpr std::array<std::pair<float, float>, CONTROL_NR> controlLimit =
{{
    {0.0f, 3.0f},           // ENV_MODE_1
    {0.001f, 4.0f},         // ATTACK
    {0.001f, 4.0f},         // DECAY
    {0.0f, 1.0f},           // SUSTAIN
    {0.001f, 4.0f},         // RELEASE

    {0.0f, 4.0f},           // WAVEFORM
    {0.0f, 1.0f},           // LEVEL
    {-24.0f, 24.0f},        // PITCH
    {0.0f, 60.0f},          // CUTOFF
    {0.0f, 8.0f},           // RES_HEIGHT
    {1.0f, 30.0f},          // RES_WIDTH

    {0.0f, 3.0f},           // ENV MODE 2
    {0.0f, 10.0f},          // ENV_AMT_2
    {0.001f, 4.0f},         // ATTACK
    {0.001f, 4.0f},         // DECAY
    {0.0f, 1.0f},           // SUSTAIN
    {0.001f, 4.0f},         // RELEASE

    {0.0f, 3.0f},           // WAVEFORM 2 MODE
    {0.0f, 4.0f},           // WAVEFORM 2
    {0.0f, 1.0f},           // LEVEL 2
    {-24.0f, 24.0f},        // PITCH 2
}};

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

    private:
        std::array<const float*, CONTROL_NR> control_ptr;
        std::array<float, CONTROL_NR> control;
        std::array<float, CONTROL_NR> moddedControl;
        double position; // For waveform 2
        double rate;

    public:
        Controls() :
            Controls (44100)
        {
        }

        Controls(double rt) :
            control_ptr {nullptr},
            rate (rt)
        {
            control_ptr.fill(nullptr);
            control.fill(0.0f);
            moddedControl.fill(0.0f);
        }
        void connectControlPort(const uint32_t port, void* data_location) {
            control_ptr[port] = static_cast<const float*>(data_location);
        }
        inline bool isEveryControlConnected() {

            for (int i=0; i<CONTROL_NR; ++i) {
                if (!control_ptr[i]) {
                    return false;
                }

            }
            return true;
        }

        inline bool updateValues() {
            /* copy and validate control port values */
            bool updated = false;
            for (int i=0; i<CONTROL_NR; ++i) {
                if (*control_ptr[i] != control[i]) {
                    control[i] = limit<float> (*control_ptr[i], controlLimit[i].first, controlLimit[i].second);
                    // if (i == CONTROL_LEVEL) controlLevel.set (control[i], 0.01 * rate);
                    if (i == CONTROL_WAVEFORM) {
                        updated = true;
                    }
                }
            }
            return updated;

            /* filter refreshing and control moving */
            // if (refresh_filter) {
            //     filter.setValues(
            //         control[CONTROL_CUTOFF_DIFF],
            //         control[CONTROL_CUTOFF],
            //         control[CONTROL_RES_HEIGHT],
            //         static_cast<Waveform> (control[CONTROL_WAVEFORM])
            //     );
            // }
        }
        inline float get(ControlPorts index) {
            return getAbsolute(index) + getModded(index);
        }
        inline void set(ControlPorts index, float value) {
            control[index] = value;
        }
        inline float getModded(ControlPorts index) {
            return moddedControl[index];
        }
        inline float getAbsolute(ControlPorts index) {
            return control[index];
        }
        // inline double proceed() {
            // //MONO stuff
            // float freq = control[CONTROL_PITCH_2];
            // position += freq / rate;
            // if (control[CONTROL_WAVEFORM_2] == WAVEFORM_SQUARE) {
            //     if (control[CONTROL_WAVEFORM_2_TARGET] == LEVEL_1) {
            //         float p = fmod(position, 1.0f);
            //         float val = (p < 0.5f ? 1.0f : -1.0f);
            //         val *= control[CONTROL_LEVEL_2];
            //         moddedControl[CONTROL_LEVEL] = val;
            //         // std::cout << "Mod control: " << val << std::endl;
            //     }
            // }
            // return position;
        // }
};

#endif