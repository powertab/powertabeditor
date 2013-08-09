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

#include <catch.hpp>

#include <audio/midievent.h>

class MockMidiEvent : public MidiEvent
{
public:
    MockMidiEvent(int channel, double startTime, double duration,
                  int position, int system)
        : MidiEvent(channel, startTime, duration, position, system)
    {
    }

    void performEvent(MidiOutputDevice &) const {}
};

TEST_CASE("Audio/Ordering/StartTime", "Events should be ordered by timestamp.")
{
    MockMidiEvent event1(0, 100, 100, 4, 0);
    MockMidiEvent event2(0, 50, 100, 4, 0);

    REQUIRE(event2 < event1);
}

TEST_CASE("Audio/Ordering/CloseTimeStamps",
          "Very close timestamps are considered equal.")
{
    MockMidiEvent event1(0, 100, 100, 4, 0);
    MockMidiEvent event2(0, 100.15, 100, 4, 0);

    REQUIRE(event1 < event2);

    // timestamps that are extremely close are considered equal
    MockMidiEvent event3(0, 100.000001, 100, 4, 0);
    const bool equal = !(event1 < event3) && !(event3 < event1);
    REQUIRE(equal);
}

TEST_CASE("Audio/Ordering/SystemIndex",
          "Order by system index if timestamps are equal.")
{
    MockMidiEvent event1(0, 100, 100, 4, 0);
    MockMidiEvent event2(0, 100, 100, 4, 1);

    REQUIRE(event1 < event2);
}

TEST_CASE("Audio/Ordering/PositionIndex",
          "Order by position index if systems are equal.")
{
    MockMidiEvent event1(0, 100, 100, 5, 1);
    MockMidiEvent event2(0, 100, 100, 4, 1);

    REQUIRE(event2 < event1);
}
