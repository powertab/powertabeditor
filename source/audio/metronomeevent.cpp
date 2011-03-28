#include "metronomeevent.h"

#include <powertabdocument/generalmidi.h>
#include <rtmidiwrapper.h>
#include <QSettings>

const uint8_t MetronomeEvent::METRONOME_PITCH = midi::MIDI_NOTE_MIDDLE_C;

MetronomeEvent::MetronomeEvent(uint8_t channel, double startTime,
                               double duration, uint32_t positionIndex, VelocityType velocity) :
    MidiEvent(channel, startTime, duration, positionIndex),
    velocity(velocity)
{
}

void MetronomeEvent::performEvent(RtMidiWrapper& sequencer)
{
    // check if the metronome has been disabled
    QSettings settings;
    if (settings.value("midi/metronomeEnabled").toBool() == false)
    {
        velocity = METRONOME_OFF;
    }

    sequencer.setPatch(channel, midi::MIDI_PRESET_WOODBLOCK);
    sequencer.setVolume(channel, midi::MAX_MIDI_CHANNEL_VOLUME);
    sequencer.playNote(channel, METRONOME_PITCH, velocity);
}
