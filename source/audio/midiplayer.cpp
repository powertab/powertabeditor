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

#ifdef _WIN32
#include <boost/scope_exit.hpp>
#include <objbase.h>
#endif

static const int METRONOME_CHANNEL = 9;

static bool isMetronomeEnabled()
{
    QSettings settings;
    return settings.value(Settings::MIDI_METRONOME_ENABLED,
                          Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool();
}

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
    // Workaround to fix errors with the Microsoft GS Wavetable Synth on
    // Windows 10 - see http://stackoverflow.com/a/32553208/586978
#ifdef _WIN32
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    BOOST_SCOPE_EXIT(this_) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif

    setIsPlaying(true);

    MidiFile file;
    file.load(myScore, /* enable_metronome */ true,
              /* record_position_changes */ true);
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
    SystemLocation current_location = myStartLocation;

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
                if (settings.value(Settings::MIDI_METRONOME_ENABLE_COUNTIN,
                                   Settings::MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT).toBool())
                {
                    performCountIn(device, event->getLocation(), beat_duration);
                }

                started = true;
            }
        }

        // Skip metronome events if necessary.
        if (event->isNoteOnOff() && event->getChannel() == METRONOME_CHANNEL &&
            !isMetronomeEnabled())
        {
            continue;
        }

        const int delta = event->getTicks();
        assert(delta >= 0);

        const int duration_us = boost::rational_cast<int>(
            boost::rational<int>(delta, ticks_per_beat) * beat_duration);

        usleep(duration_us * (100.0 / myPlaybackSpeed));

        device.sendMessage(event->getData());

        // Notify listeners of the current playback position.
        if (event->getLocation() != current_location)
        {
            const SystemLocation &new_location = event->getLocation();

            // Don't move backwards unless a repeat occurred.
            if (new_location < current_location && !event->isPositionChange())
                    continue;

            if (new_location.getSystem() != current_location.getSystem())
                emit playbackSystemChanged(new_location.getSystem());

            emit playbackPositionChanged(new_location.getPosition());

            current_location = new_location;
        }
    }
}

void MidiPlayer::performCountIn(MidiOutputDevice &device,
                                const SystemLocation &location,
                                int beat_duration)
{
    // Figure out the time signature where playback is starting.
    const System &system = myScore.getSystems()[location.getSystem()];
    const Barline *barline = system.getPreviousBarline(location.getPosition());
    if (!barline)
        barline = &system.getBarlines().front();

    const TimeSignature &time_sig = barline->getTimeSignature();

    const int tick_duration = boost::rational_cast<int>(
        boost::rational<int>(4, time_sig.getBeatValue()) *
        boost::rational<int>(time_sig.getBeatsPerMeasure(),
                             time_sig.getNumPulses()) * beat_duration);

    QSettings settings;
    const uint8_t velocity =
        settings.value(Settings::MIDI_METRONOME_COUNTIN_VOLUME,
                       Settings::MIDI_METRONOME_COUNTIN_VOLUME_DEFAULT).toUInt();
    const uint8_t preset =
        Midi::MIDI_PERCUSSION_PRESET_OFFSET +
        settings.value(Settings::MIDI_METRONOME_COUNTIN_PRESET,
                       Settings::MIDI_METRONOME_COUNTIN_PRESET_DEFAULT).toUInt();

    // Play the count-in.
    device.setChannelMaxVolume(METRONOME_CHANNEL,
                               Midi::MAX_MIDI_CHANNEL_VOLUME);

    for (int i = 0; i < time_sig.getNumPulses(); ++i)
    {
        if (!isPlaying())
            break;

        device.playNote(METRONOME_CHANNEL, preset, velocity);
        usleep(tick_duration * (100.0 / myPlaybackSpeed));
        device.stopNote(METRONOME_CHANNEL, preset);
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
