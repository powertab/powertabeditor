#include "settings.h"

namespace Settings
{
    const char* APP_PREVIOUS_DIRECTORY = "app/previousDirectory";

    const char* MIDI_PREFERRED_PORT = "midi/preferredPort";
    const int MIDI_PREFFERED_PORT_DEFAULT = 0;

    const char* MIDI_METRONOME_ENABLED = "midi/metronomeEnabled";
    const bool MIDI_METRONOME_ENABLED_DEFAULT = true;

    const char* MIDI_VIBRATO_LEVEL = "midi/vibrato";
    const int MIDI_VIBRATO_LEVEL_DEFAULT = 85;

    const char* MIDI_WIDE_VIBRATO_LEVEL = "midi/wide_vibrato";
    const int MIDI_WIDE_VIBRATO_LEVEL_DEFAULT = 127;
}
