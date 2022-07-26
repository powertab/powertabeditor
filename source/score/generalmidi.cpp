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

    const std::vector<std::string> &getPresetNames()
    {
        using namespace std::literals;
        static const std::vector<std::string> names = {
            "Acoustic Grand Piano"s,
            "Bright Acoustic Piano"s,
            "Electric Grand Piano"s,
            "Honky-tonk Piano"s,
            "Electric Piano 1"s,
            "Electric Piano 2"s,
            "Harpsichord"s,
            "Clavinet"s,
            "Celesta"s,
            "Glockenspiel"s,
            "Music Box"s,
            "Vibraphone"s,
            "Marimba"s,
            "Xylophone"s,
            "Tubular Bells"s,
            "Dulcimer"s,
            "Drawbar Organ"s,
            "Percussive Organ"s,
            "Rock Organ"s,
            "Church Organ"s,
            "Reed Organ"s,
            "Accordion"s,
            "Harmonica"s,
            "Tango Accordion"s,
            "Acoustic Guitar (nylon)"s,
            "Acoustic Guitar (steel)"s,
            "Electric Guitar (jazz)"s,
            "Electric Guitar (clean)"s,
            "Electric Guitar (muted)"s,
            "Overdriven Guitar"s,
            "Distortion Guitar"s,
            "Guitar harmonics"s,
            "Acoustic Bass"s,
            "Electric Bass (finger)"s,
            "Electric Bass (pick)"s,
            "Fretless Bass"s,
            "Slap Bass 1"s,
            "Slap Bass 2"s,
            "Synth Bass 1"s,
            "Synth Bass 2"s,
            "Violin"s,
            "Viola"s,
            "Cello"s,
            "Contrabass"s,
            "Tremolo Strings"s,
            "Pizzicato Strings"s,
            "Orchestral Harp"s,
            "Timpani"s,
            "String Ensemble 1"s,
            "String Ensemble 2"s,
            "Synth Strings 1"s,
            "Synth Strings 2"s,
            "Choir Aahs"s,
            "Voice Oohs"s,
            "Synth Voice"s,
            "Orchestra Hit"s,
            "Trumpet"s,
            "Trombone"s,
            "Tuba"s,
            "Muted Trumpet"s,
            "French Horn"s,
            "Brass Section"s,
            "Synth Brass 1"s,
            "Synth Brass 2"s,
            "Soprano Sax"s,
            "Alto Sax"s,
            "Tenor Sax"s,
            "Baritone Sax"s,
            "Oboe"s,
            "English Horn"s,
            "Bassoon"s,
            "Clarinet"s,
            "Piccolo"s,
            "Flute"s,
            "Recorder"s,
            "Pan Flute"s,
            "Blown Bottle"s,
            "Shakuhachi"s,
            "Whistle"s,
            "Ocarina"s,
            "Lead 1 (square)"s,
            "Lead 2 (sawtooth)"s,
            "Lead 3 (calliope)"s,
            "Lead 4 (chiff)"s,
            "Lead 5 (charang)"s,
            "Lead 6 (voice)"s,
            "Lead 7 (fifths)"s,
            "Lead 8 (bass + lead)"s,
            "Pad 1 (new age)"s,
            "Pad 2 (warm)"s,
            "Pad 3 (polysynth)"s,
            "Pad 4 (choir)"s,
            "Pad 5 (bowed)"s,
            "Pad 6 (metallic)"s,
            "Pad 7 (halo)"s,
            "Pad 8 (sweep)"s,
            "FX 1 (rain)"s,
            "FX 2 (soundtrack)"s,
            "FX 3 (crystal)"s,
            "FX 4 (atmosphere)"s,
            "FX 5 (brightness)"s,
            "FX 6 (goblins)"s,
            "FX 7 (echoes)"s,
            "FX 8 (sci-fi)"s,
            "Sitar"s,
            "Banjo"s,
            "Shamisen"s,
            "Koto"s,
            "Kalimba"s,
            "Bag pipe"s,
            "Fiddle"s,
            "Shanai"s,
            "Tinkle Bell"s,
            "Agogo"s,
            "Steel Drums"s,
            "Woodblock"s,
            "Taiko Drum"s,
            "Melodic Tom"s,
            "Synth Drum"s,
            "Reverse Cymbal"s,
            "Guitar Fret Noise"s,
            "Breath Noise"s,
            "Seashore"s,
            "Bird Tweet"s,
            "Telephone Ring"s,
            "Helicopter"s,
            "Applause"s,
            "Gunshot"s,
        };

        return names;
    }

    const std::vector<std::string> &getPercussionPresetNames()
    {
        using namespace std::literals;
        static const std::vector<std::string> names = {
            "Acoustic Bass Drum"s,
            "Bass Drum 1"s,
            "Side Stick"s,
            "Acoustic Snare"s,
            "Hand Clap"s,
            "Electric Snare"s,
            "Low Floor Tom"s,
            "Closed Hi-Hat"s,
            "High Floor Tom"s,
            "Pedal Hi-Hat"s,
            "Low Tom"s,
            "Open Hi-Hat"s,
            "Low-Mid Tom"s,
            "Hi-Mid Tom"s,
            "Crash Cymbal 1"s,
            "High Tom"s,
            "Ride Cymbal 1"s,
            "Chinese Cymbal"s,
            "Ride Bell"s,
            "Tambourine"s,
            "Splash Cymbal"s,
            "Cowbell"s,
            "Crash Cymbal 2"s,
            "Vibraslap"s,
            "Ride Cymbal 2"s,
            "Hi Bongo"s,
            "Low Bongo"s,
            "Mute Hi Conga"s,
            "Open Hi Conga"s,
            "Low Conga"s,
            "High Timbale"s,
            "Low Timbale"s,
            "High Agogo"s,
            "Low Agogo"s,
            "Cabasa"s,
            "Maracas"s,
            "Short Whistle"s,
            "Long Whistle"s,
            "Short Guiro"s,
            "Long Guiro"s,
            "Claves"s,
            "Hi Wood Block"s,
            "Low Wood Block"s,
            "Mute Cuica"s,
            "Open Cuica"s,
            "Mute Triangle"s,
            "Open Triangle"s,
        };

        return names;
    }
}
