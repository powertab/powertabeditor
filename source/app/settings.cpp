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

#include "settings.h"

#include <app/paths.h>
#include <score/generalmidi.h>
#include <score/tuning.h>
#include <sstream>

#include <QByteArray>

namespace Settings
{
const Setting<std::string> PreviousDirectory(
    "app/previous_directory", Paths::getHomeDir().generic_string());

const Setting<QByteArray> WindowState("app/window_state", QByteArray());

const Setting<std::vector<std::string>> RecentFiles("app/recent_files", {});

const char *MIDI_PREFERRED_API = "midi/preferredApi";
const int MIDI_PREFERRED_API_DEFAULT = 0;

const char *MIDI_PREFERRED_PORT = "midi/preferredPort";
const int MIDI_PREFERRED_PORT_DEFAULT = 0;

const char *MIDI_VIBRATO_LEVEL = "midi/vibrato";
const int MIDI_VIBRATO_LEVEL_DEFAULT = 85;

const char *MIDI_WIDE_VIBRATO_LEVEL = "midi/wideVibrato";
const int MIDI_WIDE_VIBRATO_LEVEL_DEFAULT = 127;

const char *MIDI_METRONOME_ENABLED = "midi/metronomeEnabled";
const bool MIDI_METRONOME_ENABLED_DEFAULT = true;

const char *MIDI_METRONOME_PRESET = "midi/metronomePreset";
const int MIDI_METRONOME_PRESET_DEFAULT =
    Midi::MIDI_PERCUSSION_PRESET_HI_WOOD_BLOCK;

const char *MIDI_METRONOME_STRONG_ACCENT = "midi/metronomeStrongAccent";
const int MIDI_METRONOME_STRONG_ACCENT_DEFAULT = 127;

const char *MIDI_METRONOME_WEAK_ACCENT = "midi/metronomeWeakAccent";
const int MIDI_METRONOME_WEAK_ACCENT_DEFAULT = 80;

const char *MIDI_METRONOME_ENABLE_COUNTIN = "midi/metronomeEnableCountIn";
const bool MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT = true;

const char *MIDI_METRONOME_COUNTIN_PRESET = "midi/metronomeCountInPreset";
const int MIDI_METRONOME_COUNTIN_PRESET_DEFAULT =
    Midi::MIDI_PERCUSSION_PRESET_RIDE_CYMBAL2;

const char *MIDI_METRONOME_COUNTIN_VOLUME = "midi/metronomeCountInVolume";
const int MIDI_METRONOME_COUNTIN_VOLUME_DEFAULT = 127;

const Setting<bool> OpenFilesInNewWindow("app/open_files_in_new_window",
                                         false);

const Setting<std::string> DefaultInstrumentName("app/default_instrument_name",
                                                 "Untitled");

const Setting<int> DefaultInstrumentPreset(
    "app/default_instrument_preset", Midi::MIDI_PRESET_ACOUSTIC_GUITAR_STEEL);

const Setting<Tuning> DefaultTuning("app/default_tuning", Tuning());
}

Tuning SettingValueConverter<Tuning>::from(const SettingsTree::SettingValue &v)
{
    std::string name;
    int offset = 0;
    bool sharps = false;
    size_t num_notes = 0;
    std::vector<uint8_t> notes;

    std::stringstream input(boost::get<std::string>(v));
    input >> name >> offset >> sharps >> num_notes;

    for (size_t i = 0; i < num_notes; ++i)
    {
        int note = 0;
        input >> note;
        notes.push_back(note);
    }

    Tuning tuning;
    tuning.setName(name);
    tuning.setMusicNotationOffset(offset);
    tuning.setSharps(sharps);
    tuning.setNotes(notes);
    return tuning;
}

SettingsTree::SettingValue SettingValueConverter<Tuning>::to(
    const Tuning &tuning)
{
    std::ostringstream ss;
    ss << tuning.getName() << " "
       << static_cast<int>(tuning.getMusicNotationOffset()) << " "
       << tuning.usesSharps() << " ";
    ss << tuning.getNotes().size();
    for (int n : tuning.getNotes())
        ss << " " << n;

    return ss.str();
}

QByteArray SettingValueConverter<QByteArray>::from(
    const SettingsTree::SettingValue &v)
{
    return QByteArray::fromBase64(
        QByteArray::fromStdString(boost::get<std::string>(v)));
}

SettingsTree::SettingValue SettingValueConverter<QByteArray>::to(
    const QByteArray &array)
{
    return array.toBase64().toStdString();
}
