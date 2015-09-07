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
  
#ifndef MIDI_MIDIEVENTLIST_H
#define MIDI_MIDIEVENTLIST_H

#include <midi/midievent.h>
#include <vector>

class MidiEventList
{
public:
    MidiEventList(bool absolute_ticks = true);

    /// Convert the MIDI events from absolute to delta ticks.
    void convertToDeltaTicks();
    /// Convert the MIDI events from delta to absolute ticks.
    void convertToAbsoluteTicks();

    void append(const MidiEvent &event) { myEvents.push_back(event); }
    void append(MidiEvent &&event)
    {
        myEvents.push_back(std::forward<MidiEvent>(event));
    }

    void concat(const MidiEventList &other);

    typedef std::vector<MidiEvent>::iterator iterator;
    typedef std::vector<MidiEvent>::const_iterator const_iterator;

    iterator begin() { return myEvents.begin(); }
    iterator end() { return myEvents.end(); }
    const_iterator begin() const { return myEvents.begin(); }
    const_iterator end() const { return myEvents.end(); }

private:
    std::vector<MidiEvent> myEvents;
    bool myAbsoluteTicks;
};

#endif
