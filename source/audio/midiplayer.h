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
  
#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QThread>
#include <QMutex>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/ptr_container/ptr_list.hpp>
#include <powertabdocument/systemlocation.h>

class Caret;
class Position;
class TempoMarker;
class Guitar;
class System;
class Staff;
class MidiEvent;
class Note;
class Score;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(Caret* caret, int playbackSpeed);
    ~MidiPlayer();

signals:
    // these signals are used to notify the caret when a position change is necessary
    void playbackSystemChanged(quint32 systemIndex);
    void playbackPositionChanged(quint8 positionIndex);

public slots:
    void changePlaybackSpeed(int newPlaybackSpeed);

private:
    void run();

    void generateEvents(const Score* score,
                        boost::ptr_list<MidiEvent>& eventList);

    enum Durations
    {
        GRACE_NOTE_DURATION = 60,
        ARPEGGIO_OFFSET = 30
    };

    static const quint8 METRONOME_CHANNEL = 15;

    double getCurrentTempo(const SystemLocation& location) const;
    boost::shared_ptr<TempoMarker> getCurrentTempoMarker(const SystemLocation &location) const;
    double calculateNoteDuration(uint32_t systemIndex, const Position *currentPosition) const;

    double generateEventsForSystem(uint32_t systemIndex, double systemStartTime,
                                   boost::ptr_list<MidiEvent>& eventList);

    void generateMetronome(uint32_t systemIndex, double startTime,
                           boost::ptr_list<MidiEvent>& eventList) const;

    void playMidiEvents(boost::ptr_list<MidiEvent>& eventList, SystemLocation startLocation);

    double getWholeRestDuration(boost::shared_ptr<const System> system, boost::shared_ptr<const Staff> staff,
                                uint32_t systemIndex, const Position *position, double originalDuration) const;

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
    void generateGradualBend(std::vector<BendEventInfo>& bends, double startTime, double duration, uint8_t startBendAmount,
                             uint8_t releaseBendAmount) const;
    void generateTremoloBar(std::vector<BendEventInfo>& bends, double startTime,
                            double noteDuration, double currentTempo, const Position* position);
    
    uint32_t getActualNotePitch(const Note* note, boost::shared_ptr<const Guitar> guitar) const;

    Caret* caret;

    QMutex mutex;

    bool isPlaying;
    uint8_t activePitchBend; ///< keeps track of the active pitch bend (used for "bend and hold"-type events)
    int playbackSpeed; ///< Current playback speed (percent)
};

#endif // MIDIPLAYER_H
