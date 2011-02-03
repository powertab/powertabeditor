#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <rtmidiwrapper.h>

#include <QThread>
#include <QMutex>
#include <QHash>

class Caret;
class Position;
class TempoMarker;
class Guitar;
class System;
class Staff;

class MidiPlayer : public QThread
{
    Q_OBJECT

public:
    MidiPlayer(Caret* caret);
    ~MidiPlayer();

signals:
    // these signals are used to notify the caret when a position change is necessary
    void playbackSystemChanged();
    void playbackPositionChanged(quint8 positionIndex);

protected:
    void run();

    enum MessageType
    {
        PLAY_NOTE,
        STOP_NOTE,
        REST,
    };

    enum NoteVelocities
    {
        DEFAULT_VELOCITY = 127,
        MUTED_VELOCITY = 60,
        GHOST_VELOCITY = 50,
        WEAK_ACCENT = 80,
        STRONG_ACCENT = 127,
    };

    // Holds information about a MIDI event
    struct NoteInfo
    {
        MessageType messageType;
        int channel;
        int pitch;
        Guitar* guitar;
        double duration;
        double startTime;
        quint8 position; // position of the note within the staff, used for moving the caret accordingly
        bool isMuted;
        bool isMetronome;
        int velocity;

        // used for sorting NoteInfo objects by their start time
        inline bool operator<(const NoteInfo& note)
        {
            return startTime < note.startTime;
        }

        inline bool operator==(const NoteInfo& note)
        {
            return messageType == note.messageType && channel == note.channel && pitch == note.pitch;
        }
    };

    double getCurrentTempo(const quint32 positionIndex) const;
    TempoMarker* getCurrentTempoMarker(const quint32 positionIndex) const;
    double calculateNoteDuration(Position* currentPosition) const;
    void generateNotesInSystem(int systemIndex, std::list<NoteInfo>& noteList) const;
    void generateMetronome(int systemIndex, std::list<NoteInfo>& noteList) const;
    void playNotesInSystem(std::list<NoteInfo>& noteList, int startPos);
    double getWholeRestDuration(System* system, Staff* staff, Position* position, double originalDuration) const;

    Caret* caret;
    RtMidiWrapper rtMidiWrapper;

    bool isPlaying;
    QMutex mutex;
    quint32 currentSystemIndex;

    QHash<quint8, quint8> naturalHarmonicPitches;
    void initNaturalHarmonics();
    quint8 getNaturalHarmonicPitch(const quint8 originalPitch, const quint8 fret) const;
};

#endif // MIDIPLAYER_H
