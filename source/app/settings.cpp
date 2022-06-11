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
#include <sstream>

static const std::string theLightName("light");
static const std::string theDarkName("dark");
static const std::string theDefaultName("default");

namespace Settings
{
const Setting<std::string> PreviousDirectory(
    "app/previous_directory", Paths::getHomeDir().generic_string());

const Setting<int> LastZoomLevel("app/last_zoom_level", 100);

const Setting<QByteArray> WindowState("app/window_state", QByteArray());

const Setting<std::vector<std::string>> RecentFiles("app/recent_files", {});

const Setting<bool> OpenFilesInNewWindow("app/open_files_in_new_window",
                                         false);

const Setting<int> SystemSpacing("app/system_spacing", 50);

const Setting<ScoreTheme> Theme("app/score_theme", ScoreTheme::SystemDefault);

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

    std::stringstream input(std::get<std::string>(v));
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
        QByteArray::fromStdString(std::get<std::string>(v)));
}

SettingsTree::SettingValue SettingValueConverter<QByteArray>::to(
    const QByteArray &array)
{
    return array.toBase64().toStdString();
}

QKeySequence SettingValueConverter<QKeySequence>::from(
    const SettingsTree::SettingValue &v)
{
    return QKeySequence::fromString(
        QString::fromStdString(std::get<std::string>(v)));
}

SettingsTree::SettingValue SettingValueConverter<QKeySequence>::to(
    const QKeySequence &seq)
{
    return seq.toString().toStdString();
}

ScoreTheme
SettingValueConverter<ScoreTheme>::from(const SettingsTree::SettingValue &v)
{
    const std::string name = std::get<std::string>(v);

    if (name == theLightName)
        return ScoreTheme::Light;
    else if (name == theDarkName)
        return ScoreTheme::Dark;
    else
        return ScoreTheme::SystemDefault;
}

SettingsTree::SettingValue
SettingValueConverter<ScoreTheme>::to(const ScoreTheme &theme)
{
    switch (theme)
    {
        case ScoreTheme::Light:
            return theLightName;
        case ScoreTheme::Dark:
            return theDarkName;
        default:
            return theDefaultName;
    }
}
