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

static double rate = 44100;

static const int MAX_CHANNELS = 2;

/* class definiton */
class Synth
{
private:
    double position;
    KeyMap keys;
    Key monoKey;
    Key* monoKeyTarget;
    Controls controls;
    float panning_factors[MAX_CHANNELS];
    bool idle;

public:
    Synth () :
        Synth (rate)
    {
    }
    Synth (const double sample_rate) :
        position (0.0),
        keys ()
    {
        controls = Controls(sample_rate);
        setPanningFactors(1, 1); // Left and right equal at normal volume
    }

    inline void outputSamples(float* audio_out_ptr, const uint32_t start, const uint32_t end, const int outchannels=1, const bool reset_output=true)
    {
        if (outchannels > MAX_CHANNELS) {
            std::cerr << "outchannels is " << outchannels << " but must be less than " << MAX_CHANNELS << std::endl;
            throw std::invalid_argument ("outchannels is greater than MAX_CHANNELS!");
        }

        // Conditionally reset the outs to 0 since it may still have data from last time
        if (reset_output) for (int i = start*outchannels; i < end*outchannels; ++i) {
            audio_out_ptr[i] = 0.0f;
        }

        idle = true; // Set to false in these loops if a key is run.
        for (uint32_t i = start; i < end; ++i)
        {
            if (controls.get(P_VOICE_MODE) == VOICE_POLY) {

                keys.startLoop();
                Key* k;
                while (k = keys.getNext()) {
                    if (k->isOn()) {
                        idle = false;
                        const float outvalue = k->get();
                        // Add to pointer
                        for (int j=0; j<outchannels; j++) {
                            audio_out_ptr[i*outchannels+j] += outvalue;
                        }
                        // Proceed every key that's ON
                        k->proceed();
                    } else {
                        keys.erasePrevious();
                    }
                }
            } else {
                if (monoKey.isOn()) {
                    idle = false;
                    const float outvalue = monoKey.get();
                    // Output to pointer
                    for (int j=0; j<outchannels; j++) {
                        audio_out_ptr[i*outchannels+j] += outvalue;
                    }
                    // Proceed just the one key
                    monoKey.proceed();
                }
            }

            // Pan with precalculated panning_factors (see setPanningFactors)
            for (int j=0; j<outchannels; j++) {
                audio_out_ptr[i*outchannels+j] *= panning_factors[j];
            }
        }
    }

    inline void setControl(const int index, const float value) {
        controls.updateValue(index, value);
    }

    inline void startNote(const int note, const int velocity) {
        idle = false;
        keys.getKey(note & 0x7f)->press(
            note,
            velocity,
            &controls
        );
        if (controls.get(P_VOICE_MODE) != VOICE_POLY) {
            const bool single_key = ! keys.hasAtLeast(2);
            const bool porta = (controls.get(P_PORTAMENTO) != 1.0f) && (
                (controls.get(P_VOICE_MODE) == VOICE_PORTA) ||
                (!single_key)
            );

            monoKey.press(
                note,
                velocity,
                &controls,
                !porta
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
            // Here in mono mode, the `keys` map is basically just used as a
            // convenient way to see which piano keys are down. Meanwhile, the
            // monoKey is the only actual key that runs its logic and generates
            // sound.

            // This is in contrast to POLY mode, where `keys` keeps track of
            // notes that are still making sound.

            // That's why we're erasing the note index here. We want to make
            // sure we don't pivot back to this note after releasing some later
            // note
            keys.eraseIndex(note);

            if (monoKey.note == note) {
                if (keys.hasAtLeast(1)) {
                    // Immediately make monoKey press the longest waiting key still held down
                    const bool refreq = (controls.get(P_PORTAMENTO) == 1.0f);
                    // (controls.get(P_VOICE_MODE) == VOICE_AUTO_PORTA) || (controls.get(P_VOICE_MODE) == VOICE_AUTO_PORTA_LEGATO);
                    monoKey.press(
                        keys.rotateKeyOrder()->note, // Play the held note we haven't played in the longest
                        monoKey.velocity,
                        &controls,
                        refreq
                    );
                } else {
                    // This is the only note. Do a normal long release.
                    monoKey.release();
                }
            }
        }
    }

    inline void stopAllNotes() {
        keys.startLoop();
        Key* k;
        while (k = keys.getNext()) {
            releaseNote(k->note, 127); // This should also cover the monoKey scenario
        }
    }

    inline void stopAllSounds() {
        keys.startLoop();
        Key* k;
        while (k = keys.getNext()) {
            releaseNote(k->note, 127); //Same as stopAllNotes, we'll just mute afterward
            k->mute();
        }
        monoKey.mute();
    }

    inline void setPanningFactors(float left, float right) {
        panning_factors[0] = left;
        panning_factors[1] = right;
    }

    inline bool isIdle() {
        return idle;
    }
};
