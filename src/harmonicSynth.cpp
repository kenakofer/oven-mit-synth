/* include libs */
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <new>
#include <array>
#include <random>
#include <ctime>
#include <utility>

#include "lv2.h"
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <lv2/midi/midi.h>
#include <lv2/core/lv2_util.h>
#include <lv2/atom/util.h>

#include "Controls.hpp"
#include "LinearFader.hpp"
#include "Key.hpp"
#include "KeyMap.hpp"
#include "LowPassBasic.hpp"
#include "Filter2.hpp"


enum PortGroups
{
    PORT_MIDI_IN     = 0,
    PORT_AUDIO_OUT   = 1,
    PORT_CONTROL     = 2,
    PORT_NR          = 3
};

struct Urids
{
    LV2_URID midi_MidiEvent;
};

/* class definiton */
class HarmonicSynth
{
private:
    const LV2_Atom_Sequence* midi_in_ptr;
    float* audio_out_ptr;
    Urids urids;
    double rate;
    double position;
    float actual_freq;
    float actual_level;
    LV2_URID_Map* map;
    KeyMap keys;
    LowPassBasic low_pass;
    Filter filter;
    Controls controls;


public:
    HarmonicSynth(const double sample_rate, const LV2_Feature *const *features);
    void connectPort(const uint32_t port, void* data_location);
    void activate();
    void run(const uint32_t sample_count);

private:
    void play (const uint32_t start, const uint32_t end);
};

HarmonicSynth::HarmonicSynth (const double sample_rate, const LV2_Feature *const *features) :
    midi_in_ptr (nullptr),
    audio_out_ptr (nullptr),
    rate (sample_rate),
    position (0.0),
    actual_freq (0.0),
    map (nullptr),
    keys (),
    low_pass (),
    filter (),
    controls (sample_rate)
{
    controls = Controls(sample_rate);
    const char* missing = lv2_features_query(
        features,
        LV2_URID__map,
        &map,
        true,
        NULL
    );

    if (missing) throw std::invalid_argument ("Feature map not provided by the host. Can't instantiate HarmonicSynth");

    urids.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
}

void HarmonicSynth::connectPort(const uint32_t port, void* data_location)
{
    switch (port)
    {
    case PORT_MIDI_IN:
        midi_in_ptr = static_cast<const LV2_Atom_Sequence*>(data_location);
        break;
    case PORT_AUDIO_OUT:
        audio_out_ptr = static_cast<float*>(data_location);
        break;
    default:
        if (port < PORT_CONTROL + CONTROL_NR)
        {
            controls.connectControlPort(port - PORT_CONTROL, data_location);
        }
        break;
    }
}

void HarmonicSynth::activate()
{
    position = 0.0;
    actual_freq = 440.0;
    actual_level = 0.1;
    filter.setWaveform(WAVEFORM_SINE);
}

void HarmonicSynth::play (const uint32_t start, const uint32_t end)
{
    for (uint32_t i = start; i < end; ++i)
    {
        audio_out_ptr[i] = 0.0f;
        keys.startLoop();
        Key* k;
        while (k = keys.getNext()) {
            if (k->isOn()) {
                audio_out_ptr[i] += k->get();
                k->proceed();
            } else {
                keys.erasePrevious();
            }
        }
    }
}

void HarmonicSynth::run(const uint32_t sample_count)
{
    if (!(audio_out_ptr && midi_in_ptr)) {
        throw std::invalid_argument ("Not all ports connected");
    }
    if (!controls.isEveryControlConnected()) {
        throw std::invalid_argument ("Not all controls connected");
    }

    if (controls.updateValues()) {
        /* filter refreshing and control moving */
        filter.setWaveform(
            static_cast<Waveform>(controls.get(CONTROL_WAVEFORM))
        );
    }

    /* analyze incoming midi data */
    uint32_t last_frame = 0;
    LV2_ATOM_SEQUENCE_FOREACH (midi_in_ptr, ev)
    {
        /* play frames until event */
        const uint32_t frame = ev->time.frames;
        play (last_frame, frame);
        last_frame = frame;

        if (ev->body.type == urids.midi_MidiEvent) {
            const uint8_t* const msg = reinterpret_cast<const uint8_t*>(ev + 1);
            const uint8_t typ = lv2_midi_message_type (msg);

            switch (typ) {
                case LV2_MIDI_MSG_NOTE_ON:
                    keys.getKey(msg[1] & 0x7f)->press(
                        msg[1], /* note */
                        msg[2], /* velocity */
                        &controls,
                        &filter
                    );
                    break;
                case LV2_MIDI_MSG_NOTE_OFF:
                    keys.getKey(msg[1] & 0x7f)->release (msg[1], msg[2]);
                    break;
                case LV2_MIDI_MSG_CONTROLLER:
                    if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {
                        keys.startLoop();
                        Key* k;
                        while (k = keys.getNext()) k->release();
                    }
                    else if (msg[1] == LV2_MIDI_CTL_ALL_SOUNDS_OFF) {
                        keys.startLoop();
                        Key* k;
                        while (k = keys.getNext()) k->mute();
                    }
                    break;
            }
        }
    }

    /* play remaining frames */
    play(last_frame, sample_count);
}




static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    HarmonicSynth* m = nullptr;
    try {
        m = new HarmonicSynth(sample_rate, features);
    } catch (const std::invalid_argument& ia) {
        std::cerr << ia.what() << std::endl;
        return nullptr;
    } catch (const std::bad_alloc& ba) {
        std::cerr << "Failed to allocate memory. Can't instantiate HarmonicSynth" << std::endl;
        return nullptr;
    }
    return m;
}


static void connect_port (LV2_Handle instance, uint32_t port, void *data_location)
{
    HarmonicSynth* m = static_cast<HarmonicSynth*>(instance);
    if (m) m->connectPort(port, data_location);
}
static void activate (LV2_Handle instance)
{
    HarmonicSynth* m = static_cast<HarmonicSynth*>(instance);
    if (m) m->activate();
}
static void run (LV2_Handle instance, uint32_t sample_count)
{
    HarmonicSynth* m = static_cast<HarmonicSynth*>(instance);
    if (m) m->run(sample_count);
}
static void deactivate (LV2_Handle instance)
{
    /* also not needed */
}
static void cleanup (LV2_Handle instance)
{
    HarmonicSynth* m = static_cast<HarmonicSynth*>(instance);
    if (m) delete m;
}
static const void* extension_data (const char *uri)
{
    return NULL;
}

/* descriptor */
static LV2_Descriptor const descriptor =
{
    "https://github.com/kenakofer/lv2-practice/harmonicSynth",
    instantiate,
    connect_port,
    activate, /* or NULL */
    run,
    deactivate, /* or NULL */
    cleanup,
    extension_data /* or NULL */
};

/* interface */

LV2_SYMBOL_EXPORT const LV2_Descriptor * lv2_descriptor (uint32_t index)
{
    if (index==0) return &descriptor;
    else return NULL;

}
