#include "letringevent.h"

#include <rtmidiwrapper.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

LetRingEvent::LetRingEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex,
                           EventType eventType) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    eventType(eventType)
{
}

void LetRingEvent::performEvent(RtMidiWrapper& sequencer) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Let Ring " << ((eventType == LET_RING_ON) ? "On" : "Off") << ": "
                << systemIndex << ", " << positionIndex << " at " << startTime;
#endif

    sequencer.setSustain(channel, eventType == LET_RING_ON);
}
