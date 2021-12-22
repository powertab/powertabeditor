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

#ifndef AUDIO_MIDIPLAYER_H
#define AUDIO_MIDIPLAYER_H

#include <atomic>
#include <boost/signals2/connection.hpp>
#include <midi/midievent.h>
#include <QObject>
#include <score/scorelocation.h>
#include <optional>

class MidiFile;
class MidiOutputDevice;
class Score;
class SettingsManager;
class SystemLocation;

class MidiPlayer : public QObject
{
    Q_OBJECT

public:
    MidiPlayer(SettingsManager &settings_manager);
    ~MidiPlayer();

    /// Location where playback began. Only valid after calling playScore().
    const ConstScoreLocation &getStartLocation() const
    {
        return *myStartLocation;
    }

    void stopPlayback();

public slots:
    void init();
    void playScore(const ConstScoreLocation &start_score_location, int speed);
    void playSingleNote(const ConstScoreLocation &location);

    /// Thread-safe, Qt::DirectConnection may be used to invoke from another
    /// thread immediately while playback is running.
    void liveChangePlaybackSpeed(int speed);

signals:
    /// These signals are used to move the caret when a position change is
    /// necessary.
    void playbackSystemChanged(int system);
    void playbackPositionChanged(int position);
    void playbackFinished();

    void error(const QString &msg);

private slots:
    void updateDeviceSettings();
    /// Thread-safe, Qt::DirectConnection may be used to invoke from another
    /// thread immediately while playback is running.
    void updateLiveSettings();

private:
    void performCountIn(const Score &score,
                        const SystemLocation &location,
                        Midi::Tempo beat_duration);
    bool playEvents(MidiFile &file, const Score &score,
                    const SystemLocation &start_location,
                    bool allow_count_in = true);

    const SettingsManager &mySettingsManager;
    boost::signals2::scoped_connection mySettingsListener;

    std::unique_ptr<MidiOutputDevice> myDevice;
    std::atomic<bool> myMetronomeEnabled = false;

    /// Flag used to terminate playback.
    std::atomic<bool> myIsPlaying = false;
    /// The current playback speed (percent).
    std::atomic<int> myPlaybackSpeed = 100;
    /// Location where playback began.
    std::optional<ConstScoreLocation> myStartLocation;
};

#endif
