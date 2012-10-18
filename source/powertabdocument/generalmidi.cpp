/////////////////////////////////////////////////////////////////////////////
// Name:            generalmidi.cpp
// Purpose:         General MIDI macros, constants and functions
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 9, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "generalmidi.h"
#include "macros.h"

#include <cstdlib> // abs

#include <string>
using std::string;

namespace midi
{
    // Pitches (relative to C = 0) corresponding to the keys in the keyText array
    static const uint8_t NUM_PITCH_CLASSES = 35;
    static const uint8_t pitchClasses[NUM_PITCH_CLASSES] = {
        3, 10, 5, 0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5,
        0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5, 0, 7, 2,
        9, 4, 11, 6, 1
    };

    // All of the possible keys, arranged in order of the circle of fifths
    static const string keyText[NUM_PITCH_CLASSES] = {
        "Fbb", "Cbb", "Gbb", "Dbb", "Abb", "Ebb", "Bbb",
        "Fb", "Cb", "Gb", "Db", "Ab", "Eb", "Bb",
        "F", "C", "G", "D", "A", "E", "B",
        "F#", "C#", "G#", "D#", "A#", "E#", "B#",
        "F##", "C##", "G##", "D##", "A##", "E##", "B##",
    };

    static const string sharpNotesText[12] = {"C", "C#", "D", "D#", "E", "F", "F#",
                                              "G", "G#", "A", "A#", "B"};

    static const string flatNotesText[12] = {"C", "Db", "D", "Eb", "E", "F", "Gb",
                                             "G", "Ab", "A", "Bb", "B"};

    // MIDI Channel Functions
    /// Determines if a MIDI channel is valid
    /// @param MIDI channel to validate
    /// @return True if the MIDI channel is valid, false if not
    bool IsValidMidiChannel(uint8_t channel)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (channel <= LAST_MIDI_CHANNEL);
    }

    bool IsValidMidiChannelVolume(uint8_t volume)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (volume <= MAX_MIDI_CHANNEL_VOLUME);
    }

    /// Determines if a MIDI channel effect level is valid
    /// @param level MIDI channel effect level to validate
    /// @return True if the MIDI channel effect level is valid, false if not
    bool IsValidMidiChannelEffectLevel(uint8_t level)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (level <= MAX_MIDI_CHANNEL_EFFECT_LEVEL);
    }

    // MIDI Note Functions
    /// Determines if a MIDI note value is valid
    /// @param note MIDI note to validate
    /// @return True if the MIDI note value is valid, false if not
    bool IsValidMidiNote(uint8_t note)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (note <= MAX_MIDI_NOTE);
    }

    /// Helper function to find the index of a key, for use with the pitchClasses and keyText arrays
    int GetKeyIndex(bool usesSharps, uint8_t numAccidentals)
    {
        const int keyC = 15;
        int keyOffset = numAccidentals;
        if (!usesSharps)
        {
            keyOffset *= -1;
        }

        return keyC + keyOffset;
    }

    /// Gets an accurate text representation of a note, given a key
    /// signature.
    /// @param note A MIDI pitch.
    /// @param usesSharps Whether the key signature uses sharps or flats.
    /// @param numAccidentals The number of accidentals in the key signature.
    /// @param forceAccidentals Whether to display accidentals for notes that
    /// are in the key signature (useful for e.g. reinstating a flat or sharp in
    /// the key signature after a natural sign).
    string GetMidiNoteText(uint8_t note, bool usesSharps,
                           uint8_t numAccidentals, bool forceAccidentals)
    {
        CHECK_THAT(IsValidMidiNote(note), "");

        const uint8_t pitch = GetMidiNotePitch(note);

        // Find the index of the key (for the pitchClasses and keyText arrays).
        const int tonic = GetKeyIndex(usesSharps, numAccidentals);

        // Initial value needs to be larger than any possible distance.
        uint8_t minDistance = 100;
        // Index of the text representation that is the best match.
        uint8_t bestMatch = 0;

        // Find the correct text representation of the pitch, by finding the
        // representation that is the shortest number of steps away from the
        // tonic in the circle of fifths.
        for (uint8_t i = 0; i < NUM_PITCH_CLASSES; i++)
        {
            if (pitchClasses[i] == pitch)
            {
                uint8_t dist = abs(tonic - i);
                if (std::min(dist, minDistance) == dist)
                {
                    minDistance = dist;
                    bestMatch = i;
                }
            }
        }

        string text = keyText[bestMatch];

        // Add in a natural sign by default.
        if (text.length() == 1)
        {
            text.append("=");
        }

        // Remove accidentals or natural signs for notes in the key signature.
        if (tonic - 1 <= bestMatch && tonic + 5 >= bestMatch &&
            !forceAccidentals)
        {
            if (text.length() > 1)
            {
                text.erase(text.begin() + 1);
            }
        }

        return text;
    }

    /// Gets the text representation of a MIDI note. This is less accurate than
    /// GetMidiNoteText, which takes the key signature into account. However, it
    /// is useful for things like displaying the notes of a tuning.
    /// @param note MIDI note to get the text representation for
    /// @param sharps True to get the sharp representation of the note, false to
    /// get the flat representation of the note.
    /// @return A text representation of the MIDI note
    string GetMidiNoteTextSimple(uint8_t note, bool sharps)
    {
        CHECK_THAT(IsValidMidiNote(note), "");

        const uint8_t pitch = GetMidiNotePitch(note);

        if (sharps)
        {
            return sharpNotesText[pitch];
        }
        else
        {
            return flatNotesText[pitch];
        }
    }

    /// Offsets a MIDI note by an offset
    /// @param note MIDI note to offset
    /// @param offset Amount to offset the note
    /// @return The offset note
    uint8_t OffsetMidiNote(uint8_t note, int8_t offset)
    {
        //------Last Checked------//
        // - Dec 14, 2004
        int32_t temp = note + offset;

        if (temp < MIN_MIDI_NOTE)
            temp = MIN_MIDI_NOTE;
        else if (temp > MAX_MIDI_NOTE)
            temp = MAX_MIDI_NOTE;

        return ((uint8_t)(temp));
    }

    /// Determines if a MIDI note key is valid
    /// @param key MIDI note key to validate
    /// @return True if the MIDI note key is valid, false if not
    bool IsValidMidiNoteKey(uint8_t key)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (key <= MAX_MIDI_NOTE_KEY);
    }

    /// Returns a string representation of the tonic note of the given key
    /// e.g. GetKeyText(true, false, 1) -> F
    std::string GetKeyText(bool minor, bool usesSharps, uint8_t numAccidentals)
    {
        int tonic = GetKeyIndex(usesSharps, numAccidentals);
        if (minor)
        {
            tonic += 3;
        }
        return keyText[tonic];
    }

    /// Gets the pitch value for a MIDI note
    /// @param note MIDI note to get the pitch value for
    /// @return The pitch value for the MIDI note
    uint8_t GetMidiNotePitch(uint8_t note)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (note % 12);
    }

    /// Gets the octave value for a MIDI note
    /// @param note MIDI note to get the octave value for
    /// @param noteText Text representation of the note, without accidentals.
    /// This is used for handling unusual accidentals like B# or Cb.
    /// @return The octave value for the MIDI note
    int32_t GetMidiNoteOctave(uint8_t note, char noteText)
    {
        int32_t octave = (note / 12) - 1;

        // Handle special cases of B# and Cb, where the normal octave calculation
        // is off by one.
        const uint8_t pitch = GetMidiNotePitch(note);
        if (pitch == MIDI_NOTE_KEY_C && noteText == 'B')
        {
            octave--;
        }
        else if (pitch == MIDI_NOTE_KEY_B && noteText == 'C')
        {
            octave++;
        }

        return octave;
    }

    /// Determines if a MIDI note volume is valid
    /// @param volume MIDI note volume to validate
    /// @return True if the MIDI note volume is valid, false if not
    bool IsValidMidiNoteVolume(uint8_t volume)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (volume <= MAX_MIDI_NOTE_VOLUME);
    }

    // MIDI Preset Functions
    /// Determines if a MIDI preset is valid
    /// @param preset MIDI preset to validate
    /// @return True if the MIDI preset is valid, false if not
    bool IsValidMidiPreset(uint8_t preset)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (preset <= LAST_MIDI_PRESET);
    }

    // MIDI Percussion Preset Functions
    /// Determines if a MIDI percussion preset is valid
    /// @param preset MIDI percussion preset to validate
    /// @return True if the MIDI percussion preset is valid, false if not
    bool IsValidMidiPercussionPreset(uint8_t preset)
    {
        //------Last Checked------//
        // - Dec 9, 2004
        return (preset <= LAST_MIDI_PERCUSSION_PRESET);
    }
}
