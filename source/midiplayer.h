#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <rtmidiwrapper.h>

#include <QThread>
#include <QMutex>
#include <QHash>
#include <memory>

class Caret;
class Position;
class TempoMarker;
class Guitar;
class System;
class Staff;
class MidiEvent;

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
        ARPEGGIO_OFFSET = 30,
    };

    static const quint8 METRONOME_CHANNEL = 15;

    double getCurrentTempo(const quint32 positionIndex) const;
    TempoMarker* getCurrentTempoMarker(const quint32 positionIndex) const;
    double calculateNoteDuration(Position* currentPosition) const;

    double generateEventsForSystem(uint32_t systemIndex, double systemStartTime,
                                 std::list<std::unique_ptr<MidiEvent> >& eventList) const;
    void generateMetronome(uint32_t systemIndex, double startTime,
                           std::list<std::unique_ptr<MidiEvent> >& eventList) const;
    void playMidiEvents(std::list<std::unique_ptr<MidiEvent> >& eventList, quint32 startPos);
    double getWholeRestDuration(std::shared_ptr<System> system, Staff* staff, Position* position, double originalDuration) const;

    Caret* caret;
    RtMidiWrapper rtMidiWrapper;

    bool isPlaying;
    quint32 currentSystemIndex;

    QHash<quint8, quint8> naturalHarmonicPitches;
    void initNaturalHarmonics();
    quint8 getNaturalHarmonicPitch(const quint8 originalPitch, const quint8 fret) const;
};

#endif // MIDIPLAYER_H
