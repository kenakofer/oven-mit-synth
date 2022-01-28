using System;
using System.Runtime.InteropServices;
using System.Xml;


namespace OvenMit
{

    public class OvenMitPreset {
        public string name;
        public float[] parameters = new float[Param.P_NUM];
    }

    public static class PresetLoader
    {
        const string LMMS_XML_PARAMETER_TAG_NAME = "models";
        const string LMMS_XML_CHILD_VALUE_ATTRIBUTE_NAME = "value";

        public static OvenMitPreset LoadPresetFromFile(string path)
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(path);

            OvenMitPreset preset = new OvenMitPreset();

            // There should only be one <models> element in the xml
            foreach (XmlElement elem in xmlDoc.GetElementsByTagName(LMMS_XML_PARAMETER_TAG_NAME)) {

                // LMMS stores the synth variables as attributes by default. Try
                // to load all the synth parameters that way.
                foreach (Param.SynthParameters param in Enum.GetValues(typeof(Param.SynthParameters))) {
                    string name = Param.PARAMETER_SYMBOLS[(int)param];
                    string stringvalue = elem.GetAttribute(name);
                    if (stringvalue != "") {
                        float value = float.Parse(stringvalue);
                        preset.parameters[(int)param] = value;
                    }
                }
                // When the scale_type is log, LMMS stores those parameters as
                // child elements, so we need to iterate through those as well
                // to get them all
                foreach (Param.SynthParameters param in Enum.GetValues(typeof(Param.SynthParameters))) {
                    string name = Param.PARAMETER_SYMBOLS[(int)param];
                    if (elem[name] == null) continue;
                    float value = float.Parse(elem[name].GetAttribute(LMMS_XML_CHILD_VALUE_ATTRIBUTE_NAME));
                    preset.parameters[(int)param] = value;
                }
            }

            return preset;
        }
    }
}