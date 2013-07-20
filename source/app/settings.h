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
  
#include <QMetaType>
#include <score/tuning.h>

#ifndef SETTINGS_H
#define SETTINGS_H

/// Contains constants for keys used with the QSettings class,
/// as well as default values for those settings where appropriate
namespace Settings
{
    extern const char* APP_PREVIOUS_DIRECTORY;
    extern const char* APP_RECENT_FILES;

    extern const char* MIDI_PREFERRED_API;
    extern const int MIDI_PREFERRED_API_DEFAULT;

    extern const char* MIDI_PREFERRED_PORT;
    extern const int MIDI_PREFERRED_PORT_DEFAULT;

    extern const char* MIDI_METRONOME_ENABLED;
    extern const bool MIDI_METRONOME_ENABLED_DEFAULT;

    extern const char* MIDI_METRONOME_PRESET;
    extern const int MIDI_METRONOME_PRESET_DEFAULT;

    extern const char* MIDI_VIBRATO_LEVEL;
    extern const int MIDI_VIBRATO_LEVEL_DEFAULT;

    extern const char* MIDI_WIDE_VIBRATO_LEVEL;
    extern const int MIDI_WIDE_VIBRATO_LEVEL_DEFAULT;

    extern const char* APPEARANCE_USE_SKIN;
    extern const char* APPEARANCE_SKIN_NAME;

    extern const char* DEFAULT_INSTRUMENT_NAME;
    extern const char* DEFAULT_INSTRUMENT_NAME_DEFAULT;

    extern const char* DEFAULT_INSTRUMENT_PRESET;
    extern const int DEFAULT_INSTRUMENT_PRESET_DEFAULT;

    extern const char* DEFAULT_INSTRUMENT_TUNING;
    extern const Tuning DEFAULT_INSTRUMENT_TUNING_DEFAULT;
}

Q_DECLARE_METATYPE(Tuning);

QDataStream& operator<<(QDataStream& out, const Tuning &tuning);
QDataStream& operator>>(QDataStream& in, Tuning &tuning);

#endif // SETTINGS_H
