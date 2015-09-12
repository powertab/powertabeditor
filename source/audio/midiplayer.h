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
class Score;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(const Score &score, int start_system, int start_pos,
               int speed);
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

    void setIsPlaying(bool set);
    bool isPlaying() const;

    const Score &myScore;
    SystemLocation myStartLocation;
    std::atomic<bool> myIsPlaying;
    /// The current playback speed (percent).
    std::atomic<int> myPlaybackSpeed;
};

#endif
