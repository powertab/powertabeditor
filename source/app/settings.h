/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <util/settingstree.h>

class QByteArray;
class Tuning;

/// Contains constants for keys used with the QSettings class,
/// as well as default values for those settings where appropriate
namespace Settings
{
    extern const Setting<std::string> PreviousDirectory;
    extern const Setting<QByteArray> WindowState;
    extern const Setting<std::vector<std::string>> RecentFiles;
    extern const Setting<bool> OpenFilesInNewWindow;

    extern const char *MIDI_PREFERRED_API;
    extern const int MIDI_PREFERRED_API_DEFAULT;

    extern const char *MIDI_PREFERRED_PORT;
    extern const int MIDI_PREFERRED_PORT_DEFAULT;

    extern const char *MIDI_VIBRATO_LEVEL;
    extern const int MIDI_VIBRATO_LEVEL_DEFAULT;

    extern const char *MIDI_WIDE_VIBRATO_LEVEL;
    extern const int MIDI_WIDE_VIBRATO_LEVEL_DEFAULT;

    extern const char *MIDI_METRONOME_ENABLED;
    extern const bool MIDI_METRONOME_ENABLED_DEFAULT;

    extern const char *MIDI_METRONOME_PRESET;
    extern const int MIDI_METRONOME_PRESET_DEFAULT;

    extern const char *MIDI_METRONOME_STRONG_ACCENT;
    extern const int MIDI_METRONOME_STRONG_ACCENT_DEFAULT;

    extern const char *MIDI_METRONOME_WEAK_ACCENT;
    extern const int MIDI_METRONOME_WEAK_ACCENT_DEFAULT;

    extern const char *MIDI_METRONOME_ENABLE_COUNTIN;
    extern const bool MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT;

    extern const char *MIDI_METRONOME_COUNTIN_PRESET;
    extern const int MIDI_METRONOME_COUNTIN_PRESET_DEFAULT;

    extern const char *MIDI_METRONOME_COUNTIN_VOLUME;
    extern const int MIDI_METRONOME_COUNTIN_VOLUME_DEFAULT;

    extern const Setting<std::string> DefaultInstrumentName;
    extern const Setting<int> DefaultInstrumentPreset;
    extern const Setting<Tuning> DefaultTuning;
}

template <>
struct SettingValueConverter<Tuning>
{
    static Tuning from(const SettingsTree::SettingValue &v);
    static SettingsTree::SettingValue to(const Tuning &t);
};

template <>
struct SettingValueConverter<QByteArray>
{
    static QByteArray from(const SettingsTree::SettingValue &v);
    static SettingsTree::SettingValue to(const QByteArray &array);
};

#endif
