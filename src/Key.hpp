#ifndef KEY_HPP_
#define KEY_HPP_

#include <cstdint>
#include <cmath>
#include <random>
#include <ctime>
#include <array>
#include <iostream>
#include <limits.h>

#include "Waveform.hpp"
#include "KeyStatus.hpp"
#include "Sine.hpp"
#include "Controls.hpp"

class Key
{
public:
    uint8_t note;
    uint8_t velocity;

private:
    KeyStatus status;
    double rate;
    double position;
    double position2;
    float start_level_1;
    float start_level_2;
    double freq;
    double target_freq;
    double freq2;
    double target_freq2;
    double modfreq1;
    double modfreq2;
    double time;
    double portamento_factor;
    Controls* controls;

public:
    Key ();
    Key (const double rt);
    void press (const uint8_t nt, const uint8_t vel, Controls *c, bool refreq);
    void release ();
    void release (const uint8_t nt, const uint8_t vel);
    void off ();
    void mute ();
    float get ();
    void proceed ();
    bool isOn();

private:
    float adsr (int whichOscillator);
    float synth ();
    float synthPartials ();
    void setCachedValue(double pos, float val);
    float getCachedValue(double pos);
    float synth2();
};

inline Key::Key () :
    Key (44100)
{

}

inline Key::Key (const double rt) :
    status (KEY_OFF),
    note (0),
    velocity (0),
    rate (rt),
    position (0.0),
    position2 (0.0),
    start_level_1 (0.0f),
    start_level_2 (0.0f),
    freq (pow (2.0, (static_cast<double> (note) - 69.0) / 12.0) * 440.0),
    freq2 (pow (2.0, (static_cast<double> (note) - 69.0) / 12.0) * 440.0),
    modfreq1 (freq),
    modfreq2 (freq),
    time (0.0)
{

}

inline void Key::press (const uint8_t nt, const uint8_t vel, Controls *c, bool refreq=true)
{
    note = nt;
    controls = c;

    // Set OSC 1 freq based on the MIDI note number, adjusting for pitch 1 offset.
    target_freq = pow (2.0, (static_cast<double> (note) - 69.0 + controls->get(P_PITCH)) / 12.0) * 440.0;

    if (controls->get(P_KEYTRACK_2) == KEYTRACK_ON) {
        // With keytracking off, base the second OSC frequncy on the MIDI note, adjusting for pitch2
        target_freq2 = pow (2.0, (static_cast<double> (note) - 69.0 + controls->get(P_PITCH_2)) / 12.0) * 440.0;
    } else {
        // With keytracking off, base the second OSC frequncy on A220, adjusting for pitch2. This should be useful for basic LFO stuff
        // The "* 2" is so that we have a greater pitch range in this case. So changing the slider by 6 is actually an octave in this mode.
        target_freq2 = pow (2.0, controls->get(P_PITCH_2) * 2 / 12.0) * 220.0;
    }

    // In monophonic mode, we sometimes don't instantly go to the right
    // frequence, instead sliding to it (portmento)
    if (refreq) {
        // no portamento, just set to the target freq
        freq = target_freq;
        freq2 = target_freq2;
        modfreq1 = freq;
        modfreq2 = freq2;
    } else {
        portamento_factor = 1.0 + .001 / (*controls).get(P_PORTAMENTO);
    }

    // We sometimes don't reattack the note (legato), meaning the key's
    // old envelope and veloctiy shouldn't be overwritten, only the pitches

    if (controls->get(P_LEGATO) == LEGATO_OFF) {
        // Full reattack
        start_level_1 = 0;
        start_level_2 = 0;
    } else {
        // Start at the key's prior start_level, then reset time
        start_level_1 = adsr(1) * velocity / vel;
        start_level_2 = adsr(2) * velocity / vel;
    }

    time = 0.0;
    velocity = vel;
    status = KEY_PRESSED;
}

inline void Key::release ()
{
    release (note, velocity);
}

inline void Key::release (const uint8_t nt, const uint8_t vel)
{
    if ((status == KEY_PRESSED) && (note == nt))
    {
        start_level_1 = adsr (1);
        start_level_2 = adsr (2);
        time = 0.0;
        status = KEY_RELEASED;
    }
}

inline void Key::off ()
{
    position = 0.0;
    position2 = 0.0;
    start_level_1 = 0.0;
    start_level_2 = 0.0;
    status = KEY_OFF;
}

inline void Key::mute ()
{
    velocity = 0;
}

inline float Key::adsr(int whichEnvelope)
{
    float attack;
    float decay;
    float sustain;
    float release;
    float start_level;
    switch (whichEnvelope) {
        case (1):
            attack = controls->get(P_ATTACK);
            decay = controls->get(P_DECAY);
            sustain = controls->get(P_SUSTAIN);
            release = controls->get(P_RELEASE);
            start_level = start_level_1;
            break;
        case (2):
            attack = controls->get(P_ATTACK_2);
            decay = controls->get(P_DECAY_2);
            sustain = controls->get(P_SUSTAIN_2);
            release = controls->get(P_RELEASE_2);
            start_level = start_level_2;
            break;
        default:
            std::cerr << "Error: Envelope #" << whichEnvelope << " not found." << std::endl;
    }


    switch (status)
    {
    case KEY_PRESSED:
        if (time < attack)
        {
            return start_level + (1.0f - start_level) * time /attack;
        }

        if (time < attack + decay)
        {
            return 1.0f + (sustain - 1.0f) * (time - attack) / decay;
        }

        return sustain;

    case KEY_RELEASED:
        if (time > release) return 0.0f;
        return start_level - start_level * time / release;

    default:
        return 0.0f;
    }
}

inline float Key::synthPartials()
{
    float cutoff_partial = controls->get(P_CUTOFF);
    if (controls->get(P_ENV_MODE_1) == ENV_CUTOFF_1) {
        cutoff_partial += 4 * adsr(1);
    }
    if (controls->get(P_ENV_MODE_2) == ENV_CUTOFF_1) {
        cutoff_partial += 4 * adsr(2) * controls->get(P_ENV_AMT_2);
    }
    if (controls->get(P_WAVEFORM_2_MODE) == OSC_CUTOFF_1) {
        cutoff_partial += 4 * synth2();
    }
    if (controls->get(P_FILTER) == FILTER_LOWPASS) {
        return lowPassInWave(
            static_cast<Waveform> (controls->get(P_WAVEFORM)),
            modfreq1,
            position,
            cutoff_partial,
            controls->get(P_RES_WIDTH), controls->get(P_RES_HEIGHT)
        );
    } else {
        return highPassInWave(
            static_cast<Waveform> (controls->get(P_WAVEFORM)),
            modfreq1,
            position,
            cutoff_partial,
            controls->get(P_RES_WIDTH), controls->get(P_RES_HEIGHT)
        );
    }
}

inline float Key::synth2()
{
    float value;
    if (freq2 < 100) {
        // Aliased wave will sound better on the low notes maybe?
        value = valueInWaveform(static_cast<Waveform> (controls->get(P_WAVEFORM_2)), position2);
    } else {
        // Dealiased wave: (partials only up to nyquist (22k))
        value = lowPassInWave(
                static_cast<Waveform> (controls->get(P_WAVEFORM_2)),
                modfreq2,
                position2,
                10000, // Take all the partials!
                controls->get(P_RES_WIDTH), controls->get(P_RES_HEIGHT)
            );
    }

    if (controls->get(P_ENV_MODE_2) == ENV_LEVEL_2) value *= adsr(2);
    else if (controls->get(P_ENV_MODE_1) == ENV_LEVEL_2) value *= adsr(1);
    else if (controls->get(P_WAVEFORM_2_MODE) == OSC_ADD) value *= adsr(1); //Since the second waveform is being added, presumably we want some envelope on it.

    value *= controls->get(P_LEVEL_2);
    return value;
}

inline float Key::get ()
{
    float value = synthPartials() *
                    controls->get(P_LEVEL);

    if (controls->get(P_ENV_MODE_1) == ENV_LEVEL_1) {
        value *= adsr(1);
        if (controls->get(P_ENV_MODE_2) == ENV_LEVEL_1) {
            value *= adsr(2);
        }
    } else if (controls->get(P_ENV_MODE_2) == ENV_LEVEL_1) {
            value *= adsr(2);
    } else {
            // If no envelope is targeting ENV_LEVEL_1, use env 1 for it anyways.
            value *= adsr(1);
    }

    if (controls->get(P_WAVEFORM_2_MODE) == OSC_AM_1) {
        value *= 1 + synth2();
    }

    if (controls->get(P_WAVEFORM_2_MODE) == OSC_ADD) {
        float value2 = synth2();
        value += value2;
    }

    // Velocity adjustment
    value *= static_cast<float> (velocity) / 127.0f;

    return value;
}

inline void Key::proceed ()
{
    time += 1.0 / rate;

    // If portamento, the freq needs to slide toward target_freq. The P_PORTAMENTO setting is slightly less than 1, so dividing makes freq bigger.
    if (freq != target_freq) {
        if (freq < target_freq) {
            freq *= portamento_factor;
            if (freq > target_freq) freq = target_freq;
        }
        if (freq > target_freq) {
            freq /= portamento_factor;
            if (freq < target_freq) freq = target_freq;
        }
    }
    if (freq2 != target_freq2) {
        if (freq2 < target_freq2) {
            freq2 *= portamento_factor;
            if (freq2 > target_freq2) freq2 = target_freq2;
        }
        if (freq2 > target_freq2) {
            freq2 /= portamento_factor;
            if (freq2 < target_freq2) freq2 = target_freq2;
        }
    }

    // Calculate instantaneous frequncy for osc 1
    modfreq1 = freq;
    if (controls->get(P_ENV_MODE_1) == ENV_PITCH_1) {
        modfreq1 *= pow (2.0, adsr(1) * 12 / 12.0);
    }
    if (controls->get(P_ENV_MODE_2) == ENV_PITCH_1) {
        modfreq1 *= pow (2.0, adsr(2) * controls->get(P_ENV_AMT_2) * 4 / 12.0); // Scale by 4 so it covers more
    }
    if (controls->get(P_WAVEFORM_2_MODE) == OSC_FM_1) {
        modfreq1 *= 1 + synth2();
    }
    // Move Osc 1 forward correctly
    position += modfreq1 / rate;

    // Calculate instantaneous frequncy for osc 2
    modfreq2 = freq2;
    if (controls->get(P_ENV_MODE_1) == ENV_PITCH_2) {
        modfreq2 *= pow (2.0, adsr(1) * 12 / 12.0);
    }
    if (controls->get(P_ENV_MODE_2) == ENV_PITCH_2) {
        modfreq2 *= pow (2.0, adsr(2) * controls->get(P_ENV_AMT_2) * 4 / 12.0); // Scale by 4 so it covers more
    }
    // Move Osc 2 forward correctly
    position2 += modfreq2 / rate;

    if ((status == KEY_RELEASED) &&
            (time >= controls->get(P_RELEASE)) && // Wait for envelope 1 to finish releasing regardless
            (time >= controls->get(P_RELEASE_2) || !(controls->get(P_ENV_MODE_2) == ENV_LEVEL_1  || controls->get(P_ENV_MODE_2) == ENV_LEVEL_2)) // Wait for envelope 2 if it's targeting either oscillator level
            ) {
        off();
    }
}

inline bool Key::isOn () {
    return (status != KEY_OFF);
}

#endif /* KEY_HPP_ */