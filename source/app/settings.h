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
class QKeySequence;
class Tuning;

/// All predefined settings and their default values.
namespace Settings
{
    extern const Setting<std::string> PreviousDirectory;
    extern const Setting<QByteArray> WindowState;
    extern const Setting<std::vector<std::string>> RecentFiles;
    extern const Setting<bool> OpenFilesInNewWindow;

    extern const Setting<int> MidiApi;
    extern const Setting<int> MidiPort;

    extern const Setting<int> MidiVibratoLevel;
    extern const Setting<int> MidiWideVibratoLevel;

    extern const Setting<bool> MetronomeEnabled;
    extern const Setting<int> MetronomePreset;
    extern const Setting<int> MetronomeStrongAccent;
    extern const Setting<int> MetronomeWeakAccent;

    extern const Setting<bool> CountInEnabled;
    extern const Setting<int> CountInPreset;
    extern const Setting<int> CountInVolume;

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

template <>
struct SettingValueConverter<QKeySequence>
{
    static QKeySequence from(const SettingsTree::SettingValue &v);
    static SettingsTree::SettingValue to(const QKeySequence &seq);
};

#endif
