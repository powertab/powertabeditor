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

// Note: All MIDI constants and functions are defined in generalmidi.h
// Default constants
const uint8_t         Guitar::DEFAULT_NUMBER                 = 0;
const char*        Guitar::DEFAULT_DESCRIPTION            = "Untitled";
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

// Constructors/Destructors
/// Default Constructor
Guitar::Guitar() :
    m_number(DEFAULT_NUMBER), m_description(DEFAULT_DESCRIPTION),
    m_preset(DEFAULT_PRESET), m_initialVolume(DEFAULT_INITIAL_VOLUME),
    m_pan(DEFAULT_PAN), m_reverb(DEFAULT_REVERB), m_chorus(DEFAULT_CHORUS),
    m_tremolo(DEFAULT_TREMOLO), m_phaser(DEFAULT_PHASER), m_capo(DEFAULT_CAPO)
{
    //------Last Checked------//
    // - Dec 8, 2004
}

/// Primary Constructor
/// @param number Unique number identifing the guitar (zero based)
/// @param description Description for the guitar
/// @param preset MIDI preset
/// @param initialVolume The initial volume level of the guitar
/// @param pan MIDI pan level
/// @param reverb MIDI reverb level
/// @param chorus MIDI chorus level
/// @param tremolo MIDI tremolo level
/// @param phaser MIDI phaser level
/// @param capo Capo fret number (0 = no capo)
Guitar::Guitar(uint8_t number, const char* description, uint8_t preset,
    uint8_t initialVolume, uint8_t pan, uint8_t reverb, uint8_t chorus,
    uint8_t tremolo, uint8_t phaser, uint8_t capo) : m_number(number),
    m_description(description), m_preset(preset),
    m_initialVolume(initialVolume), m_pan(pan), m_reverb(reverb),
    m_chorus(chorus), m_tremolo(tremolo), m_phaser(phaser), m_capo(capo)
{
    //------Last Checked------//
    // - Dec 8, 2004
    assert(description != NULL);
    assert(midi::IsValidMidiPreset(preset));
    assert(midi::IsValidMidiChannelVolume(initialVolume));
    assert(IsValidPan(pan));
    assert(IsValidReverb(reverb));
    assert(IsValidChorus(chorus));
    assert(IsValidTremolo(tremolo));
    assert(IsValidPhaser(phaser));
    assert(IsValidCapo(capo));
}

/// Copy Constructor
Guitar::Guitar (const Guitar& guitar) :
    m_number(DEFAULT_NUMBER), m_description(DEFAULT_DESCRIPTION),
    m_preset(DEFAULT_PRESET), m_initialVolume(DEFAULT_INITIAL_VOLUME),
    m_pan(DEFAULT_PAN), m_reverb(DEFAULT_REVERB), m_chorus(DEFAULT_CHORUS),
    m_tremolo(DEFAULT_TREMOLO), m_phaser(DEFAULT_PHASER), m_capo(DEFAULT_CAPO)

{
    //------Last Checked------//
    // - Dec 8, 2004
    *this = guitar;
}

/// Destructor
Guitar::~Guitar()
{
    //------Last Verified------//
    // - Nov 27, 2004
}

// Operators
/// Assignment operator
const Guitar& Guitar::operator=(const Guitar& guitar)
{
    //------Last Checked------//
    // - Dec 9, 2004

    // Check for assignment to self
    if (this != &guitar)
    {
        m_number = guitar.m_number;
        m_description = guitar.m_description;
        m_preset = guitar.m_preset;
        m_initialVolume = guitar.m_initialVolume;
        m_tuning = guitar.m_tuning;
        m_pan = guitar.m_pan;
        m_reverb = guitar.m_reverb;
        m_chorus = guitar.m_chorus;
        m_tremolo = guitar.m_tremolo;
        m_phaser = guitar.m_phaser;
        m_capo = guitar.m_capo;
    }
    return (*this);
}

/// Equality Operator
bool Guitar::operator==(const Guitar& guitar) const
{
    //------Last Checked------//
    // - Dec 8, 2004
    return (
        (m_number == guitar.m_number) &&
        (m_description == guitar.m_description) &&
        (m_preset == guitar.m_preset) &&
        (m_initialVolume == guitar.m_initialVolume) &&
        (m_tuning == guitar.m_tuning) &&
        (m_pan == guitar.m_pan) &&
        (m_reverb == guitar.m_reverb) &&
        (m_chorus == guitar.m_chorus) &&
        (m_tremolo == guitar.m_tremolo) &&
        (m_phaser == guitar.m_phaser) &&
        (m_capo == guitar.m_capo)
    );
}

/// Inequality Operator
bool Guitar::operator!=(const Guitar& guitar) const
{
    //------Last Checked------//
    // - Dec 8, 2004
    return (!operator==(guitar));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Guitar::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Dec 8, 2004
    stream << m_number;
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteMFCString(m_description);
    CHECK_THAT(stream.CheckState(), false);

    stream << m_preset << m_initialVolume << m_pan << m_reverb << m_chorus <<
        m_tremolo << m_phaser << m_capo;
    CHECK_THAT(stream.CheckState(), false);

    m_tuning.Serialize(stream);
    CHECK_THAT(stream.CheckState(), false);

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

// Operations
/// Gets the legend text for the guitar (i.e. Gtr. I - Acoustic (E A D G B E))
std::string Guitar::GetLegendText() const
{
    //------Last Checked------//
    // - Dec 8, 2004
    std::stringstream returnValue;
    returnValue << "Gtr. " << ArabicToRoman(GetNumber() + 1, true);
    returnValue << " - " << GetDescription() << " - " << m_tuning.GetSpelling();
    return returnValue.str();
}
