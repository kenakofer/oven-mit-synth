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
            if (controls.get(P_VOICE_MODE) == VOICE_POLY) {
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
            } else if (controls.get(P_VOICE_MODE) == VOICE_PORTA) {
                if (keys.hasAtLeast(1)) {
                    if (monoKey.isOn()) {
                        audio_out_ptr[i] += monoKey.get();
                        monoKey.proceed();
                    } else {
                        keys.eraseIndex(monoKey.note);
                    }
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
        if (controls.get(P_VOICE_MODE) != VOICE_POLY) {
            monoKey.press(
                note,
                velocity,
                &controls
            );
        }
    }

    inline void releaseNote(int note, const int velocity) {
        note = note & 0x7f;
        keys.getKey(note)->release(
            note,
            velocity
        );
        if (controls.get(P_VOICE_MODE) != VOICE_POLY) {
            if (monoKey.note == note) {
                if (keys.hasAtLeast(2)) {
                    // Immediately erase the released key and press the next
                    keys.eraseIndex(note);
                    monoKey.press(
                        keys.rotateKeyOrder()->note, // Play the held note we haven't played in the longest
                        monoKey.velocity,
                        &controls
                    );
                } else {
                    // This is the only note. Do a normal release.
                    monoKey.release();
                }
            } else {
                // Since we weren't playing this note, immediately erase the
                // released key so we don't accidently pivot to it later. No
                // audible change happens here.
                keys.eraseIndex(note);
            }
        }
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
