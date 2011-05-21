#include "bendevent.h"

#include <rtmidiwrapper.h>
#include <powertabdocument/generalmidi.h>

/// Pitch bend range (in half steps)
const uint8_t BendEvent::PITCH_BEND_RANGE = 24;

const uint8_t BendEvent::DEFAULT_BEND = 64;

/// Pitch bend amount to bend a note by a quarter tone
const double BendEvent::BEND_QUARTER_TONE = (midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL - BendEvent::DEFAULT_BEND) /
                                            (2.0 * BendEvent::PITCH_BEND_RANGE);

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
