#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <rtmidiwrapper.h>

#include <QThread>
#include <QMutex>

class Caret;
class Position;
class TempoMarker;
class Guitar;

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

private:

    enum MessageType
    {
        PLAY_NOTE,
        STOP_NOTE,
        REST,
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

    double getCurrentTempo() const;
    TempoMarker* getCurrentTempoMarker() const;
    double calculateNoteDuration(Position* currentPosition) const;
    void generateNotesInSystem(int systemIndex, std::list<NoteInfo>& noteList) const;
    void playNotesInSystem(std::list<NoteInfo>& noteList);

    Caret* caret;
    RtMidiWrapper rtMidiWrapper;

    bool isPlaying;
    QMutex mutex;
};

#endif // MIDIPLAYER_H
