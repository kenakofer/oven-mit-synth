
#ifndef FILTER2_HPP_
#define FILTER2_HPP_

#include <math.h>
#include "Waveform.hpp"

inline float lowPassInWave(Waveform waveform, float freq, float position, float cutoff_partial, float res_width, float res_height) {
    if (waveform == WAVEFORM_NOISE) return lowPassNoise(freq, cutoff_partial, position);

    float max_cutoff_partial = NYQUIST_FREQ / freq - 1;
    if (cutoff_partial > max_cutoff_partial) cutoff_partial = max_cutoff_partial;
    float value = valueFromCache(waveform, cutoff_partial-1, position);

    if (res_height == 0.0) return value;

    float resonance = value - valueFromCache(waveform, cutoff_partial-1-res_width, position);

    return value + resonance * res_height;
}

inline float highPassInWave(Waveform waveform, float freq, float position, float cutoff_partial, float res_width, float res_height) {
    if (waveform == WAVEFORM_NOISE) return highPassNoise(freq, cutoff_partial, position);

    float max_cutoff_partial = NYQUIST_FREQ / freq; // Note this is different than lowPassInWave.
    if (cutoff_partial > max_cutoff_partial) cutoff_partial = max_cutoff_partial;
    float max_value = valueFromCache(waveform, max_cutoff_partial, position);
    // float max_value = valueFromCache(waveform, max_cutoff_partial-1, position);
    float value = max_value - valueFromCache(waveform, cutoff_partial-1, position);

    if (res_height == 0.0) return value;

    float resonance = value - (max_value - valueFromCache(waveform, cutoff_partial-1+res_width, position));

    return value + resonance * res_height;
}


#endif