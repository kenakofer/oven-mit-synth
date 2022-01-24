/* include libs */
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <new>
#include <array>
#include <random>
#include <ctime>
#include <utility>

#include "Controls.hpp"
#include "Key.hpp"
#include "KeyMap.hpp"

/* class definiton */
class Synth
{
private:
    double rate;
    double position;
    KeyMap keys;
    Key monoKey;
    Key* monoKeyTarget;
    Controls controls;

public:
    Synth (const double sample_rate) :
        rate (sample_rate),
        position (0.0),
        keys ()
    {
        controls = Controls(sample_rate);
    }

    inline void outputSamples(float* audio_out_ptr, const uint32_t start, const uint32_t end)
    {
        for (uint32_t i = start; i < end; ++i)
        {
            audio_out_ptr[i] = 0.0f;
            keys.startLoop();
            Key* k;
            while (k = keys.getNext()) {
                if (k->isOn()) {
                    audio_out_ptr[i] += k->get();
                    k->proceed();
                } else {
                    keys.erasePrevious();
                }
            }
        }
    }

    inline void setControl(const int index, const float value) {
        controls.updateValue(index, value);
    }

    inline void startNote(const int note, const int velocity) {
        keys.getKey(note & 0x7f)->press(
            note,
            velocity,
            &controls
        );
    }

    inline void releaseNote(const int note, const int velocity) {
        keys.getKey(note & 0x7f)->release(
            note,
            velocity
        );
    }

    inline void stopAllNotes() {
        keys.startLoop();
        Key* k;
        while (k = keys.getNext()) k->release();
    }

    inline void stopAllSounds() {
        keys.startLoop();
        Key* k;
        while (k = keys.getNext()) k->mute();
    }
};
