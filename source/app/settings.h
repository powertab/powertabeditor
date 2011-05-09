#ifndef SETTINGS_H
#define SETTINGS_H

/// Contains constants for keys used with the QSettings class,
/// as well as default values for those settings where appropriate
namespace Settings
{
    extern const char* APP_PREVIOUS_DIRECTORY;

    extern const char* MIDI_PREFERRED_PORT;
    extern const int MIDI_PREFFERED_PORT_DEFAULT;

    extern const char* MIDI_METRONOME_ENABLED;
    extern const bool MIDI_METRONOME_ENABLED_DEFAULT;

    extern const char* MIDI_VIBRATO_LEVEL;
    extern const int MIDI_VIBRATO_LEVEL_DEFAULT;

    extern const char* MIDI_WIDE_VIBRATO_LEVEL;
    extern const int MIDI_WIDE_VIBRATO_LEVEL_DEFAULT;
}

#endif // SETTINGS_H
