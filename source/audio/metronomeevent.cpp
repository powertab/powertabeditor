#include "metronomeevent.h"

#include <powertabdocument/generalmidi.h>
#include <audio/rtmidiwrapper.h>
#include <QSettings>
#include <app/settings.h>

#if defined(LOG_MIDI_EVENTS)
#include <QDebug>
#endif

const uint8_t MetronomeEvent::METRONOME_PITCH = midi::MIDI_NOTE_MIDDLE_C;

MetronomeEvent::MetronomeEvent(uint8_t channel, double startTime, double duration,
                               uint32_t positionIndex, uint32_t systemIndex, VelocityType velocity) :
    MidiEvent(channel, startTime, duration, positionIndex, systemIndex),
    velocity(velocity)
{
}

void MetronomeEvent::performEvent(RtMidiWrapper& sequencer) const
{
#if defined(LOG_MIDI_EVENTS)
    qDebug() << "Metronome: " << systemIndex << ", " << positionIndex << " at " << startTime;
#endif

    // check if the metronome has been disabled
    QSettings settings;

    VelocityType actualVelocity = velocity;
    if (settings.value(Settings::MIDI_METRONOME_ENABLED,
                       Settings::MIDI_METRONOME_ENABLED_DEFAULT).toBool() == false)
    {
        actualVelocity = METRONOME_OFF;
    }

    sequencer.setPatch(channel, midi::MIDI_PRESET_WOODBLOCK);
    sequencer.setChannelMaxVolume(channel, midi::MAX_MIDI_CHANNEL_VOLUME);
    sequencer.playNote(channel, METRONOME_PITCH, actualVelocity);
}
