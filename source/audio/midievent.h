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
  
#ifndef AUDIO_MIDIEVENT_H
#define AUDIO_MIDIEVENT_H

class MidiOutputDevice;

class MidiEvent
{
public:
    static const int NUM_CHANNELS;

    MidiEvent(int channel, double startTime, double duration,
              int position, int system);
    virtual ~MidiEvent() {}

    /// Orders by timestamp, then by system index, then by position index.
    bool operator<(const MidiEvent &event) const;

    /// Performs the event by sending commands to the MIDI output device.
    virtual void performEvent(MidiOutputDevice &sequencer) const = 0;

    int getPosition() const;
    int getSystem() const;
    double getDuration() const;
    double getStartTime() const;

protected:
    /// The MIDI channel that this event will be sent to.
    int myChannel;
    /// The timestamp of the start of the event.
    double myStartTime;
    /// The length of the event (e.g. the duration of a note).
    double myDuration;
    /// The position in the staff that the event occurs at.
    int myPosition;
    /// The system that the event occurs in.
    int mySystem;
};

#endif
