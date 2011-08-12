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
