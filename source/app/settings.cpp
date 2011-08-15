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

    const char* APPEARANCE_USE_SKIN = "appearance/useSkin";
    const char* APPEARANCE_SKIN_NAME = "appearance/skinName";
}
