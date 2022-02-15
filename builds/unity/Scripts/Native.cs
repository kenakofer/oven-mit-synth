using UnityEngine;
using System;
using System.Runtime.InteropServices;


namespace OvenMit
{
    public static class Native
    {
        [DllImport("OvenMit")]
        public static extern void OvenMit_TestKeyPress();

        [DllImport("OvenMit")]
        public static extern void OvenMit_StartNote(int instance, int midiNote, int velocity);

        [DllImport("OvenMit")]
        public static extern void OvenMit_ReleaseNote(int instance, int midiNote);

        [DllImport("OvenMit")]
        public static extern void OvenMit_StopAllNotes(int instance);

        [DllImport("OvenMit")]
        public static extern void OvenMit_StopAllSounds(int instance);

        [DllImport("OvenMit")]
        public static extern long OvenMit_GetTimeInSamples();

        [DllImport("OvenMit")]
        public static extern void OvenMit_ScheduleNote(int instance, int midiNote, int velocity, double startbeat, double endbeat);

        [DllImport("OvenMit")]
        public static extern void OvenMit_SetSynthParameter(int instance, int parameterIndex, float value);

        [DllImport("OvenMit")]
        public static extern void OvenMit_SetSynthPan(int instance_index, float pan, int outchannels);

        [DllImport("OvenMit")]
        public static extern double OvenMit_GetGlobalBeat();

        [DllImport("OvenMit")]
        public static extern void OvenMit_SetGlobalSamplesPerBeat(double samplesPerBeat);
        [DllImport("OvenMit")]
        public static extern void OvenMit_ResetPlugin();
    }
}