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
  
#include "midiplayer.h"

#include <app/settings.h>
#include <audio/midioutputdevice.h>
#include <boost/rational.hpp>
#include <cassert>
#include <midi/midifile.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <QSettings>

static const int METRONOME_CHANNEL = 9;

MidiPlayer::MidiPlayer(const Score &score, int start_system, int start_pos,
                       int speed)
    : myScore(score),
      myStartLocation(start_system, start_pos),
      myIsPlaying(false),
      myPlaybackSpeed(speed)
{
}

MidiPlayer::~MidiPlayer()
{
    setIsPlaying(false);
    wait();
}

void MidiPlayer::run()
{
    setIsPlaying(true);

    MidiFile file;
    file.load(myScore, /* enable_metronome */ true);
    const int ticks_per_beat = file.getTicksPerBeat();

    // Merge the MIDI evvents for each track.
    MidiEventList events;
    for (MidiEventList &track : file.getTracks())
    {
        track.convertToAbsoluteTicks();
        events.concat(track);
    }

    // TODO - since each track is already sorted, an n-way merge should be faster.
    std::stable_sort(events.begin(), events.end());
    events.convertToDeltaTicks();

    QSettings settings;
    const int api = settings.value(Settings::MIDI_PREFERRED_API,
                                   Settings::MIDI_PREFERRED_API_DEFAULT).toInt();
    const int port = settings.value(Settings::MIDI_PREFERRED_PORT,
                                    Settings::MIDI_PREFERRED_PORT_DEFAULT).toInt();

    // Initialize RtMidi and set the port.
    MidiOutputDevice device;
    if (!device.initialize(api, port))
    {
        emit error(tr("Error initializing MIDI output device."));
        return;
    }

    bool started = false;
    int beat_duration = Midi::BEAT_DURATION_120_BPM;
    for (auto event = events.begin(); event != events.end(); ++event)
    {
        if (!isPlaying())
            break;

        if (event->isTempoChange())
            beat_duration = event->getTempo();

        // Skip events before the start location, except for events such as
        // instrument changes. Tempo changes are tracked above.
        if (!started)
        {
            if (event->getLocation() < myStartLocation)
            {
                if (event->isProgramChange())
                    device.sendMessage(event->getData());

                continue;
            }
            else
            {
                // TODO - perform count in.
#if 0
                if (settings.value(Settings::MIDI_METRONOME_ENABLE_COUNTIN,
                                   Settings::MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT).toBool())
                {
                    performCountIn(device, SystemLocation(myStartSystem, myStartPosition));
                }
#endif
                started = true;
            }
        }

        const int delta = event->getTicks();
        assert(delta >= 0);

        const int duration_us = boost::rational_cast<int>(
            boost::rational<int>(delta, ticks_per_beat) * beat_duration);

        // TODO - include speed shift factor.
        usleep(duration_us);

        device.sendMessage(event->getData());
    }
}

void MidiPlayer::changePlaybackSpeed(int new_speed)
{
    myPlaybackSpeed = new_speed;
}

void MidiPlayer::setIsPlaying(bool set)
{
    myIsPlaying = set;
}

bool MidiPlayer::isPlaying() const
{
    return myIsPlaying;
}
