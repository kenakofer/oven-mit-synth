#ifndef PARAM_HPP_
#define PARAM_HPP_

enum Param
{
    P_ENV_MODE_1 = 0,
    P_ATTACK     = 1,
    P_DECAY      = 2,
    P_SUSTAIN    = 3,
    P_RELEASE    = 4,

    P_WAVEFORM   = 5,
    P_LEVEL      = 6,
    P_PITCH      = 7,

    P_FILTER     = 8,
    P_CUTOFF     = 9,
    P_RES_HEIGHT = 10,
    P_RES_WIDTH  = 11,

    P_ENV_MODE_2 = 12,
    P_ENV_AMT_2  = 13,
    P_ATTACK_2   = 14,
    P_DECAY_2    = 15,
    P_SUSTAIN_2  = 16,
    P_RELEASE_2  = 17,

    P_WAVEFORM_2_MODE = 18,
    P_WAVEFORM_2 = 19,
    P_LEVEL_2    = 20,
    P_PITCH_2    = 21,
    P_KEYTRACK_2 = 22,

    P_VOICE_MODE = 23,
    P_PORTAMENTO = 24,

    P_LEGATO = 25,

    P_NUM_CONTROLS = 26
};

constexpr std::array<std::pair<float, float>, P_NUM_CONTROLS> PARAM_LIMIT =
{{
    {0.0f, 4.0f},           // ENV_MODE_1
    {0.001f, 4.0f},         // ATTACK
    {0.001f, 4.0f},         // DECAY
    {0.0f, 1.0f},           // SUSTAIN
    {0.001f, 4.0f},         // RELEASE

    {0.0f, 4.0f},           // WAVEFORM
    {0.0f, 1.0f},           // LEVEL
    {-24.0f, 24.0f},        // PITCH

    {0.0f, 1.0f},           // FILTER
    {0.0f, 200.0f},         // CUTOFF
    {-1.0f, 8.0f},           // RES_HEIGHT
    {1.0f, 30.0f},          // RES_WIDTH

    {0.0f, 4.0f},           // ENV MODE 2
    {0.0f, 10.0f},          // ENV_AMT_2
    {0.001f, 4.0f},         // ATTACK
    {0.001f, 4.0f},         // DECAY
    {0.0f, 1.0f},           // SUSTAIN
    {0.001f, 4.0f},         // RELEASE

    {0.0f, 3.0f},           // WAVEFORM 2 MODE
    {0.0f, 4.0f},           // WAVEFORM 2
    {0.0f, 1.0f},           // LEVEL 2
    {-48.0f, 24.0f},        // PITCH 2
    {0.0f, 1.0f},           // KEYTRACK 2

    {0.0f, 2.0f},           // VOICE MODE
    {1.00f, 100.0f},        // PORTAMENTO

    {0.00f, 1.0f},          // LEGATO
}};

constexpr std::array<float, P_NUM_CONTROLS> PARAM_DEFAULT =
{
    0,           // ENV_MODE_1
    0.001,         // ATTACK
    0.1,         // DECAY
    0.5,           // SUSTAIN
    0.1,         // RELEASE

    0,           // WAVEFORM
    .1,           // LEVEL
    0,        // PITCH

    0,           // FILTER
    20,          // CUTOFF
    0,           // RES_HEIGHT
    3,          // RES_WIDTH

    3,           // ENV MODE 2
    3,          // ENV_AMT_2
    0.001,         // ATTACK
    .1,         // DECAY
    0,           // SUSTAIN
    .1,         // RELEASE

    0,           // WAVEFORM 2 MODE
    2,           // WAVEFORM 2
    .1,           // LEVEL 2
    -12,        // PITCH 2
    1,        // KEYTRACK 2

    0,           // VOICE MODE
    1,        // PORTAMENTO

    1,        // LEGATO
};

const char* PARAM_NAME[P_NUM_CONTROLS] = {
    "EnvMode1",
    "Attack1",
    "Decay1",
    "Sustain1",
    "Release1",

    "Waveform1",
    "Level1",
    "Pitch1",

    "Filter1",
    "Cutoff1",
    "Resonance1",
    "ResonanceWidth1",

    "EnvMode2",
    "EnvAmount2",
    "Attack2",
    "Decay2",
    "Sustain2",
    "Release2",

    "Oscillator2Mode",
    "Waveform2",
    "Level2",
    "Pitch2",
    "Keytrack2",

    "VoiceMode",
    "Portamento",

    "Legato",
};

#endif