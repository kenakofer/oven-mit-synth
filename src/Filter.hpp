
#ifndef FILTER_HPP_
#define FILTER_HPP_

#include "Waveform.hpp"
#include "whiteband500to1k.hpp"

const float KEY_TRACK_FREQUENCY = 440.0f;
const int CACHED_WAVE_SAMPLES = 512;


// 10 partials becomes unsafe/unnecessary above 2205 hz, or A4 + 28 semitones == c# == midi note 97
// 20 partials becomes unsafe above 1103 hz, or A4 + 15 semitones == c == midi note 84
// 30 partials becomes unsafe above 735 hz, or A4 + 7 semitones == e == midi note 76

// Unsafe, because trying to render frequencies above the nyquil limit (rate/2) actually reflects the frequency downward, ghosting unwanted frequencies.
// Unnecessary, because even if it did render correctly, it wouldn't be audible.

const float PARTIAL_LIMIT_10 = 20000.0f / 10;
const float PARTIAL_LIMIT_20 = 20000.0f / 20;
const float PARTIAL_LIMIT_30 = 20000.0f / 30;

const int TOTAL_DISTANCE = 1000; // On transitions, how many samples to transition

class Filter
{
    private:
        float cutoff_diff;
        float cutoff_partial;
        float peak_freq;
        float res_height;
        Waveform waveform;
        Controls* controls;

        float cutoff_freq;
        float flat_freq;
        float slope;

        int coveredDistance;

        std::array<float, CACHED_WAVE_SAMPLES> startCachedValues06;
        std::array<float, CACHED_WAVE_SAMPLES> startCachedValues10;
        std::array<float, CACHED_WAVE_SAMPLES> startCachedValues20;
        std::array<float, CACHED_WAVE_SAMPLES> startCachedValues30;
        std::array<float, CACHED_WAVE_SAMPLES> startCachedValues40;
        std::array<float, CACHED_WAVE_SAMPLES> destCachedValues06;
        std::array<float, CACHED_WAVE_SAMPLES> destCachedValues10;
        std::array<float, CACHED_WAVE_SAMPLES> destCachedValues20;
        std::array<float, CACHED_WAVE_SAMPLES> destCachedValues30;
        std::array<float, CACHED_WAVE_SAMPLES> destCachedValues40;

    public:
        Filter();
        float attenuationForFreq(float freq);
        float attenuationForFreq(float freq, float pf);
        void setValues(Controls *c);
        float valueInWave(float freq, float pos);
        float valueInWaveNoCache(float freq, float pos, float cutoff_partial);
        float getValueInNoise(float freq, float pos);
        void proceed();

    private:
        void _recalculate_values();
        void _fillStartCacheWithCurrent();
        void _fillDestCacheWithComputed();
        void _fillDestCacheWithComputedNoise();
};

inline Filter::Filter () :
    res_height (1.2f),
    cutoff_partial (30.0f),
    cutoff_diff (1000.0f),
    coveredDistance (0),
    waveform (WAVEFORM_SINE),
    controls (nullptr)
{
    startCachedValues10.fill(0.0f);
    startCachedValues20.fill(0.0f);
    startCachedValues30.fill(0.0f);
    startCachedValues40.fill(0.0f);
    destCachedValues10.fill(0.0f);
    destCachedValues20.fill(0.0f);
    destCachedValues30.fill(0.0f);
    destCachedValues40.fill(0.0f);
    _recalculate_values();
}

inline float Filter::attenuationForFreq(float freq) {
    return attenuationForFreq(freq, peak_freq);
}
inline float Filter::attenuationForFreq(float freq, float pf) {
    float cf = pf + cutoff_diff;
    float s = res_height / (pf - cf);
    float ff = pf + (res_height - 1.0f) / s;
    if (s > 0) {
        // Hi pass
        if (freq < cf) return 0.0f;
        if (freq < pf) return s * (freq - cf);
        if (freq < ff) return res_height - (s * (freq - pf));
        else return 1.0f;
    } else {
        // Lo pass
        if (freq < ff) return 1.0f;
        if (freq < pf) return res_height - (s * (freq - pf));
        if (freq < cf) return s * (freq - cf);
        else return 0.0f;
    }
}
inline void Filter::setValues(Controls *c) {
    controls = c;
    cutoff_diff = (*controls).get(CONTROL_CUTOFF_DIFF);
    cutoff_partial = (*controls).get(CONTROL_CUTOFF);

    res_height = (*controls).get(CONTROL_RES_HEIGHT);
    waveform = static_cast<Waveform>((*controls).get(CONTROL_WAVEFORM));
    _recalculate_values();
}

inline void Filter::_recalculate_values() {
    peak_freq = KEY_TRACK_FREQUENCY * cutoff_partial;
    cutoff_freq = peak_freq + cutoff_diff;
    if (cutoff_freq < 20.0f) {
        cutoff_freq = 20.0f;
    } else if (cutoff_freq > 20000.0f) {
        cutoff_freq == 20000.0f;
    }
    slope = res_height / (peak_freq - cutoff_freq);
    flat_freq = peak_freq + (res_height - 1.0f) / slope;

    // std::cout << "New filter settings for an A440 note:" << std::endl;
    // std::cout << "Cutoff freq: " << cutoff_freq << std::endl;
    // std::cout << "Peak freq: " << peak_freq << std::endl;
    // std::cout << "res height: " << res_height << std::endl;

    _fillStartCacheWithCurrent();
    _fillDestCacheWithComputed();
    coveredDistance = 0;
}
inline void Filter::_fillStartCacheWithCurrent() {
    float destProportion = (float)coveredDistance / TOTAL_DISTANCE;
    float startProportion = 1.0f - destProportion;
    for (int n=0; n<CACHED_WAVE_SAMPLES; n++) {
        startCachedValues06[n] = startProportion * startCachedValues06[n] + destProportion * destCachedValues06[n];
        startCachedValues10[n] = startProportion * startCachedValues10[n] + destProportion * destCachedValues10[n];
        startCachedValues20[n] = startProportion * startCachedValues20[n] + destProportion * destCachedValues20[n];
        startCachedValues30[n] = startProportion * startCachedValues30[n] + destProportion * destCachedValues30[n];
        // startCachedValues40[n] = startProportion * startCachedValues40[n] + destProportion * destCachedValues40[n];
    }
}

inline void Filter::_fillDestCacheWithComputed() {
    if (waveform == WAVEFORM_NOISE) return _fillDestCacheWithComputedNoise();

    float value;
    for (int n=0; n<CACHED_WAVE_SAMPLES; n++) {
        value = 0.0f;
        for (int i=0; i<6; i++) {
            if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
            value += PARTIAL_AMPLITUDES[waveform][i] *
                    attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1)) *
                    sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

        }
        destCachedValues06[n] = value;
        for (int i=6; i<10; i++) {
            if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
            value += PARTIAL_AMPLITUDES[waveform][i] *
                    attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1)) *
                    sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

        }
        destCachedValues10[n] = value;
        for (int i=10; i<20; i++) {
            if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
            value += PARTIAL_AMPLITUDES[waveform][i] *
                    attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1)) *
                    sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

        }
        destCachedValues20[n] = value;
        for (int i=20; i<30; i++) {
            if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
            value += PARTIAL_AMPLITUDES[waveform][i] *
                    attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1)) *
                    sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

        }
        destCachedValues30[n] = value;
        // for (int i=30; i<40; i++) {
        //     if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
        //     value += PARTIAL_AMPLITUDES[waveform][i] *
        //             attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1)) *
        //             sin(2.0 * M_PI * (i+1) * ((float)n/CACHED_WAVE_SAMPLES));

        // }
        // destCachedValues40[n] = value;
    }
}

inline void Filter::_fillDestCacheWithComputedNoise() {
    // No action necessary?
}

inline void Filter::proceed() {
    if (coveredDistance < TOTAL_DISTANCE) {
        coveredDistance++;
    }
}

inline float Filter::valueInWave(float freq, float pos) {
    if (waveform == WAVEFORM_NOISE) return getValueInNoise(freq, pos);

    int i = (int)(CACHED_WAVE_SAMPLES * fmod(pos, 1.0));
    float destProportion = (float)coveredDistance / TOTAL_DISTANCE;
    float startProportion = 1.0f - destProportion;

    if (freq > PARTIAL_LIMIT_10)
        return (startCachedValues06[i] * startProportion + destCachedValues06[i] * destProportion);
    if (freq > PARTIAL_LIMIT_20)
        return (startCachedValues10[i] * startProportion + destCachedValues10[i] * destProportion);
    if (freq > PARTIAL_LIMIT_30)
        return (startCachedValues20[i] * startProportion + destCachedValues20[i] * destProportion);
    // if (freq > PARTIAL_LIMIT_40) return cachedValues30[i];
    return (startCachedValues30[i] * startProportion + destCachedValues30[i] * destProportion);
}

inline float Filter::valueInWaveNoCache(float freq, float pos, float peak_part) {
    float pf = KEY_TRACK_FREQUENCY * peak_part;
    float value = 0.0f;
    for (int i=0; i<20; i++) {
        if (PARTIAL_AMPLITUDES[waveform][i] == 0.0f) continue;
        if ((freq * (i + 1)) > 20000) break;
        value += PARTIAL_AMPLITUDES[waveform][i] *
                attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1), pf) *
                sin(2.0 * M_PI * (i+1) * pos);
    }
    return value;
}

inline float Filter::getValueInNoise(float freq, float pos) {
    float samples_per_cycle = 44100.0f / 750;

    float value = 0.0f;
    float i=0.0f;
    while (i < 20) {
        if ((freq * (i+1)) > (44100 / 2)) {
            break;
        }
        int index = ((int)(samples_per_cycle * pos * (i+1))) % WHITEBAND500TO1K_LENGTH;
        value += WHITEBAND500TO1K_SAMPLES[index] * attenuationForFreq(KEY_TRACK_FREQUENCY * (i+1));
        if (i==0.0f) i += .85f;
        else i += .95;
    }
    return value;
}

#endif