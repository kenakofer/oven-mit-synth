#ifndef KEY_HPP_
#define KEY_HPP_

#include <cstdint>
#include <cmath>
#include <random>
#include <ctime>
#include <array>
#include <iostream>

#include "Waveform.hpp"
#include "KeyStatus.hpp"
#include "Sine.hpp"
#include "Filter2.hpp"
#include "Controls.hpp"

class Key
{
private:
    KeyStatus status;
    Waveform waveform;
    uint8_t note;
    uint8_t velocity;
    double rate;
    double position;
    double position2;
    float start_level_1;
    float start_level_2;
    double freq;
    double freq2;
    double time;
    std::minstd_rand rnd;
    std::uniform_real_distribution<float> dist;
    Controls* controls;
    Filter* filter;

public:
    Key ();
    Key (const double rt);
    void press (const uint8_t nt, const uint8_t vel, Controls *c, Filter *f);
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
    waveform (WAVEFORM_SINE),
    note (0),
    velocity (0),
    rate (rt),
    position (0.0),
    position2 (0.0),
    start_level_1 (0.0f),
    start_level_2 (0.0f),
    freq (pow (2.0, (static_cast<double> (note) - 69.0) / 12.0) * 440.0),
    time (0.0),
    rnd (std::time (0)),
    dist (-1.0f, 1.0f),
    filter ()
{

}

inline void Key::press (const uint8_t nt, const uint8_t vel, Controls *c, Filter *f)
{

    controls = c;
    start_level_1 = adsr(1);
    start_level_2 = adsr(2);
    note = nt;
    velocity = vel;
    freq = pow (2.0, (static_cast<double> (note) - 69.0) / 12.0) * 440.0;
    freq2 = pow (2.0, (static_cast<double> (note) - 69.0 + (*controls).get(CONTROL_PITCH_2)) / 12.0) * 440.0;
    time = 0.0;
    status = KEY_PRESSED;
    filter = f;
    // std::cout << "Starting note with freq: " << freq << std::endl;
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
            attack = (*controls).get(CONTROL_ATTACK);
            decay = (*controls).get(CONTROL_DECAY);
            sustain = (*controls).get(CONTROL_SUSTAIN);
            release = (*controls).get(CONTROL_RELEASE);
            start_level = start_level_1;
            break;
        case (2):
            attack = (*controls).get(CONTROL_ATTACK_2);
            decay = (*controls).get(CONTROL_DECAY_2);
            sustain = (*controls).get(CONTROL_SUSTAIN_2);
            release = (*controls).get(CONTROL_RELEASE_2);
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
    // return (*filter).valueInWave(freq, position);

    float cutoff_partial = (*controls).get(CONTROL_CUTOFF);
    if ((*controls).get(CONTROL_ENV_MODE_1) == ENV_CUTOFF_1) {
        cutoff_partial += 4 * adsr(1);
    }
    if ((*controls).get(CONTROL_ENV_MODE_2) == ENV_CUTOFF_1) {
        cutoff_partial += 4 * adsr(2) * (*controls).get(CONTROL_ENV_AMT_2);
    }
    if ((*controls).get(CONTROL_WAVEFORM_2_MODE) == OSC_CUTOFF_1) {
        cutoff_partial += 4 * synth2();
    }
    return (*filter).resonatedValueInWave(freq, position, cutoff_partial, (*controls).get(CONTROL_RES_WIDTH), (*controls).get(CONTROL_RES_HEIGHT));

}

inline float Key::synth2()
{
    float value = valueInWaveform(static_cast<Waveform> ((*controls).get(CONTROL_WAVEFORM_2)), position2);

    if ((*controls).get(CONTROL_ENV_MODE_1) == ENV_LEVEL_2) value *= adsr(1);
    if ((*controls).get(CONTROL_ENV_MODE_2) == ENV_LEVEL_2) value *= adsr(2);

    value *= (*controls).get(CONTROL_LEVEL_2);
    return value;
}

inline float Key::get ()
{
    float value = synthPartials() * // Synth partials is 0 for some reason!!!
                    (*controls).get(CONTROL_LEVEL);

    if ((*controls).get(CONTROL_ENV_MODE_1) == ENV_LEVEL_1) {
        value *= adsr(1);
    }
    if ((*controls).get(CONTROL_ENV_MODE_2) == ENV_LEVEL_1) {
        value *= adsr(2);
    }
    if ((*controls).get(CONTROL_WAVEFORM_2_MODE) == OSC_AM_1) {
        value *= 1 + synth2();
    }

    if ((*controls).get(CONTROL_WAVEFORM_2_MODE) == OSC_ADD) {
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

    // Find oscillator 1's freq:
    float modfreq = freq;
    modfreq *= pow (2.0, (*controls).get(CONTROL_PITCH) / 12.0);
    if ((*controls).get(CONTROL_ENV_MODE_1) == ENV_PITCH_1) {
        modfreq *= pow (2.0, adsr(1) * 12 / 12.0);
    }
    if ((*controls).get(CONTROL_ENV_MODE_2) == ENV_PITCH_1) {
        modfreq *= pow (2.0, adsr(2) * (*controls).get(CONTROL_ENV_AMT_2) * 4 / 12.0); // Scale by 4 so it covers more
    }
    if ((*controls).get(CONTROL_WAVEFORM_2_MODE) == OSC_FM_1) {
        modfreq *= 1 + synth2();
    }
    // Move Osc 1 forward correctly
    position += modfreq / rate;

    // Move Osc 2 forward correctly
    position2 += freq2 / rate;

    if ((status == KEY_RELEASED) &&
            (time >= (*controls).get(CONTROL_RELEASE)) &&
            (time >= (*controls).get(CONTROL_RELEASE_2))) {
        off();
    }
}

inline bool Key::isOn () {
    return (status != KEY_OFF);
}

#endif /* KEY_HPP_ */