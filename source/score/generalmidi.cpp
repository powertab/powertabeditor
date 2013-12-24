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

    std::vector<std::string> getPresetNames()
    {
        std::vector<std::string> names;

        names.emplace_back("Acoustic Grand Piano");
        names.emplace_back("Bright Acoustic Piano");
        names.emplace_back("Electric Grand Piano");
        names.emplace_back("Honky-tonk Piano");
        names.emplace_back("Electric Piano 1");
        names.emplace_back("Electric Piano 2");
        names.emplace_back("Harpsichord");
        names.emplace_back("Clavinet");
        names.emplace_back("Celesta");
        names.emplace_back("Glockenspiel");
        names.emplace_back("Music Box");
        names.emplace_back("Vibraphone");
        names.emplace_back("Marimba");
        names.emplace_back("Xylophone");
        names.emplace_back("Tubular Bells");
        names.emplace_back("Dulcimer");
        names.emplace_back("Drawbar Organ");
        names.emplace_back("Percussive Organ");
        names.emplace_back("Rock Organ");
        names.emplace_back("Church Organ");
        names.emplace_back("Reed Organ");
        names.emplace_back("Accordion");
        names.emplace_back("Harmonica");
        names.emplace_back("Tango Accordion");
        names.emplace_back("Acoustic Guitar (nylon)");
        names.emplace_back("Acoustic Guitar (steel)");
        names.emplace_back("Electric Guitar (jazz)");
        names.emplace_back("Electric Guitar (clean)");
        names.emplace_back("Electric Guitar (muted)");
        names.emplace_back("Overdriven Guitar");
        names.emplace_back("Distortion Guitar");
        names.emplace_back("Guitar harmonics");
        names.emplace_back("Acoustic Bass");
        names.emplace_back("Electric Bass (finger)");
        names.emplace_back("Electric Bass (pick)");
        names.emplace_back("Fretless Bass");
        names.emplace_back("Slap Bass 1");
        names.emplace_back("Slap Bass 2");
        names.emplace_back("Synth Bass 1");
        names.emplace_back("Synth Bass 2");
        names.emplace_back("Violin");
        names.emplace_back("Viola");
        names.emplace_back("Cello");
        names.emplace_back("Contrabass");
        names.emplace_back("Tremolo Strings");
        names.emplace_back("Pizzicato Strings");
        names.emplace_back("Orchestral Harp");
        names.emplace_back("Timpani");
        names.emplace_back("String Ensemble 1");
        names.emplace_back("String Ensemble 2");
        names.emplace_back("Synth Strings 1");
        names.emplace_back("Synth Strings 2");
        names.emplace_back("Choir Aahs");
        names.emplace_back("Voice Oohs");
        names.emplace_back("Synth Voice");
        names.emplace_back("Orchestra Hit");
        names.emplace_back("Trumpet");
        names.emplace_back("Trombone");
        names.emplace_back("Tuba");
        names.emplace_back("Muted Trumpet");
        names.emplace_back("French Horn");
        names.emplace_back("Brass Section");
        names.emplace_back("Synth Brass 1");
        names.emplace_back("Synth Brass 2");
        names.emplace_back("Soprano Sax");
        names.emplace_back("Alto Sax");
        names.emplace_back("Tenor Sax");
        names.emplace_back("Baritone Sax");
        names.emplace_back("Oboe");
        names.emplace_back("English Horn");
        names.emplace_back("Bassoon");
        names.emplace_back("Clarinet");
        names.emplace_back("Piccolo");
        names.emplace_back("Flute");
        names.emplace_back("Recorder");
        names.emplace_back("Pan Flute");
        names.emplace_back("Blown Bottle");
        names.emplace_back("Shakuhachi");
        names.emplace_back("Whistle");
        names.emplace_back("Ocarina");
        names.emplace_back("Lead 1 (square)");
        names.emplace_back("Lead 2 (sawtooth)");
        names.emplace_back("Lead 3 (calliope)");
        names.emplace_back("Lead 4 (chiff)");
        names.emplace_back("Lead 5 (charang)");
        names.emplace_back("Lead 6 (voice)");
        names.emplace_back("Lead 7 (fifths)");
        names.emplace_back("Lead 8 (bass + lead)");
        names.emplace_back("Pad 1 (new age)");
        names.emplace_back("Pad 2 (warm)");
        names.emplace_back("Pad 3 (polysynth)");
        names.emplace_back("Pad 4 (choir)");
        names.emplace_back("Pad 5 (bowed)");
        names.emplace_back("Pad 6 (metallic)");
        names.emplace_back("Pad 7 (halo)");
        names.emplace_back("Pad 8 (sweep)");
        names.emplace_back("FX 1 (rain)");
        names.emplace_back("FX 2 (soundtrack)");
        names.emplace_back("FX 3 (crystal)");
        names.emplace_back("FX 4 (atmosphere)");
        names.emplace_back("FX 5 (brightness)");
        names.emplace_back("FX 6 (goblins)");
        names.emplace_back("FX 7 (echoes)");
        names.emplace_back("FX 8 (sci-fi)");
        names.emplace_back("Sitar");
        names.emplace_back("Banjo");
        names.emplace_back("Shamisen");
        names.emplace_back("Koto");
        names.emplace_back("Kalimba");
        names.emplace_back("Bag pipe");
        names.emplace_back("Fiddle");
        names.emplace_back("Shanai");
        names.emplace_back("Tinkle Bell");
        names.emplace_back("Agogo");
        names.emplace_back("Steel Drums");
        names.emplace_back("Woodblock");
        names.emplace_back("Taiko Drum");
        names.emplace_back("Melodic Tom");
        names.emplace_back("Synth Drum");
        names.emplace_back("Reverse Cymbal");
        names.emplace_back("Guitar Fret Noise");
        names.emplace_back("Breath Noise");
        names.emplace_back("Seashore");
        names.emplace_back("Bird Tweet");
        names.emplace_back("Telephone Ring");
        names.emplace_back("Helicopter");
        names.emplace_back("Applause");
        names.emplace_back("Gunshot");

        return names;
    }

    std::vector<std::string> getPercussionPresetNames()
    {
        std::vector<std::string> names;

        names.emplace_back("Acoustic Bass Drum");
        names.emplace_back("Bass Drum 1");
        names.emplace_back("Side Stick");
        names.emplace_back("Acoustic Snare");
        names.emplace_back("Hand Clap");
        names.emplace_back("Electric Snare");
        names.emplace_back("Low Floor Tom");
        names.emplace_back("Closed Hi-Hat");
        names.emplace_back("High Floor Tom");
        names.emplace_back("Pedal Hi-Hat");
        names.emplace_back("Low Tom");
        names.emplace_back("Open Hi-Hat");
        names.emplace_back("Low-Mid Tom");
        names.emplace_back("Hi-Mid Tom");
        names.emplace_back("Crash Cymbal 1");
        names.emplace_back("High Tom");
        names.emplace_back("Ride Cymbal 1");
        names.emplace_back("Chinese Cymbal");
        names.emplace_back("Ride Bell");
        names.emplace_back("Tambourine");
        names.emplace_back("Splash Cymbal");
        names.emplace_back("Cowbell");
        names.emplace_back("Crash Cymbal 2");
        names.emplace_back("Vibraslap");
        names.emplace_back("Ride Cymbal 2");
        names.emplace_back("Hi Bongo");
        names.emplace_back("Low Bongo");
        names.emplace_back("Mute Hi Conga");
        names.emplace_back("Open Hi Conga");
        names.emplace_back("Low Conga");
        names.emplace_back("High Timbale");
        names.emplace_back("Low Timbale");
        names.emplace_back("High Agogo");
        names.emplace_back("Low Agogo");
        names.emplace_back("Cabasa");
        names.emplace_back("Maracas");
        names.emplace_back("Short Whistle");
        names.emplace_back("Long Whistle");
        names.emplace_back("Short Guiro");
        names.emplace_back("Long Guiro");
        names.emplace_back("Claves");
        names.emplace_back("Hi Wood Block");
        names.emplace_back("Low Wood Block");
        names.emplace_back("Mute Cuica");
        names.emplace_back("Open Cuica");
        names.emplace_back("Mute Triangle");
        names.emplace_back("Open Triangle");

        return names;
    }
}
