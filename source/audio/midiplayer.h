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

#include <QThread>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <QMutex>

class Barline;
class MidiEvent;
class Note;
class Position;
class Score;
class Staff;
class System;
class TempoMarker;
class Tuning;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(const Score &score, int startSystem, int startPosition,
               int speed);
    ~MidiPlayer();

signals:
    // These signals are used to move the caret when a position change is
    // necessary
    void playbackSystemChanged(int system);
    void playbackPositionChanged(int position);
#if 0
public slots:
    void changePlaybackSpeed(int newPlaybackSpeed);
#endif

private:
    void run();
    void setIsPlaying(bool set);
    bool isPlaying() const;

    void generateEvents(boost::ptr_list<MidiEvent> &eventList);
    void playMidiEvents(const boost::ptr_list<MidiEvent> &eventList);

    /// Generates a list of all notes in the given bar.
    /// @returns The timestamp of the end of the last event in the bar.
    double generateEventsForBar(const System &system, int systemIndex,
                                const Staff &staff, int staffIndex, int voice,
                                int leftPos, int rightPos,
                                const double barStartTime,
                                boost::ptr_list<MidiEvent>& eventList);

    /// Returns the current tempo (duration of a quarter note in milliseconds).
    double getCurrentTempo(int system, int position) const;

    /// Returns the active tempo marker, if one exists.
    const TempoMarker *getCurrentTempoMarker(int systemIndex, int position) const;

    /// Calculates the duration of a note in the given position.
    double calculateNoteDuration(int system, const Position &pos) const;

    /// Computes the duration of a whole rest. If it's the only rest/note in the
    /// bar, then it lasts for the entire bar instead of 4 beats.
    double getWholeRestDuration(const System &system, int systemIndex,
                                const Staff &staff, int voice,
                                const Position &pos,
                                double originalDuration) const;

    /// Computes the pitch of a note, including things like harmonics.
    int getActualNotePitch(const Note &note, const Tuning &tuning) const;

    /// Generates metronome events for a bar.
    /// @param notesEndTime The timestamp of the last note event in the bar.
    double generateMetronome(const System &system, int systemIndex,
                             const Barline &barline, double startTime,
                             const double notesEndTime,
                             boost::ptr_list<MidiEvent> &eventList) const;

    const Score &myScore;
    const int myStartSystem;
    const int myStartPosition;
    bool myIsPlaying;
    /// Tracks the active pitch bend (used for "bend and hold"-type events).
    uint8_t myActivePitchBend;
    /// The current playback speed (percent).
    int myPlaybackSpeed;
    mutable QMutex myMutex;


    enum Durations
    {
        GraceNoteDuration = 60,
        ArpeggioOffset = 30
    };

#if 0

    /// Holds basic information about a bend - used to simplify the generateBends function
    struct BendEventInfo
    {
        BendEventInfo(double timestamp, uint8_t pitchBendAmout);

        double timestamp;
        uint8_t pitchBendAmount;
    };

    void generateBends(std::vector<BendEventInfo>& bends, double startTime, double duration,
                       double currentTempo, const Note* note);
    void generateSlides(std::vector<BendEventInfo>& bends, double startTime, double noteDuration,
                       double currentTempo, const Note* note);
    void generateGradualBend(std::vector<BendEventInfo>& bends, double startTime, double duration, int startBendAmount,
                             int releaseBendAmount) const;
    void generateTremoloBar(std::vector<BendEventInfo>& bends, double startTime,
                            double noteDuration, double currentTempo, const Position* position);



#endif
};

#endif
