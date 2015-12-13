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
#include <score/systemlocation.h>

class MidiFile;
class MidiOutputDevice;
class Score;
class SettingsManager;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(SettingsManager &settings_manager, const Score &score,
               int start_system, int start_pos, int speed);
    ~MidiPlayer();

    void changePlaybackSpeed(int new_speed);

signals:
    // These signals are used to move the caret when a position change is
    // necessary
    void playbackSystemChanged(int system);
    void playbackPositionChanged(int position);
    void error(const QString &msg);

private:
    virtual void run() override;

    void performCountIn(MidiOutputDevice &device,
                        const SystemLocation &location, int beat_duration);

    void setIsPlaying(bool set);
    bool isPlaying() const;

    SettingsManager &mySettingsManager;
    const Score &myScore;
    SystemLocation myStartLocation;
    std::atomic<bool> myIsPlaying;
    std::atomic<bool> myMetronomeEnabled;
    /// The current playback speed (percent).
    std::atomic<int> myPlaybackSpeed;
};

#endif
