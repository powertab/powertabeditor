#include "bendevent.h"

#include <rtmidiwrapper.h>
#include <powertabdocument/generalmidi.h>

const uint8_t BendEvent::DEFAULT_BEND = 64;

/// Pitch bend amount to bend a note by a quarter tone
const double BendEvent::BEND_QUARTER_TONE = (midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL -
                                             BendEvent::DEFAULT_BEND) / 24.0; // Use a 24-step pitch bend range

BendEvent::BendEvent(uint8_t channel, double startTime, uint32_t positionIndex,
                     uint32_t systemIndex, uint8_t bendAmount) :
    MidiEvent(channel, startTime, 0, positionIndex, systemIndex),
    bendAmount(bendAmount)
{
}

void BendEvent::performEvent(RtMidiWrapper& sequencer) const
{
    sequencer.setPitchBend(channel, bendAmount);
}
