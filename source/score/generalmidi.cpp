/*
  * Copyright (C) 2013 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "generalmidi.h"

#include <algorithm>
#include <cstdlib>
#include <stdexcept>

namespace Midi {

    // Pitches (relative to C = 0) corresponding to the keys in the keyText
    // array.
    static const uint8_t NUM_PITCH_CLASSES = 35;
    static const uint8_t pitchClasses[NUM_PITCH_CLASSES] = {
        3, 10, 5, 0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5,
        0, 7, 2, 9, 4, 11, 6, 1, 8, 3, 10, 5, 0, 7, 2,
        9, 4, 11, 6, 1
    };

    // All of the possible keys, arranged in order of the circle of fifths.
    static const std::string keyText[NUM_PITCH_CLASSES] = {
        "Fbb", "Cbb", "Gbb", "Dbb", "Abb", "Ebb", "Bbb",
        "Fb", "Cb", "Gb", "Db", "Ab", "Eb", "Bb",
        "F", "C", "G", "D", "A", "E", "B",
        "F#", "C#", "G#", "D#", "A#", "E#", "B#",
        "F##", "C##", "G##", "D##", "A##", "E##", "B##",
    };

    static const std::string sharpNotesText[12] = {"C", "C#", "D", "D#", "E",
        "F", "F#", "G", "G#", "A", "A#", "B"};

    static const std::string flatNotesText[12] = {"C", "Db", "D", "Eb", "E",
        "F", "Gb", "G", "Ab", "A", "Bb", "B"};

    bool isValidMidiChannel(uint8_t channel)
    {
        return channel <= LAST_MIDI_CHANNEL;
    }

    bool isValidMidiChannelVolume(uint8_t volume)
    {
        return volume <= MAX_MIDI_CHANNEL_VOLUME;
    }

    bool isValidMidiChannelEffectLevel(uint8_t level)
    {
        return level <= MAX_MIDI_CHANNEL_EFFECT_LEVEL;
    }

    bool isValidMidiNote(uint8_t note)
    {
        return note <= MAX_MIDI_NOTE;
    }

    /// Helper function to find the index of a key, for use with the
    /// pitchClasses and keyText arrays.
    int getKeyIndex(bool minor, bool usesSharps, uint8_t numAccidentals)
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

    std::string getMidiNoteText(uint8_t note, bool minor, bool usesSharps,
                                uint8_t numAccidentals, bool forceAccidentals)
    {
        if (!isValidMidiNote(note))
            throw std::out_of_range("Invalid MIDI note value");

        const uint8_t pitch = getMidiNotePitch(note);

        // Find the index of the key (for the pitchClasses and keyText arrays).
        const int tonic = getKeyIndex(minor, usesSharps, numAccidentals);

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

        std::string text = keyText[bestMatch];

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

    std::string getMidiNoteTextSimple(uint8_t note, bool sharps)
    {
        if (!isValidMidiNote(note))
            throw std::out_of_range("Invalid MIDI note value");

        const uint8_t pitch = getMidiNotePitch(note);

        return sharps ? sharpNotesText[pitch] : flatNotesText[pitch];
    }

    uint8_t offsetMidiNote(uint8_t note, int8_t offset)
    {
        int32_t temp = note + offset;

        if (temp < MIN_MIDI_NOTE)
            temp = MIN_MIDI_NOTE;
        else if (temp > MAX_MIDI_NOTE)
            temp = MAX_MIDI_NOTE;

        return (uint8_t)temp;
    }

    bool isValidMidiNoteKey(uint8_t key)
    {
        return key <= MAX_MIDI_NOTE_KEY;
    }

    std::string getKeyText(bool minor, bool usesSharps, uint8_t numAccidentals)
    {
        int tonic = getKeyIndex(minor, usesSharps, numAccidentals);
        return keyText[tonic];
    }

    uint8_t getMidiNotePitch(uint8_t note)
    {
        return note % 12;
    }

    int32_t getMidiNoteOctave(uint8_t note, char noteText)
    {
        int32_t octave = (note / 12) - 1;

        // Handle special cases of B# and Cb, where the normal octave
        // calculation is off by one.
        const uint8_t pitch = getMidiNotePitch(note);
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

    bool isValidMidiNoteVolume(uint8_t volume)
    {
        return volume <= MAX_MIDI_NOTE_VOLUME;
    }

    bool isValidMidiPreset(uint8_t preset)
    {
        return preset <= LAST_MIDI_PRESET;
    }

    bool isValidMidiPercussionPreset(uint8_t preset)
    {
        return preset <= LAST_MIDI_PERCUSSION_PRESET;
    }

    void getMidiPresetNames(std::vector<std::string>& names)
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
