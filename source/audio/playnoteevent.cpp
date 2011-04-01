#include "playnoteevent.h"

#include <rtmidiwrapper.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/guitar.h>

#include <assert.h>

using std::shared_ptr;

PlayNoteEvent::PlayNoteEvent(uint8_t channel, double startTime, double duration, uint8_t pitch, uint32_t positionIndex,
                             uint32_t systemIndex, shared_ptr<Guitar> guitar, bool isMuted, VelocityType velocity) :
    MidiEvent(channel, startTime, duration, positionIndex, systemIndex),
    pitch(pitch),
    guitar(guitar),
    isMuted(isMuted),
    velocity(velocity)
{
}

void PlayNoteEvent::performEvent(RtMidiWrapper& sequencer) const
{
    assert(guitar != NULL);

    // grab the patch/pan/volume immediately before playback to allow for real-time mixing
    if (isMuted)
    {
        sequencer.setPatch(channel, midi::MIDI_PRESET_ELECTRIC_GUITAR_MUTED);
    }
    else
    {
        sequencer.setPatch(channel, guitar->GetPreset());
    }

    sequencer.setPan(channel, guitar->GetPan());
    sequencer.setVolume(channel, guitar->GetInitialVolume());

    sequencer.playNote(channel, pitch, velocity);
}
