using System;

namespace OvenMit
{
    public static class Param
    {
        public enum SynthParameters {
            P_ENV_MODE_1,
            P_ATTACK    ,
            P_DECAY     ,
            P_SUSTAIN   ,
            P_RELEASE   ,

            P_WAVEFORM  ,
            P_LEVEL     ,
            P_PITCH     ,

            P_FILTER    ,
            P_CUTOFF    ,
            P_RES_HEIGHT,
            P_RES_WIDTH ,

            P_ENV_MODE_2,
            P_ENV_AMT_2 ,
            P_ATTACK_2  ,
            P_DECAY_2   ,
            P_SUSTAIN_2 ,
            P_RELEASE_2 ,

            P_WAVEFORM_2_MODE,
            P_WAVEFORM_2,
            P_LEVEL_2,
            P_PITCH_2,
            P_KEYTRACK_2,

            P_VOICE_MODE,
            P_PORTAMENTO,

            P_LEGATO
        };
        public static readonly int P_NUM = Enum.GetValues( typeof( SynthParameters ) ).Length;

        public static readonly string[] PARAMETER_SYMBOLS = {
            "env_mode_1",
            "attack",
            "decay",
            "sustain",
            "release",
            "waveform",
            "level",
            "pitch1",
            "filter",
            "cutoff",
            "res_height",
            "res_width",
            "env_mode_2",
            "env_amount_2",
            "attack2",
            "decay2",
            "sustain2",
            "release2",
            "waveform2mode",
            "waveform2",
            "level2",
            "pitch2",
            "keytrack2",
            "voice_mode",
            "portamento",
            "legato"
        };
    }
}