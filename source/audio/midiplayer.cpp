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

#include <app/settingsmanager.h>
#include <audio/midioutputdevice.h>
#include <audio/settings.h>
#include <boost/rational.hpp>
#include <cassert>
#include <midi/midifile.h>
#include <score/generalmidi.h>
#include <score/score.h>

#ifdef _WIN32
#include <boost/scope_exit.hpp>
#include <objbase.h>
#endif

static const int METRONOME_CHANNEL = 9;

MidiPlayer::MidiPlayer(SettingsManager &settings_manager, const Score &score,
                       int start_system, int start_pos, int speed)
    : mySettingsManager(settings_manager),
      myScore(score),
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

    boost::signals2::scoped_connection connection(
        mySettingsManager.subscribeToChanges([&]() {
            auto settings = mySettingsManager.getReadHandle();
            myMetronomeEnabled = settings->get(Settings::MetronomeEnabled);
        }));

    setIsPlaying(true);

    MidiFile::LoadOptions options;
    options.myEnableMetronome = true;
    options.myRecordPositionChanges = true;

    // Load MIDI settings.
    int api;
    int port;
    {
        auto settings = mySettingsManager.getReadHandle();
        myMetronomeEnabled = settings->get(Settings::MetronomeEnabled);

        api = settings->get(Settings::MidiApi);
        port = settings->get(Settings::MidiPort);

        options.myMetronomePreset = settings->get(Settings::MetronomePreset) +
                                    Midi::MIDI_PERCUSSION_PRESET_OFFSET;
        options.myStrongAccentVel =
            settings->get(Settings::MetronomeStrongAccent);
        options.myWeakAccentVel = settings->get(Settings::MetronomeWeakAccent);
        options.myVibratoStrength = settings->get(Settings::MidiVibratoLevel);
        options.myWideVibratoStrength =
            settings->get(Settings::MidiWideVibratoLevel);
    }

    MidiFile file;
    file.load(myScore, options);

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
                performCountIn(device, event->getLocation(), beat_duration);

                started = true;
            }
        }

        const int delta = event->getTicks();
        assert(delta >= 0);

        const int duration_us = boost::rational_cast<int>(
            boost::rational<int>(delta, ticks_per_beat) * beat_duration);

        usleep(duration_us * (100.0 / myPlaybackSpeed));

        // Don't play metronome events if the metronome is disabled.
        if (event->isNoteOnOff() && event->getChannel() == METRONOME_CHANNEL &&
            !myMetronomeEnabled)
        {
            continue;
        }

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
    // Load preferences.
    uint8_t velocity;
    uint8_t preset;
    {
        auto settings = mySettingsManager.getReadHandle();

        if (!settings->get(Settings::CountInEnabled))
            return;

        velocity = settings->get(Settings::CountInVolume);
        preset = settings->get(Settings::CountInPreset) +
                 Midi::MIDI_PERCUSSION_PRESET_OFFSET;
    }

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
