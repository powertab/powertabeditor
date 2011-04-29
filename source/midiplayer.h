#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QThread>
#include <QHash>
#include <memory>
#include <boost/ptr_container/ptr_list.hpp>

class Caret;
class Position;
class TempoMarker;
class Guitar;
class System;
class Staff;
class MidiEvent;
class Note;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(Caret* caret);
    ~MidiPlayer();

signals:
    // these signals are used to notify the caret when a position change is necessary
    void playbackSystemChanged(quint32 systemIndex);
    void playbackPositionChanged(quint8 positionIndex);

protected:
    void run();

    enum Durations
    {
        GRACE_NOTE_DURATION = 60,
        ARPEGGIO_OFFSET = 30
    };

    static const quint8 METRONOME_CHANNEL = 15;

    double getCurrentTempo(const quint32 positionIndex) const;
    TempoMarker* getCurrentTempoMarker(const quint32 positionIndex) const;
    double calculateNoteDuration(const Position* currentPosition) const;

    double generateEventsForSystem(uint32_t systemIndex, double systemStartTime,
                                   boost::ptr_list<MidiEvent>& eventList) const;
    void generateMetronome(uint32_t systemIndex, double startTime,
                           boost::ptr_list<MidiEvent>& eventList) const;
    void playMidiEvents(boost::ptr_list<MidiEvent>& eventList, uint32_t startSystem, uint32_t startPos);
    double getWholeRestDuration(std::shared_ptr<const System> system, const Staff* staff, 
                                const Position* position, double originalDuration) const;

    void generateBends(boost::ptr_list<MidiEvent>& eventList, uint8_t channel, const Note* note,
                       double startTime, double duration, uint32_t positionIndex, uint32_t systemIndex) const;
    
    uint32_t getActualNotePitch(const Note* note, std::shared_ptr<const Guitar> guitar) const;

    Caret* caret;

    bool isPlaying;
    quint32 currentSystemIndex;

    QHash<quint8, quint8> harmonicPitches;
    void initHarmonicPitches();
    quint8 getHarmonicPitch(const quint8 originalPitch, const quint8 fret) const;
};

#endif // MIDIPLAYER_H
