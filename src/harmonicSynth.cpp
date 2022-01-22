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

#include "Synth.hpp"
#include "Param.hpp"


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
class MidiFeeder
{
private:
    std::array<const float*, P_NUM_CONTROLS> control_ptr;
    const LV2_Atom_Sequence* midi_in_ptr;
    float* audio_out_ptr;
    Urids urids;
    double rate;
    LV2_URID_Map* map;
    Synth synth;


public:
    MidiFeeder(const double sample_rate, const LV2_Feature *const *features);
    inline void connectPort(const uint32_t port, void* data_location);
    inline bool isEveryControlConnected();
    inline void activate();
    inline void run(const uint32_t sample_count);

private:
    void play (const uint32_t start, const uint32_t end);
};

MidiFeeder::MidiFeeder (const double sample_rate, const LV2_Feature *const *features) :
    midi_in_ptr (nullptr),
    audio_out_ptr (nullptr),
    rate (sample_rate),
    map (nullptr),
    synth (rate)
{
    control_ptr.fill(nullptr);
    const char* missing = lv2_features_query(
        features,
        LV2_URID__map,
        &map,
        true,
        NULL
    );

    if (missing) throw std::invalid_argument ("Feature map not provided by the host. Can't instantiate MidiFeeder for OvenMit synth");

    urids.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
}

void MidiFeeder::connectPort(const uint32_t port, void* data_location)
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
        if (port < PORT_CONTROL + P_NUM_CONTROLS)
        {
            control_ptr[port - PORT_CONTROL] = static_cast<const float*>(data_location);
        }
        break;
    }
}

void MidiFeeder::activate()
{
}

inline bool MidiFeeder::isEveryControlConnected() {

    for (int i=0; i<P_NUM_CONTROLS; ++i) {
        if (!control_ptr[i]) {
            return false;
        }

    }
    return true;
}

inline void MidiFeeder::run(const uint32_t sample_count)
{
    if (!(audio_out_ptr && midi_in_ptr)) {
        throw std::invalid_argument ("Not all ports connected");
    }
    if (!isEveryControlConnected()) {
        throw std::invalid_argument ("Not all controls connected");
    }

    for (int i=0; i<P_NUM_CONTROLS; ++i) {
        synth.setControl(i, *control_ptr[i]);
    }

    /* analyze incoming midi data */
    uint32_t last_frame = 0;
    LV2_ATOM_SEQUENCE_FOREACH (midi_in_ptr, ev)
    {
        /* play frames until event */
        const uint32_t frame = ev->time.frames;
        synth.outputSamples(audio_out_ptr, last_frame, frame);
        last_frame = frame;

        if (ev->body.type == urids.midi_MidiEvent) {
            const uint8_t* const msg = reinterpret_cast<const uint8_t*>(ev + 1);
            const uint8_t typ = lv2_midi_message_type (msg);

            switch (typ) {
                case LV2_MIDI_MSG_NOTE_ON:
                    synth.startNote(
                        msg[1], /* note */
                        msg[2] /* velocity */
                    );
                    break;
                case LV2_MIDI_MSG_NOTE_OFF:
                    synth.releaseNote(msg[1], msg[2]);
                    break;
                case LV2_MIDI_MSG_CONTROLLER:
                    if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {
                        synth.stopAllNotes();
                    }
                    else if (msg[1] == LV2_MIDI_CTL_ALL_SOUNDS_OFF) {
                        synth.stopAllSounds();
                    }
                    break;
            }
        }
    }

    /* play remaining frames */
    synth.outputSamples(audio_out_ptr, last_frame, sample_count);
}




static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    MidiFeeder* m = nullptr;
    try {
        m = new MidiFeeder(sample_rate, features);
    } catch (const std::invalid_argument& ia) {
        std::cerr << ia.what() << std::endl;
        return nullptr;
    } catch (const std::bad_alloc& ba) {
        std::cerr << "Failed to allocate memory. Can't instantiate MidiFeeder" << std::endl;
        return nullptr;
    }
    return m;
}


static void connect_port (LV2_Handle instance, uint32_t port, void *data_location)
{
    MidiFeeder* m = static_cast<MidiFeeder*>(instance);
    if (m) m->connectPort(port, data_location);
}
static void activate (LV2_Handle instance)
{
    MidiFeeder* m = static_cast<MidiFeeder*>(instance);
    if (m) m->activate();
}
static void run (LV2_Handle instance, uint32_t sample_count)
{
    MidiFeeder* m = static_cast<MidiFeeder*>(instance);
    if (m) m->run(sample_count);
}
static void deactivate (LV2_Handle instance)
{
    /* also not needed */
}
static void cleanup (LV2_Handle instance)
{
    MidiFeeder* m = static_cast<MidiFeeder*>(instance);
    if (m) delete m;
}
static const void* extension_data (const char *uri)
{
    return NULL;
}

/* descriptor */
static LV2_Descriptor const descriptor =
{
    "https://github.com/kenakofer/oven-mit-synth",
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
