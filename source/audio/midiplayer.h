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
#include <QThread>
#include <midi/midievent.h>
#include <score/scorelocation.h>

class MidiFile;
class MidiOutputDevice;
class Score;
class SettingsManager;
class SystemLocation;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(SettingsManager &settings_manager,
               const ScoreLocation &start_location, int speed);
    ~MidiPlayer();

    void changePlaybackSpeed(int new_speed);

    const ScoreLocation &getStartLocation() const { return myStartLocation; }

signals:
    // These signals are used to move the caret when a position change is
    // necessary
    void playbackSystemChanged(int system);
    void playbackPositionChanged(int position);
    void error(const QString &msg);

private:
    virtual void run() override;

    void performCountIn(MidiOutputDevice &device,
                        const SystemLocation &location,
                        Midi::Tempo beat_duration);

    void setIsPlaying(bool set);
    bool isPlaying() const;

    // gets the player index for a specific channel
    static int getPlayerFromChannel(const int channel);

    SettingsManager &mySettingsManager;
    const Score &myScore;
    ScoreLocation myStartLocation;
    std::atomic<bool> myIsPlaying;
    std::atomic<bool> myMetronomeEnabled;
    /// The current playback speed (percent).
    std::atomic<int> myPlaybackSpeed;
};

#endif
