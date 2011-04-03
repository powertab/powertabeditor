#include "metronomeevent.h"

#include <powertabdocument/generalmidi.h>
#include <rtmidiwrapper.h>
#include <QSettings>
#include <QDebug>

const uint8_t MetronomeEvent::METRONOME_PITCH = midi::MIDI_NOTE_MIDDLE_C;

MetronomeEvent::MetronomeEvent(uint8_t channel, double startTime, double duration,
                               uint32_t positionIndex, uint32_t systemIndex, VelocityType velocity) :
    MidiEvent(channel, startTime, duration, positionIndex, systemIndex),
    velocity(velocity)
{
}

void MetronomeEvent::performEvent(RtMidiWrapper& sequencer) const
{
    qDebug() << "Metronome: " << systemIndex << ", " << positionIndex;

    // check if the metronome has been disabled
    QSettings settings;

    VelocityType actualVelocity = velocity;
    if (settings.value("midi/metronomeEnabled").toBool() == false)
    {
        actualVelocity = METRONOME_OFF;
    }

    sequencer.setPatch(channel, midi::MIDI_PRESET_WOODBLOCK);
    sequencer.setVolume(channel, midi::MAX_MIDI_CHANNEL_VOLUME);
    sequencer.playNote(channel, METRONOME_PITCH, actualVelocity);
}
