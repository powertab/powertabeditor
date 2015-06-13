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
#include <cstdint>
#include <memory>
#include <QThread>

class Barline;
class MidiEvent;
class MidiOutputDevice;
class Note;
class Position;
class Score;
class SystemLocation;
class Staff;
class System;
class TempoMarker;
class Tuning;
class Voice;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(const Score &score, int startSystem, int startPosition,
               int speed);
    ~MidiPlayer();

    void changePlaybackSpeed(int newPlaybackSpeed);

signals:
    // These signals are used to move the caret when a position change is
    // necessary
    void playbackSystemChanged(int system);
    void playbackPositionChanged(int position);
    void error(const QString &msg);

private:
    typedef std::vector<std::unique_ptr<MidiEvent>> EventList;

    virtual void run() override;
    void setIsPlaying(bool set);
    bool isPlaying() const;

    void generateEvents(EventList &eventList);
    void playMidiEvents(const EventList &eventList);
    void performCountIn(MidiOutputDevice &device,
                        const SystemLocation &location);

    /// Generates a list of all notes in the given bar.
    /// @returns The timestamp of the end of the last event in the bar.
    double generateEventsForBar(const System &system, int systemIndex,
                                const Staff &staff, int staffIndex,
                                const Voice &voice, int voiceIndex, int leftPos,
                                int rightPos, const double barStartTime,
                                EventList &eventList, uint8_t &activePitchBend);

    /// Returns the current tempo (duration of a quarter note in milliseconds).
    double getCurrentTempo(int system, int position) const;

    /// Returns the active tempo marker, if one exists.
    const TempoMarker *getCurrentTempoMarker(int systemIndex, int position) const;

    /// Calculates the duration of a note in the given position.
    double calculateNoteDuration(int system, const Voice &voice,
                                 const Position &pos) const;

    /// Computes the duration of a whole rest. If it's the only rest/note in the
    /// bar, then it lasts for the entire bar instead of 4 beats.
    double getWholeRestDuration(const System &system, int systemIndex,
                                const Voice &voice, const Position &pos,
                                double originalDuration) const;

    /// Computes the pitch of a note, including things like harmonics.
    int getActualNotePitch(const Note &note, const Tuning &tuning) const;

    /// Generates metronome events for a bar.
    /// @param notesEndTime The timestamp of the last note event in the bar.
    double generateMetronome(const System &system, int systemIndex,
                             const Barline &barline, double startTime,
                             const double notesEndTime,
                             EventList &eventList) const;

    const Score &myScore;
    const int myStartSystem;
    const int myStartPosition;
    std::atomic<bool> myIsPlaying;
    /// The current playback speed (percent).
    std::atomic<int> myPlaybackSpeed;

    /// Holds basic information about a bend - used to simplify the generateBends function
    struct BendEventInfo
    {
        BendEventInfo(double timestamp, uint8_t pitchBendAmout);

        double timestamp;
        uint8_t pitchBendAmount;
    };

    void generateBends(std::vector<BendEventInfo> &bends,
                       uint8_t &activePitchBend, double startTime,
                       double duration, double currentTempo, const Note &note);

    void generateSlides(std::vector<BendEventInfo> &bends, double startTime,
                        double noteDuration, double currentTempo,
                        const Note &note, const Note *nextNote);

    /// Generates a series of BendEvents to perform a gradual bend over the
    /// given duration. Bends the note from the startBendAmount to the
    /// releaseBendAmount over the note duration.
    void generateGradualBend(std::vector<BendEventInfo> &bends,
                             double startTime, double duration,
                             int startBendAmount, int releaseBendAmount) const;
#if 0
    void generateTremoloBar(std::vector<BendEventInfo>& bends, double startTime,
                            double noteDuration, double currentTempo, const Position* position);
#endif

};

#endif
