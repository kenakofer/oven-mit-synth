#ifndef WAVEFORM_HPP_
#define WAVEFORM_HPP_

#include <array>

enum Waveform
{
    WAVEFORM_SINE       = 0,
    WAVEFORM_TRIANGLE   = 1,
    WAVEFORM_SQUARE     = 2,
    WAVEFORM_SAW        = 3,
    WAVEFORM_NOISE      = 4,
    WAVEFORM_NR         = 5
};

const int PARTIAL_NR = 50;


constexpr std::array<std::array<float, PARTIAL_NR>, WAVEFORM_NR> PARTIAL_AMPLITUDES =
{{
    {1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
    {1.00f, 0.00f, -1.0f/9, 0.00f, 1.0f/25, 0.00f, -1.0f/49, 0.00f, 1.0f/81, 0.00f, -1.0f/121, 0.00f, 1.0f/169, 0.00f, -1.0f/225, 0.00f, 1.0f/289, 0.00f, -1.0f/361, 0.00f, 1.0f/441, 0.00f, -1.0f/529, 0.00f, 1.0f/625, 0.00f, -1.0f/729, 0.00f, 1.0f/841, 0.00f, 1.0f/(31*31), 0.00f, -1.0f/(33*33), 0.00f, 1.0f/(35*35), 0.00f, -1.0f/(37*37), 0.00f, 1.0f/(39*39), 0.00f, 1.0f/(41*41), 0.00f, -1.0f/(43*43), 0.00f, 1.0f/(45*45), 0.00f, -1.0f/(47*47), 0.00f, 1.0f/(49*49), 0.00f},
    {1.00f, 0.00f, 1.0f/3, 0.00f, 1.0f/5, 0.00f, 1.0f/7, 0.00f, 1.0f/9, 0.00f, 1.0f/11, 0.00f, 1.0f/13, 0.00f, 1.0f/15, 0.00f, 1.0f/17, 0.00f, 1.0f/19, 0.00f, 1.0f/21, 0.00f, 1.0f/23, 0.00f, 1.0f/25, 0.00f, 1.0f/27, 0.00f, 1.0f/29, 0.00f, 1.0f/31, 0.00f, 1.0f/33, 0.00f, 1.0f/35, 0.00f, 1.0f/37, 0.00f, 1.0f/39, 0.00f, 1.0f/41, 0.00f, 1.0f/43, 0.00f, 1.0f/45, 0.00f, 1.0f/47, 0.00f, 1.0f/49, 0.00f},
    {1.00f, -1.0f/2, 1.0f/3, -1.0f/4, 1.0f/5, -1.0f/6, 1.0f/7, -1.0f/8, 1.0f/9, -1.0f/10, 1.0f/11, -1.0f/12, 1.0f/13, -1.0f/14, 1.0f/15, -1.0f/16, 1.0f/17, -1.0f/18, 1.0f/19, -1.0f/20 , 1.0f/21, -1.0f/22, 1.0f/23, -1.0f/24, 1.0f/25, -1.0f/26, 1.0f/27, -1.0f/28, 1.0f/29, -1.0f/30, 1.0f/31, -1.0f/32, 1.0f/33, -1.0f/34, 1.0f/35, -1.0f/36, 1.0f/37, -1.0f/38, 1.0f/39, -1.0f/40, 1.0f/41, -1.0f/42, 1.0f/43, -1.0f/44, 1.0f/45, -1.0f/46, 1.0f/47, -1.0f/48, 1.0f/49, -1.0f/50}
}};

inline float getPartialAmplitude(const Waveform waveform, const int partial) {
    if (partial <= PARTIAL_NR) return PARTIAL_AMPLITUDES[waveform][partial-1];

    switch (waveform)
    {
        case WAVEFORM_SINE:     return partial == 1 ? 1.0f : 0.0f;
        case WAVEFORM_TRIANGLE: return (partial % 2 == 0) ? 0.0f : (partial % 4 == 3 ? (-1.0f / partial / partial) : (1.0f / partial / partial));
        case WAVEFORM_SQUARE:   return (partial % 2 == 0) ? 0.0f : (1.0f / partial);
        case WAVEFORM_SAW:      return (partial % 2 == 0) ? (-1.0f / partial) : (1.0f / partial);
        default:                return 0.0f;
    }
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