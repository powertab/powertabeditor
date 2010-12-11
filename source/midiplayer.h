#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include "rtmidiwrapper.h"

#include <QObject>
#include <QMap>
#include <QVector>

class Caret;
class QTimer;
class QSignalMapper;

class MidiPlayer : public QObject
{
    Q_OBJECT

public:
    MidiPlayer(Caret* caret);
    ~MidiPlayer();
    void play();
    void stop();

private slots:
    void playNotesAtCurrentPosition(int staff);
    void playbackSong(int staff);

private:
    Caret* caret;
    RtMidiWrapper rtMidiWrapper;
    QMap<quint32, quint32> previousPositionInStaff;
    QVector<QTimer*> songTimers;

    struct NoteHistory
    {
        unsigned int pitch; unsigned int stringNum;
        bool operator==(const NoteHistory& history) { return (pitch == history.pitch) && (stringNum == history.stringNum); }
    };

    QSignalMapper* signalMapper;
    bool isPlaying;
    QMultiMap<unsigned int, NoteHistory> oldNotes; // map channels to pitches

};

#endif // MIDIPLAYER_H
