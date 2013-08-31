/////////////////////////////////////////////////////////////////////////////
// Name:            guitar.cpp
// Purpose:         Stores information about a guitar (tuning, effects, etc.)
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 8, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

#include "guitar.h"
#include "generalmidi.h"

namespace PowerTabDocument {

// Note: All MIDI constants and functions are defined in generalmidi.h
// Default constants
const uint8_t         Guitar::DEFAULT_NUMBER                 = 0;
const std::string     Guitar::DEFAULT_DESCRIPTION            = "Untitled";
const uint8_t         Guitar::DEFAULT_PRESET                 = midi::MIDI_PRESET_ACOUSTIC_GUITAR_STEEL;
const uint8_t         Guitar::DEFAULT_INITIAL_VOLUME         = 104;
const uint8_t         Guitar::DEFAULT_PAN                    = PAN_CENTER;
const uint8_t         Guitar::DEFAULT_REVERB                 = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::DEFAULT_CHORUS                 = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::DEFAULT_TREMOLO                = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::DEFAULT_PHASER                 = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::DEFAULT_CAPO                   = 0;

const uint8_t         Guitar::MIN_PRESET                     = midi::FIRST_MIDI_PRESET;
const uint8_t         Guitar::MAX_PRESET                     = midi::LAST_MIDI_PRESET;

const uint8_t         Guitar::MIN_INITIAL_VOLUME             = midi::MIN_MIDI_CHANNEL_VOLUME;
const uint8_t         Guitar::MAX_INITIAL_VOLUME             = midi::MAX_MIDI_CHANNEL_VOLUME;

const uint8_t         Guitar::MIN_PAN                        = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::MAX_PAN                        = midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::PAN_HARD_LEFT                  = MIN_PAN;
const uint8_t         Guitar::PAN_CENTER                     = 64;
const uint8_t         Guitar::PAN_HARD_RIGHT                 = MAX_PAN;

const uint8_t         Guitar::MIN_REVERB                     = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::MAX_REVERB                     = midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL;

const uint8_t         Guitar::MIN_CHORUS                     = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::MAX_CHORUS                     = midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL;

const uint8_t         Guitar::MIN_TREMOLO                    = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::MAX_TREMOLO                    = midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL;

const uint8_t         Guitar::MIN_PHASER                     = midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t         Guitar::MAX_PHASER                     = midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL;

const uint8_t         Guitar::MIN_CAPO                       = 0;
const uint8_t         Guitar::MAX_CAPO                       = 12;

Guitar::Guitar() :
    m_number(DEFAULT_NUMBER), m_description(DEFAULT_DESCRIPTION),
    m_preset(DEFAULT_PRESET), m_initialVolume(DEFAULT_INITIAL_VOLUME),
    m_pan(DEFAULT_PAN), m_reverb(DEFAULT_REVERB), m_chorus(DEFAULT_CHORUS),
    m_tremolo(DEFAULT_TREMOLO), m_phaser(DEFAULT_PHASER), m_capo(DEFAULT_CAPO),
    m_isShown(true)
{
}

/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Guitar::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_number;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteMFCString(m_description);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream << m_preset << m_initialVolume << m_pan << m_reverb << m_chorus <<
              m_tremolo << m_phaser << m_capo;
    PTB_CHECK_THAT(stream.CheckState(), false);

    m_tuning.Serialize(stream);
    PTB_CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Guitar::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    stream >> m_number;

    stream.ReadMFCString(m_description);

    stream >> m_preset >> m_initialVolume >> m_pan >> m_reverb >> m_chorus >>
              m_tremolo >> m_phaser >> m_capo;

    m_tuning.Deserialize(stream, version);

    return true;
}

/// Gets the tuning used by the guitar
Tuning& Guitar::GetTuning()
{
    return m_tuning;
}

/// Gets the tuning used by the guitar
const Tuning& Guitar::GetTuning() const
{
    return m_tuning;
}

/// Gets the number of strings on the guitar (determined by the tuning)
size_t Guitar::GetStringCount() const
{
    return m_tuning.GetStringCount();
}

/// Sets the number (id) used by the guitar
/// @param number Number to set (zero based)
void Guitar::SetNumber(uint8_t number)
{
    m_number = number;
}

/// Gets the number (id) used by the guitar
uint8_t Guitar::GetNumber() const
{
    return m_number;
}

/// Sets the description for the guitar
void Guitar::SetDescription(const std::string& description)
{
    m_description = description;
}

/// Gets the description for the guitar (i.e. Acoustic w/Chorus)
std::string Guitar::GetDescription() const
{
    return m_description;
}

/// Determines if a preset is valid
/// @return True if the preset is valid, false if not
bool Guitar::IsValidPreset(uint8_t preset)
{
    return /*(preset >= MIN_PRESET) && */(preset <= MAX_PRESET);
}

/// Sets the preset (MIDI) used by the guitar
/// @param preset Preset to set
/// @return True if the preset was set, false if not
bool Guitar::SetPreset(uint8_t preset)
{
    PTB_CHECK_THAT(IsValidPreset(preset), false);
    m_preset = preset;
    return true;
}

/// Gets the preset (MIDI) used by the guitar
uint8_t Guitar::GetPreset() const
{
    return m_preset;
}

/// Determines if an initial volume is valid
/// @return True if the initial volume is valid, false if not
bool Guitar::IsValidInitialVolume(uint8_t initialVolume)
{
    return /*(initialVolume >= MIN_INITIAL_VOLUME) &&*/ (initialVolume <= MAX_INITIAL_VOLUME);
}

/// Sets the initial volume level of the guitar
/// @return True if the volume was set, false if not
bool Guitar::SetInitialVolume(uint8_t initialVolume)
{
    PTB_CHECK_THAT(IsValidInitialVolume(initialVolume), false);
    m_initialVolume = initialVolume;
    return true;
}

/// Gets the initial volume level of the guitar
uint8_t Guitar::GetInitialVolume() const
{
    return m_initialVolume;
}

/// Determines if a pan level is valid
/// @return True if the pan level is valid, false if not
bool Guitar::IsValidPan(uint8_t pan)
{
    return /*(pan >= MIN_PAN) && */(pan <= MAX_PAN);
}

/// Sets the pan level for the guitar
/// @param pan Pan level to set (0 - hard left, 64 - center, 127 - hard
/// right)
/// @return True if the pan level was set, false if not
bool Guitar::SetPan(uint8_t pan)
{
    PTB_CHECK_THAT(IsValidPan(pan), false);
    m_pan = pan;
    return true;
}

/// Gets the pan level for the guitar
uint8_t Guitar::GetPan() const
{
    return m_pan;
}

/// Determines if a reverb level is valid
/// @return True if the reverb level is valid, false if not
bool Guitar::IsValidReverb(uint8_t reverb)
{
    return /*(reverb >= MIN_REVERB) && */(reverb <= MAX_REVERB);
}

/// Sets the reverb level for the guitar
/// @param reverb Reverb level to set (0 - none, 127 - max)
/// @return True if the reverb level was set, false if not
bool Guitar::SetReverb(uint8_t reverb)
{
    PTB_CHECK_THAT(IsValidReverb(reverb), false);
    m_reverb = reverb;
    return true;
}

/// Gets the reverb level for the guitar
uint8_t Guitar::GetReverb() const
{
    return m_reverb;
}

/// Determines if a chorus level is valid
/// @return True if the chorus level is valid, false if not
bool Guitar::IsValidChorus(uint8_t chorus)
{
    return /*(chorus >= MIN_CHORUS) && */(chorus <= MAX_CHORUS);
}

/// Sets the chorus level for the guitar (0 - none, 127 - maximum chorus)
/// @return True if the chorus level was set, false if not
bool Guitar::SetChorus(uint8_t chorus)
{
    PTB_CHECK_THAT(IsValidChorus(chorus), false);
    m_chorus = chorus;
    return true;
}

/// Gets the chorus level for the guitar
uint8_t Guitar::GetChorus() const
{
    return m_chorus;
}

/// Determines if a tremolo level is valid
/// @return True if the tremolo level is valid, false if not
bool Guitar::IsValidTremolo(uint8_t tremolo)
{
    return /*(tremolo >= MIN_TREMOLO) && */(tremolo <= MAX_TREMOLO);
}

/// Sets the tremolo level for the guitar (0 - none, 127 - maximum tremolo)
/// @return True if the tremolo level was set, false is not
bool Guitar::SetTremolo(uint8_t tremolo)
{
    PTB_CHECK_THAT(IsValidTremolo(tremolo), false);
    m_tremolo = tremolo;
    return true;
}

/// Gets the tremolo level for the guitar
uint8_t Guitar::GetTremolo() const
{
    return m_tremolo;
}

/// Determines if a phaser level is valid
/// @return True if the phaser level is valid, false if not
bool Guitar::IsValidPhaser(uint8_t phaser)
{
    return /*(phaser >= MIN_PHASER) && */(phaser <= MAX_PHASER);
}

/// Sets the phaser level for the guitar (0 - none, 127 - maximum phaser)
/// @return True if the phaser level was set, false is not
bool Guitar::SetPhaser(uint8_t phaser)
{
    PTB_CHECK_THAT(IsValidPhaser(phaser), false);
    m_phaser = phaser;
    return true;
}

/// Gets the phaser level for the guitar
uint8_t Guitar::GetPhaser() const
{
    return m_phaser;
}

/// Determines if a capo value is valid
/// @return True if the capo value is valid, false if not
bool Guitar::IsValidCapo(uint8_t capo)
{
    return /*(capo >= MIN_CAPO) && */(capo <= MAX_CAPO);
}

/// Sets the capo used by the guitar (0 - no capo, all other values
/// represent the fret where the capo is placed)
/// @return True if the capo was set, false if not
bool Guitar::SetCapo(uint8_t capo)
{
    PTB_CHECK_THAT(IsValidCapo(capo), false);
    m_capo = capo;
    return true;
}

/// Gets the capo setting used by the guitar
uint8_t Guitar::GetCapo() const
{
    return m_capo;
}

/// Determines if the guitar uses a capo
/// @return True if the guitar uses a capo, false if not
bool Guitar::UsesCapo() const
{
    return GetCapo() != MIN_CAPO;
}

bool Guitar::IsShown() const
{
    return m_isShown;
}

void Guitar::SetShown(bool set)
{
    m_isShown = set;
}

}
