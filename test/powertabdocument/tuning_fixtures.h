#ifndef TUNING_FIXTURES_H
#define TUNING_FIXTURES_H

#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>

struct StandardTuningFixture
{
    StandardTuningFixture() :
        tuning("Tuning", 0, true, midi::MIDI_NOTE_E4, midi::MIDI_NOTE_B3,
            midi::MIDI_NOTE_G3, midi::MIDI_NOTE_D3, midi::MIDI_NOTE_A2,
            midi::MIDI_NOTE_E2)
    {
    }

    ~StandardTuningFixture() {}

    Tuning tuning;
};

#endif // TUNING_FIXTURES_H
