using UnityEngine;
using System;
using System.Runtime.InteropServices;
using System.Xml;


namespace OvenMit
{
    public static class PatchLoader
    {
        const string LMMS_XML_PARAMETER_TAG_NAME = "models";

        public static readonly Param.SynthParameters[] LMMS_ATTRIBUTE_PARAMETERS= {
            Param.SynthParameters.P_ENV_MODE_1,
            Param.SynthParameters.P_SUSTAIN   ,
            Param.SynthParameters.P_WAVEFORM  ,
            Param.SynthParameters.P_LEVEL     ,
            Param.SynthParameters.P_PITCH     ,
            Param.SynthParameters.P_FILTER    ,
            Param.SynthParameters.P_CUTOFF    ,
            Param.SynthParameters.P_RES_HEIGHT,
            Param.SynthParameters.P_RES_WIDTH ,
            Param.SynthParameters.P_ENV_MODE_2,
            Param.SynthParameters.P_ENV_AMT_2 ,
            Param.SynthParameters.P_SUSTAIN_2 ,
            Param.SynthParameters.P_WAVEFORM_2_MODE,
            Param.SynthParameters.P_WAVEFORM_2,
            Param.SynthParameters.P_LEVEL_2,
            Param.SynthParameters.P_PITCH_2,
            Param.SynthParameters.P_VOICE_MODE,
            Param.SynthParameters.P_ATTACK    ,
            Param.SynthParameters.P_DECAY     ,
            Param.SynthParameters.P_RELEASE   ,
            Param.SynthParameters.P_ATTACK_2  ,
            Param.SynthParameters.P_DECAY_2   ,
            Param.SynthParameters.P_RELEASE_2 ,
            Param.SynthParameters.P_PORTAMENTO
        };

        // When the scale_type is log, LMMS stores those parameters as child elements.
        // Currently none, but we'll keep the logic around for now.
        public static readonly Param.SynthParameters[] LMMS_CHILD_PARAMETERS= {
        };

        public static void LoadPatchFromFile(int instance_index, string path)
        {
            string fullPath = Application.streamingAssetsPath + "/" + path;
            Debug.Log($"Application.streamingAssetsPath is {Application.streamingAssetsPath}");
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(fullPath);
            foreach (XmlElement elem in xmlDoc.GetElementsByTagName("models")) {
                foreach (Param.SynthParameters param in LMMS_ATTRIBUTE_PARAMETERS) {
                    string name = Param.PARAMETER_SYMBOLS[(int)param];
                    float value = float.Parse(elem.GetAttribute(name));
                    Debug.Log("Attribute: "+ (int)param + " " + name + " " + value.ToString());
                    Native.OvenMit_SetSynthParameter(instance_index, (int)param, value);
                }
                foreach (Param.SynthParameters param in LMMS_CHILD_PARAMETERS) {
                    string name = Param.PARAMETER_SYMBOLS[(int)param];
                    float value = float.Parse(elem[name].GetAttribute("value"));
                    Debug.Log("Attribute: "+ (int)param + " " + name + " " + value.ToString());
                    Native.OvenMit_SetSynthParameter(instance_index, (int)param, value);
                }
            }
        }
    }
}