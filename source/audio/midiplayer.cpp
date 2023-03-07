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
#include <chrono>
#include <midi/midifile.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <thread>
#include <util/scopeexit.h>

#ifdef _WIN32
#include <objbase.h>
#endif

static const int METRONOME_CHANNEL = 9;

using DurationType = std::chrono::duration<int, std::micro>;

MidiPlayer::MidiPlayer(SettingsManager &settings_manager)
    : mySettingsManager(settings_manager)
{
    mySettingsListener = settings_manager.subscribeToChanges(
        [&]()
        {
            // Always immediately perform any live settings updates if e.g.
            // playback is already running.
            QMetaObject::invokeMethod(this, &MidiPlayer::updateLiveSettings,
                                      Qt::DirectConnection);

            // Add a queued event to update the device settings.
            QMetaObject::invokeMethod(this, &MidiPlayer::updateDeviceSettings);
        });
}

MidiPlayer::~MidiPlayer()
{
    // Matches the CoInitializeEx() call in init().
#ifdef _WIN32
    CoUninitialize();
#endif
}

void
MidiPlayer::init()
{
	// Workaround to fix errors with the Microsoft GS Wavetable Synth on
	// Windows 10 - see http://stackoverflow.com/a/32553208/586978
#ifdef _WIN32
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif

    updateLiveSettings();
    updateDeviceSettings();
}

void
MidiPlayer::updateDeviceSettings()
{
    // Load MIDI settings.
    int api;
    int port;
    {
        auto settings = mySettingsManager.getReadHandle();
        api = settings->get(Settings::MidiApi);
        port = settings->get(Settings::MidiPort);
    }

    // Initialize RtMidi and set the port.
    myDevice = std::make_unique<MidiOutputDevice>();
    if (!myDevice->initialize(api, port))
    {
        myDevice.reset();
        QString msg = tr("Error initializing MIDI output device.");

        // The snap package doesn't have ALSA / JACK access by default, so
        // suggest how to grant access.
#ifdef PTE_BUILDING_FOR_SNAP
        msg += "\n\n";
        msg += tr("The MIDI device may not be accessible. The following "
                  "commands can be run to allow access:");
        msg += "\nsudo snap connect powertabeditor:alsa";
        msg += "\nsudo snap connect powertabeditor:jack1";
#endif

        emit error(msg);
        return;
    }
}

void
MidiPlayer::updateLiveSettings()
{
    auto settings = mySettingsManager.getReadHandle();
    myMetronomeEnabled = settings->get(Settings::MetronomeEnabled);
}

void
MidiPlayer::liveChangePlaybackSpeed(int speed)
{
    myPlaybackSpeed = speed;
}

void
MidiPlayer::liveChangePlayerSettings(int player, uint8_t max_volume,
                                     uint8_t pan)
{
    const int channel = Midi::getPlayerChannel(player);
    myChannelMaxVolumes[channel] = max_volume;
    myChannelPans[channel] = pan;
}

static void
loadMidiSettings(const SettingsManager &settings_manager,
                 MidiFile::LoadOptions &options)
{
    auto settings = settings_manager.getReadHandle();

    options.myMetronomePreset = settings->get(Settings::MetronomePreset) +
                                Midi::MIDI_PERCUSSION_PRESET_OFFSET;
    options.myStrongAccentVel = settings->get(Settings::MetronomeStrongAccent);
    options.myWeakAccentVel = settings->get(Settings::MetronomeWeakAccent);
    options.myVibratoStrength = settings->get(Settings::MidiVibratoLevel);
    options.myWideVibratoStrength =
        settings->get(Settings::MidiWideVibratoLevel);
}

static MidiEventList
mergeMidiEvents(MidiFile &file)
{
    // Merge the MIDI evvents for each track.
    MidiEventList events;
    for (MidiEventList &track : file.getTracks())
    {
        track.convertToAbsoluteTicks();
        events.concat(track);
    }

    // TODO - since each track is already sorted, an n-way merge should be
    // faster.
    std::stable_sort(events.begin(), events.end());
    events.convertToDeltaTicks();

    return events;
}

bool
MidiPlayer::playEvents(MidiFile &file, const SystemLocation &start_location,
                       const MidiPlaybackSettings &initial_settings,
                       bool allow_count_in, const Score *score)
{
    setPlaybackSettings(initial_settings);

    myIsPlaying = true;
    Util::ScopeExit on_exit([&]() {
        myIsPlaying = false;
    });

    MidiEventList events = mergeMidiEvents(file);
    const int ticks_per_beat = file.getTicksPerBeat();

    bool started = false;
    Midi::Tempo beat_duration = Midi::BEAT_DURATION_120_BPM;
    SystemLocation current_location = start_location;
    DurationType clock_drift(0);

    std::array<uint8_t, Midi::NUM_MIDI_CHANNELS_PER_PORT> initial_pitch_wheel;
    initial_pitch_wheel.fill(Midi::DEFAULT_BEND);

    for (const MidiEvent &event : events)
    {
        if (!myIsPlaying)
            return false;

        if (event.isTempoChange())
            beat_duration = event.getTempo();

        // Skip note on / off events before the start location, but send events
        // such as instrument changes, pitch wheels, etc.
        // Tempo changes are tracked above and shouldn't be sent out since
        // CoreMidi on OSX complains about them.
        if (!started)
        {
            if (event.getLocation() < start_location)
            {
                if (event.isVolumeChange())
                {
                    // Use MidiOutputDevice::setVolume() so that the volume is
                    // updated when the channel's max volume changes (see
                    // below).
                    myDevice->setVolume(event.getChannel(), event.getVolume());
                }
                else if (event.isPitchWheel())
                {
                    // On Windows (GS wavetable synth) pitch wheel events seem
                    // to get lost if they're sent too rapidly (bug 395). Just
                    // record the latest pitch bend value and set the final
                    // value before starting playback.
                    initial_pitch_wheel[event.getChannel()] = event.getPitchWheelValue();
                }
                else if (!event.isNoteOnOff() && !event.isTempoChange())
                    myDevice->sendMessage(event.getData());

                continue;
            }
            else
            {
                for (int i = 0, n = int(initial_pitch_wheel.size()); i < n; ++i)
                    myDevice->setPitchBend(i, initial_pitch_wheel[i]);

                if (allow_count_in)
                    performCountIn(*score, event.getLocation(), beat_duration);

                started = true;
            }
        }

        auto start_timestamp = std::chrono::high_resolution_clock::now();

        const int delta = event.getTicks();
        assert(delta >= 0);

		// Compute the time in microseconds that we should sleep for, and then
        // adjust for accumulated timing errors (since sleep_for() is not
        // perfectly precise).
        auto sleep_duration = DurationType(static_cast<int64_t>(
            boost::rational_cast<int64_t>(
                boost::rational<int64_t>(delta, ticks_per_beat) *
                beat_duration.count()) *
            (100.0 / myPlaybackSpeed)));

        auto error_correction = std::min(sleep_duration, clock_drift);
        clock_drift -= error_correction;
        sleep_duration -= error_correction;

        if (sleep_duration.count() != 0)
            std::this_thread::sleep_for(sleep_duration);

        // Don't play metronome events if the metronome is disabled.
        // Tempo change events also don't need to be sent since they are
        // handled in this loop. CoreMidi on OSX also complains about them.
        // Similarly, ALSA complains about the meta "track end" events.
        if (!(event.isNoteOnOff() &&
              event.getChannel() == METRONOME_CHANNEL &&
              !myMetronomeEnabled) &&
            !event.isTempoChange() &&
            !event.isTrackEnd() &&
            !event.isVolumeChange())
        {
            myDevice->sendMessage(event.getData());
        }

        if (!event.isMetaMessage())
        {
            const int channel = event.getChannel();
            myDevice->setChannelMaxVolume(channel, myChannelMaxVolumes[channel]);
            myDevice->setPan(channel, myChannelPans[channel]);

            // handle volume change events
            // using device.setVolume() ensures that the maximum volume
            // threshold is taken into consideration
            if (event.isVolumeChange())
                myDevice->setVolume(channel, event.getVolume());
        }

        // Notify listeners of the current playback position.
        if (event.getLocation() != current_location)
        {
            const SystemLocation &new_location = event.getLocation();

            // Don't move backwards unless a repeat occurred.
            if (new_location >= current_location || event.isPositionChange())
            {
                if (new_location.getSystem() != current_location.getSystem())
                    emit playbackSystemChanged(new_location.getSystem());

                emit playbackPositionChanged(new_location.getPosition());

                current_location = new_location;
            }
        }

        // Accumulate any difference between the desired delta time and what
        // actually happened.
        auto end_timestamp = std::chrono::high_resolution_clock::now();
        auto actual_duration = std::chrono::duration_cast<DurationType>(
            end_timestamp - start_timestamp);
        clock_drift += actual_duration - sleep_duration;
    }

    return true;
}

void
MidiPlayer::playScore(const ConstScoreLocation &start_score_location,
                      const MidiPlaybackSettings &initial_settings)
{
    myStartLocation.emplace(start_score_location);
    const Score &score = start_score_location.getScore();

    MidiFile::LoadOptions options;
    options.myEnableMetronome = true;
    options.myRecordPositionChanges = true;
    loadMidiSettings(mySettingsManager, options);

    MidiFile file;
    file.load(score, options);

    const SystemLocation start_location(myStartLocation->getSystemIndex(),
                                        myStartLocation->getPositionIndex());

    if (playEvents(file, start_location, initial_settings, true,
                   &score))
    {
        emit playbackFinished();
    }
}

MidiFile
MidiPlayer::generateSingleNote(const ConstScoreLocation &location,
                               const SettingsManager &settings)
{
    const Score &score = location.getScore();

    MidiFile::LoadOptions options;
    options.myEnableMetronome = false;
    options.myRecordPositionChanges = false;
    loadMidiSettings(settings, options);

    MidiFile file;
    file.loadSingleNote(score, location, options);
    return file;
}

void
MidiPlayer::playSingleNote(MidiFile &file, const ConstScoreLocation &location,
                           const MidiPlaybackSettings &initial_settings)
{
    SystemLocation start_location(location.getSystemIndex(),
                                  location.getPositionIndex());

    playEvents(file, start_location, initial_settings,
               /* allow_count_in */ false, nullptr);
    myDevice->stopAllNotes();
}

void
MidiPlayer::performCountIn(const Score &score, const SystemLocation &location,
                           Midi::Tempo beat_duration)
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
    const System &system = score.getSystems()[location.getSystem()];
    const Barline *barline = system.getPreviousBarline(location.getPosition());
    if (!barline)
        barline = &system.getBarlines().front();

    const TimeSignature &time_sig = barline->getTimeSignature();

    const auto tick_duration = DurationType(static_cast<int64_t>(
        boost::rational_cast<int64_t>(
            boost::rational<int64_t>(4, time_sig.getBeatValue()) *
            boost::rational<int64_t>(time_sig.getBeatsPerMeasure(),
                                     time_sig.getNumPulses()) *
            beat_duration.count()) *
        100.0 / myPlaybackSpeed));

    // Play the count-in.
    myDevice->setChannelMaxVolume(METRONOME_CHANNEL,
                               Midi::MAX_MIDI_CHANNEL_VOLUME);

    for (int i = 0; i < time_sig.getNumPulses(); ++i)
    {
        if (!myIsPlaying)
            return;

        myDevice->playNote(METRONOME_CHANNEL, preset, velocity);
        std::this_thread::sleep_for(tick_duration);
        myDevice->stopNote(METRONOME_CHANNEL, preset);
    }
}

void MidiPlayer::stopPlayback()
{
    if (myIsPlaying)
    {
        myIsPlaying = false;
        // Wait for playback to actually finish.
        QMetaObject::invokeMethod(
            this, []() { /* Do nothing */ }, Qt::BlockingQueuedConnection);
    }

    myDevice->stopAllNotes();
}

void
MidiPlayer::setPlaybackSettings(const MidiPlaybackSettings &settings)
{
    myPlaybackSpeed = settings.myPlaybackSpeed;

    std::fill(myChannelMaxVolumes.begin(), myChannelMaxVolumes.end(),
              Midi::MAX_MIDI_CHANNEL_VOLUME);
    std::fill(myChannelPans.begin(), myChannelPans.end(), Midi::DEFAULT_PAN);

    // Set the initial volume and pan values.
    for (size_t i = 0, n = settings.myPlayerMaxVolumes.size(); i < n; ++i)
    {
        const int channel = Midi::getPlayerChannel(i);
        myChannelMaxVolumes[channel] = settings.myPlayerMaxVolumes[i];
        myChannelPans[channel] = settings.myPlayerPans[i];
    }
}

MidiPlaybackSettings::MidiPlaybackSettings(int speed, const Score &score)
    : myPlaybackSpeed(speed)
{
    for (const Player &player : score.getPlayers())
    {
        myPlayerMaxVolumes.push_back(player.getMaxVolume());
        myPlayerPans.push_back(player.getPan());
    }
}
