/*
  * Copyright (C) 2012 Cameron White
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

#ifndef AUDIO_RESTEVENT_H
#define AUDIO_RESTEVENT_H

#include "midievent.h"

/// This event doesn't actually generate any audio, but is useful for
/// advancing the caret location when a rest is active.
class RestEvent : public MidiEvent
{
public:
    RestEvent(int channel, double startTime, double duration,
              int position, int system);

    virtual void performEvent(MidiOutputDevice &device) const;
};

#endif
