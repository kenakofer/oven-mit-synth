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
        public static extern void OvenMit_ScheduleNote(int instance, int midiNote, int velocity, long start, long end);

    }
}