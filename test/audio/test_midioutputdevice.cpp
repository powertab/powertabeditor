/*
 * Copyright (C) 2020 Cameron White
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

#include <doctest/doctest.h>

#include <audio/midioutputdevice.h>
#include <RtMidi.h>

TEST_CASE("Audio/MidiOutputDevice/Basic")
{
    MidiOutputDevice device;

    // Ensure we aren't using the dummy API that is used when RtMidi was
    // compiled without support for the appropriate platform-specific API. If
    // this is the case, MIDI output won't do anything.
    std::vector<RtMidi::Api> apis;
    RtMidi::getCompiledApi(apis);
    for (RtMidi::Api api : apis)
        REQUIRE(api != RtMidi::RTMIDI_DUMMY);

    // Headless environments (e.g. CI runners) may have no MIDI backend
    // available at runtime (e.g. no ALSA sequencer), so there is nothing
    // further to check.
    if (device.getApiCount() == 0)
    {
        MESSAGE("No MIDI APIs available at runtime, skipping initialization check");
        return;
    }
}
