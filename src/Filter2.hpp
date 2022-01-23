
#ifndef FILTER2_HPP_
#define FILTER2_HPP_

#include <math.h>
#include "Waveform.hpp"

inline float valueInNoise(float freq, float position, float cutoff_partial, float res_width, float res_height) {

    // TODO this cutoff check is a little iffy, but it works on most notes
    if (freq * 3 * cutoff_partial  > 21000) cutoff_partial = 21000.0f / freq / 3;
    float value = valueFromNoiseCache(cutoff_partial-1, position);
    if (res_height <= 0.0) return value;

    float resonance = value - valueFromNoiseCache(cutoff_partial-1-res_width, position);

    return value + resonance * res_height;
}

inline float resonatedValueInWave(Waveform waveform, float freq, float position, float cutoff_partial, float res_width, float res_height) {
    if (waveform == WAVEFORM_NOISE) return valueInNoise(freq, position, cutoff_partial, res_width, res_height);

    if (freq * cutoff_partial > 20000) cutoff_partial = 20000.0f / freq;
    float value = valueFromCache(waveform, cutoff_partial-1, position);

    if (res_height <= 0.0) return value;

    float resonance = value - valueFromCache(waveform, cutoff_partial-1-res_width, position);

    return value + resonance * res_height;
}

#endif