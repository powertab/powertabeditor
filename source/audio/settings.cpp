/*
  * Copyright (C) 2015 Cameron White
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

#include <score/generalmidi.h>

namespace Settings
{
const Setting<int> MidiApi("midi/api", 0);

const Setting<int> MidiPort("midi/port", 0);

const Setting<int> MidiVibratoLevel("midi/vibrato_level", 85);

const Setting<int> MidiWideVibratoLevel("midi/wide_vibrato_level", 127);

const Setting<bool> PlayNotesWhileEditing("midi/play_notes_while_editing",
                                          false);

const Setting<bool> MetronomeEnabled("midi/metronome_enabled", true);

const Setting<int> MetronomePreset("midi/metronome_preset",
                                   Midi::MIDI_PERCUSSION_PRESET_HI_WOOD_BLOCK);

const Setting<int> MetronomeStrongAccent("midi/metronome_strong_accent", 127);

const Setting<int> MetronomeWeakAccent("midi/metronome_weak_accent", 80);

const Setting<bool> CountInEnabled("midi/count_in_enabled", true);

const Setting<int> CountInPreset("midi/count_in_preset",
                                 Midi::MIDI_PERCUSSION_PRESET_RIDE_CYMBAL2);

const Setting<int> CountInVolume("midi/count_in_volume", 127);
}
