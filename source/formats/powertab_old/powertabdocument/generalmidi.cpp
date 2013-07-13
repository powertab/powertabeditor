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

    /// Helper function to find the index of a key, for use with the
    /// pitchClasses and keyText arrays.
    int GetKeyIndex(bool minor, bool usesSharps, uint8_t numAccidentals)
    {
        const int keyC = 15;
        int keyOffset = numAccidentals;
        if (!usesSharps)
        {
            keyOffset *= -1;
        }

        if (minor)
        {
            keyOffset += 3;
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
    string GetMidiNoteText(uint8_t note, bool minor, bool usesSharps,
                           uint8_t numAccidentals, bool forceAccidentals)
    {
        PTB_CHECK_THAT(IsValidMidiNote(note), "");

        const uint8_t pitch = GetMidiNotePitch(note);

        // Find the index of the key (for the pitchClasses and keyText arrays).
        const int tonic = GetKeyIndex(minor, usesSharps, numAccidentals);

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

        int lowerLimit = tonic - 1;
        int upperLimit = tonic + 5;
        if (minor)
        {
            lowerLimit -= 3;
            upperLimit -= 3;
        }

        // Remove accidentals or natural signs for notes in the key signature.
        if (lowerLimit <= bestMatch && upperLimit >= bestMatch &&
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
        PTB_CHECK_THAT(IsValidMidiNote(note), "");

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
        int tonic = GetKeyIndex(minor, usesSharps, numAccidentals);
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

    void GetMidiPresetNames(std::vector<std::string>& names)
    {
        names.push_back("Acoustic Grand Piano");
        names.push_back("Bright Acoustic Piano");
        names.push_back("Electric Grand Piano");
        names.push_back("Honky-tonk Piano");
        names.push_back("Electric Piano 1");
        names.push_back("Electric Piano 2");
        names.push_back("Harpsichord");
        names.push_back("Clavinet");
        names.push_back("Celesta");
        names.push_back("Glockenspiel");
        names.push_back("Music Box");
        names.push_back("Vibraphone");
        names.push_back("Marimba");
        names.push_back("Xylophone");
        names.push_back("Tubular Bells");
        names.push_back("Dulcimer");
        names.push_back("Drawbar Organ");
        names.push_back("Percussive Organ");
        names.push_back("Rock Organ");
        names.push_back("Church Organ");
        names.push_back("Reed Organ");
        names.push_back("Accordion");
        names.push_back("Harmonica");
        names.push_back("Tango Accordion");
        names.push_back("Acoustic Guitar (nylon)");
        names.push_back("Acoustic Guitar (steel)");
        names.push_back("Electric Guitar (jazz)");
        names.push_back("Electric Guitar (clean)");
        names.push_back("Electric Guitar (muted)");
        names.push_back("Overdriven Guitar");
        names.push_back("Distortion Guitar");
        names.push_back("Guitar harmonics");
        names.push_back("Acoustic Bass");
        names.push_back("Electric Bass (finger)");
        names.push_back("Electric Bass (pick)");
        names.push_back("Fretless Bass");
        names.push_back("Slap Bass 1");
        names.push_back("Slap Bass 2");
        names.push_back("Synth Bass 1");
        names.push_back("Synth Bass 2");
        names.push_back("Violin");
        names.push_back("Viola");
        names.push_back("Cello");
        names.push_back("Contrabass");
        names.push_back("Tremolo Strings");
        names.push_back("Pizzicato Strings");
        names.push_back("Orchestral Harp");
        names.push_back("Timpani");
        names.push_back("String Ensemble 1");
        names.push_back("String Ensemble 2");
        names.push_back("Synth Strings 1");
        names.push_back("Synth Strings 2");
        names.push_back("Choir Aahs");
        names.push_back("Voice Oohs");
        names.push_back("Synth Voice");
        names.push_back("Orchestra Hit");
        names.push_back("Trumpet");
        names.push_back("Trombone");
        names.push_back("Tuba");
        names.push_back("Muted Trumpet");
        names.push_back("French Horn");
        names.push_back("Brass Section");
        names.push_back("Synth Brass 1");
        names.push_back("Synth Brass 2");
        names.push_back("Soprano Sax");
        names.push_back("Alto Sax");
        names.push_back("Tenor Sax");
        names.push_back("Baritone Sax");
        names.push_back("Oboe");
        names.push_back("English Horn");
        names.push_back("Bassoon");
        names.push_back("Clarinet");
        names.push_back("Piccolo");
        names.push_back("Flute");
        names.push_back("Recorder");
        names.push_back("Pan Flute");
        names.push_back("Blown Bottle");
        names.push_back("Shakuhachi");
        names.push_back("Whistle");
        names.push_back("Ocarina");
        names.push_back("Lead 1 (square)");
        names.push_back("Lead 2 (sawtooth)");
        names.push_back("Lead 3 (calliope)");
        names.push_back("Lead 4 (chiff)");
        names.push_back("Lead 5 (charang)");
        names.push_back("Lead 6 (voice)");
        names.push_back("Lead 7 (fifths)");
        names.push_back("Lead 8 (bass + lead)");
        names.push_back("Pad 1 (new age)");
        names.push_back("Pad 2 (warm)");
        names.push_back("Pad 3 (polysynth)");
        names.push_back("Pad 4 (choir)");
        names.push_back("Pad 5 (bowed)");
        names.push_back("Pad 6 (metallic)");
        names.push_back("Pad 7 (halo)");
        names.push_back("Pad 8 (sweep)");
        names.push_back("FX 1 (rain)");
        names.push_back("FX 2 (soundtrack)");
        names.push_back("FX 3 (crystal)");
        names.push_back("FX 4 (atmosphere)");
        names.push_back("FX 5 (brightness)");
        names.push_back("FX 6 (goblins)");
        names.push_back("FX 7 (echoes)");
        names.push_back("FX 8 (sci-fi)");
        names.push_back("Sitar");
        names.push_back("Banjo");
        names.push_back("Shamisen");
        names.push_back("Koto");
        names.push_back("Kalimba");
        names.push_back("Bag pipe");
        names.push_back("Fiddle");
        names.push_back("Shanai");
        names.push_back("Tinkle Bell");
        names.push_back("Agogo");
        names.push_back("Steel Drums");
        names.push_back("Woodblock");
        names.push_back("Taiko Drum");
        names.push_back("Melodic Tom");
        names.push_back("Synth Drum");
        names.push_back("Reverse Cymbal");
        names.push_back("Guitar Fret Noise");
        names.push_back("Breath Noise");
        names.push_back("Seashore");
        names.push_back("Bird Tweet");
        names.push_back("Telephone Ring");
        names.push_back("Helicopter");
        names.push_back("Applause");
        names.push_back("Gunshot");
    }
}
