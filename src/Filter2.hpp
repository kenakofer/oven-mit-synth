
#ifndef FILTER2_HPP_
#define FILTER2_HPP_

#include <math.h>
#include "Waveform.hpp"
#include "whiteband500to1k.hpp"

const int CACHED_WAVE_SAMPLES = 512;


// 10 partials becomes unsafe/unnecessary above 2205 hz, or A4 + 28 semitones == c# == midi note 97
// 20 partials becomes unsafe above 1103 hz, or A4 + 15 semitones == c == midi note 84
// 30 partials becomes unsafe above 735 hz, or A4 + 7 semitones == e == midi note 76

// Unsafe, because trying to render frequencies above the nyquil limit (rate/2) actually reflects the frequency downward, ghosting unwanted frequencies.
// Unnecessary, because even if it did render correctly, it wouldn't be audible.

const float PARTIAL_LIMIT_10 = 22000.0f / 10;
const float PARTIAL_LIMIT_20 = 22000.0f / 20;
const float PARTIAL_LIMIT_30 = 22000.0f / 30;
const float PARTIAL_LIMIT_40 = 22000.0f / 40;
const float PARTIAL_LIMIT_50 = 22000.0f / 50;

class Filter
{
private:
    Waveform waveform;

    std::array<float, CACHED_WAVE_SAMPLES> cachedValues01;
    std::array<float, CACHED_WAVE_SAMPLES> cachedValues06;
    std::array<float, CACHED_WAVE_SAMPLES> cachedValues10;
    std::array<float, CACHED_WAVE_SAMPLES> cachedValues20;
    std::array<float, CACHED_WAVE_SAMPLES> cachedValues30;
    std::array<float, CACHED_WAVE_SAMPLES> cachedValues40;
    std::array<float, CACHED_WAVE_SAMPLES> cachedValues50;
    // std::array<float, CACHED_WAVE_SAMPLES> cachedValues40;

public:
    inline Filter()
    {
        setWaveform(WAVEFORM_SINE);
    }
    inline Filter(Waveform w)
    {
        setWaveform(w);
    }

    inline void setWaveform(Waveform w) {
        waveform = w;
    }

    inline float resonatedValueInWave(float freq, float position, float cutoff_partial, float res_width, float res_height) {
        if (waveform == WAVEFORM_NOISE) return valueInNoise(freq, position, cutoff_partial);

        const float pos = fmod(position, 1.0);

        if (freq * cutoff_partial > 20000) cutoff_partial = 20000.0f / freq;
        float value = valueFromCache(waveform, cutoff_partial-1, pos);

        if (res_height <= 0.0) return value;

        float resonance = value - valueFromCache(waveform, cutoff_partial-1-res_width, pos);

        return value + resonance * res_height;
    }

    inline float valueInNoise(float freq, float position, float cutoff_partial) {
        const float samples_per_cycle = 44100.0f / 750;

        float value = 0.0f;
        float i=0.0f;
        while (i < 20) {
            if ((freq * (i+1)) > (44100 / 2)) break;
            if (i >= cutoff_partial) break;

            int index = ((int)(samples_per_cycle * position * (i+1))) % WHITEBAND500TO1K_LENGTH;
            float factor = 1.0;
            if ((cutoff_partial - 1 < (i-1)) && ((i-1) < cutoff_partial)) {
                factor = 1.0f - (cutoff_partial - (i-1));
            }
            value += WHITEBAND500TO1K_SAMPLES[index] * factor;
            if (i==0.0f) i += .85f;
            else i += .95;
        }
        return value;
    }

};

#endif