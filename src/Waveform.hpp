#ifndef WAVEFORM_HPP_
#define WAVEFORM_HPP_

#include <array>
#include "ConstCache.hpp"
// #include "whiteband500to1k.hpp"
#include "whiteband1000to1100.hpp"
#include "whiteband1000to1500.hpp"
#include "whiteband1000to4000.hpp"

enum Waveform
{
    WAVEFORM_SINE       = 0,
    WAVEFORM_TRIANGLE   = 1,
    WAVEFORM_SQUARE     = 2,
    WAVEFORM_SAW        = 3,
    WAVEFORM_NOISE      = 4,
    WAVEFORM_NR         = 5
};

// constexpr std::array<std::array<float, PARTIAL_NR>, WAVEFORM_NR> PARTIAL_AMPLITUDES =
// {{
//     {1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
//     {1.00f, 0.00f, -1.0f/9, 0.00f, 1.0f/25, 0.00f, -1.0f/49, 0.00f, 1.0f/81, 0.00f, -1.0f/121, 0.00f, 1.0f/169, 0.00f, -1.0f/225, 0.00f, 1.0f/289, 0.00f, -1.0f/361, 0.00f, 1.0f/441, 0.00f, -1.0f/529, 0.00f, 1.0f/625, 0.00f, -1.0f/729, 0.00f, 1.0f/841, 0.00f, 1.0f/(31*31), 0.00f, -1.0f/(33*33), 0.00f, 1.0f/(35*35), 0.00f, -1.0f/(37*37), 0.00f, 1.0f/(39*39), 0.00f, 1.0f/(41*41), 0.00f, -1.0f/(43*43), 0.00f, 1.0f/(45*45), 0.00f, -1.0f/(47*47), 0.00f, 1.0f/(49*49), 0.00f},
//     {1.00f, 0.00f, 1.0f/3, 0.00f, 1.0f/5, 0.00f, 1.0f/7, 0.00f, 1.0f/9, 0.00f, 1.0f/11, 0.00f, 1.0f/13, 0.00f, 1.0f/15, 0.00f, 1.0f/17, 0.00f, 1.0f/19, 0.00f, 1.0f/21, 0.00f, 1.0f/23, 0.00f, 1.0f/25, 0.00f, 1.0f/27, 0.00f, 1.0f/29, 0.00f, 1.0f/31, 0.00f, 1.0f/33, 0.00f, 1.0f/35, 0.00f, 1.0f/37, 0.00f, 1.0f/39, 0.00f, 1.0f/41, 0.00f, 1.0f/43, 0.00f, 1.0f/45, 0.00f, 1.0f/47, 0.00f, 1.0f/49, 0.00f},
//     {1.00f, -1.0f/2, 1.0f/3, -1.0f/4, 1.0f/5, -1.0f/6, 1.0f/7, -1.0f/8, 1.0f/9, -1.0f/10, 1.0f/11, -1.0f/12, 1.0f/13, -1.0f/14, 1.0f/15, -1.0f/16, 1.0f/17, -1.0f/18, 1.0f/19, -1.0f/20 , 1.0f/21, -1.0f/22, 1.0f/23, -1.0f/24, 1.0f/25, -1.0f/26, 1.0f/27, -1.0f/28, 1.0f/29, -1.0f/30, 1.0f/31, -1.0f/32, 1.0f/33, -1.0f/34, 1.0f/35, -1.0f/36, 1.0f/37, -1.0f/38, 1.0f/39, -1.0f/40, 1.0f/41, -1.0f/42, 1.0f/43, -1.0f/44, 1.0f/45, -1.0f/46, 1.0f/47, -1.0f/48, 1.0f/49, -1.0f/50}
// }};

// inline float getPartialAmplitude(const Waveform waveform, const int partial) {
//     if (partial <= PARTIAL_NR) return PARTIAL_AMPLITUDES[waveform][partial-1];

//     switch (waveform)
//     {
//         case WAVEFORM_SINE:     return partial == 1 ? 1.0f : 0.0f;
//         case WAVEFORM_TRIANGLE: return (partial % 2 == 0) ? 0.0f : (partial % 4 == 3 ? (-1.0f / partial / partial) : (1.0f / partial / partial));
//         case WAVEFORM_SQUARE:   return (partial % 2 == 0) ? 0.0f : (1.0f / partial);
//         case WAVEFORM_SAW:      return (partial % 2 == 0) ? (-1.0f / partial) : (1.0f / partial);
//         default:                return 0.0f;
//     }
// }

const float NYQUIST_FREQ = 21000; // Even at higher framerates, no sense generating beyond hearing range

inline float valueFromCache(Waveform waveform, float partial_index, float position) {
    if (partial_index >= CACHE_PARTIALS-1) partial_index = CACHE_PARTIALS - 1 - 0.01f;
    if (partial_index < 0) partial_index = 0.0f;
    position = fmod(position, 1.0);

    const int i = (int)(CACHE_SAMPLES * position);
    const int bottom_partial = (int)partial_index;
    const float p2 = partial_index - bottom_partial;
    const float p1 = 1.0f - p2;

    // std::cout << "Actual partial: " << partial_index << std::endl;
    switch (waveform)
    {
        case WAVEFORM_SINE:     return p1 * CACHE_SINE[bottom_partial][i] + p2 * CACHE_SINE[bottom_partial+1][i];
        case WAVEFORM_TRIANGLE: return 3 * (p1 * CACHE_TRIANGLE[bottom_partial][i] + p2 * CACHE_TRIANGLE[bottom_partial+1][i]);
        case WAVEFORM_SQUARE:   return p1 * CACHE_SQUARE[bottom_partial][i] + p2 * CACHE_SQUARE[bottom_partial+1][i];
        case WAVEFORM_SAW:      return p1 * CACHE_SAW[bottom_partial][i] + p2 * CACHE_SAW[bottom_partial+1][i];
        default:                return 0.0f;
    }
}

const float SAMPLES_PER_THIN_NOISE_CYCLE = 44100.0f / 262.5;
inline float valueFromThinNoiseCache(float position) {
    int i = (int)(WHITEBAND1000TO1100_LENGTH * position / SAMPLES_PER_THIN_NOISE_CYCLE);
    i %= WHITEBAND1000TO1100_LENGTH;

    return CACHE_WHITEBAND1000TO1100[0][i];

}
const float SAMPLES_PER_THICK_NOISE_CYCLE = 44100.0f / 250;
inline float valueFromThickNoiseCache(float position) {
    int i = (int)(WHITEBAND1000TO1500_LENGTH * position / SAMPLES_PER_THICK_NOISE_CYCLE);
    i %= WHITEBAND1000TO1500_LENGTH;

    return CACHE_WHITEBAND1000TO1500[0][i];

}
const float SAMPLES_PER_THICKER_NOISE_CYCLE = 44100.0f / 250;
inline float valueFromThickerNoiseCache(float position) {
    int i = (int)(WHITEBAND1000TO4000_LENGTH * position / SAMPLES_PER_THICKER_NOISE_CYCLE);
    i %= WHITEBAND1000TO4000_LENGTH;

    return .6 * CACHE_WHITEBAND1000TO4000[0][i];

}

inline float noiseBand(float bottom_freq, float partial_index, float position) {
    float value = 0.0f;
    float pfreq = bottom_freq;
    float top_freq = bottom_freq * (partial_index + 1.1);
    if (top_freq > NYQUIST_FREQ * .4) top_freq = NYQUIST_FREQ * .4;

    float p = 1.0f;
    while (pfreq < top_freq) {
        if (pfreq > top_freq) break;

        float ratio = top_freq / pfreq;

        if (ratio > 4.5) {
            value += valueFromThickerNoiseCache(position * p);
            p *= 3.9f;
        } else if (ratio > 1.6) {
            value += valueFromThickNoiseCache(position * p);
            p *= 1.3f;
        } else {
            if (ratio > 1.33) {
                value += valueFromThinNoiseCache(position * p);
            } else {
                value += (ratio - 1.0) * 3 * valueFromThinNoiseCache(position * p);
            }
            p *= 1.12f;
        }
        pfreq = bottom_freq * p;
    }
    return value;

}

inline float valueInWaveform(Waveform waveform, double position) {
    if (waveform == WAVEFORM_SAW) position += .5; // To line up with the partial amplitudes above

    const float p = fmod (position, 1.0);
    switch (waveform)
    {
        case WAVEFORM_SINE:     return sin (2.0 * M_PI * p);
        case WAVEFORM_TRIANGLE: return 1.23f * (p < 0.25f ? 4.0f * p : (p < 0.75f ? 1.0f - 4.0 * (p - 0.25f) : -1.0f + 4.0f * (p - 0.75f)));
        case WAVEFORM_SQUARE:   return 0.80f * (p < 0.5f ? 1.0f : -1.0f);
        case WAVEFORM_SAW:      return 1.58f * (2.0f * p - 1.0f);
        default:                return 0.0f;
    }
}

#endif /* WAVEFORM_HPP_ */