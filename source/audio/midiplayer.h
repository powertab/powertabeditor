#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <QThread>
#include <QMutex>
#include <memory>
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

protected:
    void run();

    enum Durations
    {
        GRACE_NOTE_DURATION = 60,
        ARPEGGIO_OFFSET = 30
    };

    static const quint8 METRONOME_CHANNEL = 15;

    double getCurrentTempo(const quint32 positionIndex) const;
    std::shared_ptr<TempoMarker> getCurrentTempoMarker(const quint32 positionIndex) const;
    double calculateNoteDuration(const Position* currentPosition) const;

    double generateEventsForSystem(uint32_t systemIndex, double systemStartTime,
                                   boost::ptr_list<MidiEvent>& eventList);

    void generateMetronome(uint32_t systemIndex, double startTime,
                           boost::ptr_list<MidiEvent>& eventList) const;

    void playMidiEvents(boost::ptr_list<MidiEvent>& eventList, SystemLocation startLocation);

    double getWholeRestDuration(std::shared_ptr<const System> system, std::shared_ptr<const Staff> staff,
                                const Position* position, double originalDuration) const;

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
    
    uint32_t getActualNotePitch(const Note* note, std::shared_ptr<const Guitar> guitar) const;

    Caret* caret;

    QMutex mutex;

    bool isPlaying;
    quint32 currentSystemIndex;
    uint8_t activePitchBend; ///< keeps track of the active pitch bend (used for "bend and hold"-type events)
    int playbackSpeed; ///< Current playback speed (percent)
};

#endif // MIDIPLAYER_H
