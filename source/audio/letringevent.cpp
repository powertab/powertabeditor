#include "letringevent.h"

#include <rtmidiwrapper.h>
#include <QDebug>

LetRingEvent::LetRingEvent(uint8_t channel, double startTime, uint32_t positionIndex, uint32_t systemIndex,
                           EventType eventType) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    eventType(eventType)
{
}

void LetRingEvent::performEvent(RtMidiWrapper& sequencer) const
{
    qDebug() << "Let Ring " << ((eventType == LET_RING_ON) ? "On" : "Off") << ": "
                << systemIndex << ", " << positionIndex << " at " << startTime;

    sequencer.setSustain(channel, eventType == LET_RING_ON);
}
