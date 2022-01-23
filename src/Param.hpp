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
    P_INSTANCE   = 22,
};

static const int P_NUM_CONTROLS = 22; // The P_INSTANCE param is only used in unity's communication
static const int P_NUM_UNITY = 23;

constexpr std::array<std::pair<float, float>, P_NUM_CONTROLS> controlLimit =
{{
    {0.0f, 3.0f},           // ENV_MODE_1
    {0.001f, 4.0f},         // ATTACK
    {0.001f, 4.0f},         // DECAY
    {0.0f, 1.0f},           // SUSTAIN
    {0.001f, 4.0f},         // RELEASE

    {0.0f, 4.0f},           // WAVEFORM
    {0.0f, 1.0f},           // LEVEL
    {-24.0f, 24.0f},        // PITCH

    {0.0f, 1.0f},           // FILTER
    {0.0f, 60.0f},          // CUTOFF
    {0.0f, 8.0f},           // RES_HEIGHT
    {1.0f, 30.0f},          // RES_WIDTH

    {0.0f, 3.0f},           // ENV MODE 2
    {0.0f, 10.0f},          // ENV_AMT_2
    {0.001f, 4.0f},         // ATTACK
    {0.001f, 4.0f},         // DECAY
    {0.0f, 1.0f},           // SUSTAIN
    {0.001f, 4.0f},         // RELEASE

    {0.0f, 3.0f},           // WAVEFORM 2 MODE
    {0.0f, 4.0f},           // WAVEFORM 2
    {0.0f, 1.0f},           // LEVEL 2
    {-24.0f, 24.0f},        // PITCH 2
}};

#endif