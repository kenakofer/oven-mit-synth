
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
        cachedValues01.fill(0.0f);
        cachedValues06.fill(0.0f);
        cachedValues10.fill(0.0f);
        cachedValues20.fill(0.0f);
        cachedValues30.fill(0.0f);
        cachedValues40.fill(0.0f);
        cachedValues50.fill(0.0f);
    }

    inline void setWaveform(Waveform w) {
        waveform = w;
        fillCache();
    }

    inline void fillCache() {
        float value;
        for (int n=0; n<CACHED_WAVE_SAMPLES; n++) {
            value = 0.0f;
            for (int i=0; i<1; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues01[n] = value;
            for (int i=1; i<6; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues06[n] = value;
            for (int i=6; i<10; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues10[n] = value;
            for (int i=10; i<20; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues20[n] = value;
            for (int i=20; i<30; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues30[n] = value;
            for (int i=30; i<40; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues40[n] = value;
            for (int i=40; i<50; i++) {
                if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
                value += PARTIAL_AMPLITUDES[waveform][i] *
                        sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

            }
            cachedValues50[n] = value;
        }
    }

    inline float resonatedValueInWave(float freq, float position, float cutoff_partial, float res_width, float res_height) {
        float value = valueInWave(freq, position, cutoff_partial);
        if (res_height <= 0.0) return value;

        int bottom = (int)(cutoff_partial - res_width*2) + 1;
        if (bottom <= 1) bottom = 1; // We don't want to amplify the fundamental maybe
        const int top = (int)cutoff_partial + 1;
        const float center = cutoff_partial - res_width;
        const float pos = fmod(position, 1.0);

        for (int p = bottom; p < top; p++) {
            // if (p >= PARTIAL_NR) break;
            if (freq * (p+1) > 20000) break;

            value += sin(2.0 * M_PI * (p+1) * pos) *
                        getPartialAmplitude(waveform, p+1) *
                        res_height * (1 - (abs(center - p) / res_width));
        }
        return value;
    }

    inline float valueInWave(float freq, float position, float cutoff_partial) {
        if (waveform == WAVEFORM_NOISE) return valueInNoise(freq, position, cutoff_partial);

        const float pos = fmod(position, 1.0);
        const int i = (int)(CACHED_WAVE_SAMPLES * pos);

        if (freq > PARTIAL_LIMIT_10 && cutoff_partial > 6.0f) return cachedValues06[i];
        if (freq > PARTIAL_LIMIT_20 && cutoff_partial > 10.0f) return cachedValues10[i];
        if (freq > PARTIAL_LIMIT_30 && cutoff_partial > 20.0f) return cachedValues20[i];
        if (freq > PARTIAL_LIMIT_40 && cutoff_partial > 30.0f) return cachedValues30[i];
        if (freq > PARTIAL_LIMIT_50 && cutoff_partial > 40.0f) return cachedValues40[i];

        float p1, p2;

        if (cutoff_partial < 1) {
            return cutoff_partial * cachedValues01[i];
        } else if (cutoff_partial < 6) {
            p2 = (cutoff_partial - 1) / 5;
            p1 = 1.0f - p2;
            return (p1 * cachedValues01[i]) + (p2 * cachedValues06[i]);
        } else if (cutoff_partial < 10) {
            p2 = (cutoff_partial - 6) / 4;
            p1 = 1.0f - p2;
            return (p1 * cachedValues06[i]) + (p2 * cachedValues10[i]);
        } else if (cutoff_partial < 20) {
            p2 = (cutoff_partial - 10) / 10;
            p1 = 1.0f - p2;
            return (p1 * cachedValues10[i]) + (p2 * cachedValues20[i]);
        } else if (cutoff_partial < 30) {
            p2 = (cutoff_partial - 20) / 10;
            p1 = 1.0f - p2;
            return (p1 * cachedValues20[i]) + (p2 * cachedValues30[i]);
        } else if (cutoff_partial < 40) {
            p2 = (cutoff_partial - 30) / 10;
            p1 = 1.0f - p2;
            return (p1 * cachedValues30[i]) + (p2 * cachedValues40[i]);
        } else if (cutoff_partial < 50) {
            p2 = (cutoff_partial - 40) / 10;
            p1 = 1.0f - p2;
            return (p1 * cachedValues40[i]) + (p2 * cachedValues50[i]);
        } else {
            p2 = (cutoff_partial - 50) / 10;
            p1 = 1.0f - p2;
            return (p1 * cachedValues50[i]) + (p2 * valueInWaveform(waveform, pos)); //Average with the perfect waveform type
        }
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