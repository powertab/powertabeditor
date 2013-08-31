/////////////////////////////////////////////////////////////////////////////
// Name:            keysignature.cpp
// Purpose:         Stores and renders a key signature
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 10, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "keysignature.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

using std::string;

// Default constants
const uint8_t       KeySignature::DEFAULT_DATA              = 0;
const uint8_t       KeySignature::ACCIDENTAL_WIDTH          = 6;

/// Default Constructor
KeySignature::KeySignature() :
    m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Dec 10, 2004
}

/// Primary Constructor
/// @param keyType major or minor (see keyTypes enum in .h for values)
/// @param keyAccidentals type and number of accidentals (4 sharps, 2 flats,
/// etc.; see keyAccidentals enum in .h for values)
KeySignature::KeySignature(uint8_t keyType, uint8_t keyAccidentals) :
    m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Dec 10, 2004
    SetKey(keyType, keyAccidentals);
}

/// Copy Constructor
KeySignature::KeySignature(const KeySignature& keySignature) :
    PowerTabObject(), m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Dec 10, 2004
    *this = keySignature;
}

/// Destructor
KeySignature::~KeySignature()
{
    //------Last Checked------//
    // - Dec 10, 2004
}

/// Assignment Operator
const KeySignature& KeySignature::operator=(const KeySignature& keySignature)
{
    //------Last Checked------//
    // - Dec 10, 2004

    // Check for assignment to self
    if (this != &keySignature)
        m_data = keySignature.m_data;
    return (*this);
}

/// Equality operator
bool KeySignature::operator==(const KeySignature& keySignature) const
{
    //------Last Checked------//
    // - Dec 10, 2004
    return (m_data == keySignature.m_data);
}

/// Inequality operator
bool KeySignature::operator!=(const KeySignature& keySignature) const
{
    //------Last Checked------//
    // - Dec 10, 2004
    return (!operator==(keySignature));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool KeySignature::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_data;

    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool KeySignature::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream >> m_data;
    return true;
}

// Key Functions
/// Sets the key type and accidentals
/// @param keyType major or minor (see keyTypes enum in .h for values)
/// @param keyAccidentals type and number of accidentals (4 sharps, 2 flats,
/// etc.; see keyAccidentals enum in .h for values)
bool KeySignature::SetKey(uint8_t keyType, uint8_t keyAccidentals)
{
    //------Last Checked------//
    // - Dec 11, 2004
    if (!SetKeyType(keyType))
        return (false);
    if (!SetKeyAccidentals(keyAccidentals))
        return (false);
    return (true);
}

/// Gets the key type and accidentals
/// @param keyType major or minor (see keyTypes enum in .h for values)
/// @param keyAccidentals type and number of accidentals (4 sharps, 2 flats,
/// etc.; see keyAccidentals enum in .h for values)
void KeySignature::GetKey(uint8_t& keyType, uint8_t& keyAccidentals) const
{
    //------Last Checked------//
    // - Dec 11, 2004
    keyType = GetKeyType();
    keyAccidentals = GetKeyAccidentals();

    // Cancellations store the key of the cancelled type, but we know that the
    // cancellations will always
    // occur on C Major/A Minor so set the key to 0
    if (IsCancellation())
        keyAccidentals = 0;
}

/// Gets the key type and accidentals required to draw the key signature
/// @param keyType major or minor (see keyTypes enum in .h for values)
/// @param keyAccidentals Type and number of accidentals (4 sharps, 2 flats,
/// etc.; see keyAccidentals enum in .h for values)
/// @return True if the key is a cancellation, false if not
bool KeySignature::GetDrawKey(uint8_t& keyType, uint8_t& keyAccidentals) const
{
    //------Last Checked------//
    // - Dec 11, 2004
    keyType = GetKeyType();
    keyAccidentals = (uint8_t)(m_data & keyAccidentalsMask);
    return (IsCancellation());
}

// Key Type Functions
/// Sets the key type
/// @param keyType Key type to set (see keyType enum in .h for values)
/// @return True if the key type was set, false if not
bool KeySignature::SetKeyType(uint8_t keyType)
{
    //------Last Checked------//
    // - Dec 11, 2004
    PTB_CHECK_THAT(IsValidKeyType(keyType), false);
    m_data &= ~keyTypeMask;
    m_data |= (keyType << 6);
    return (true);
}

// Key Accidentals Functions
/// Sets the type and number of accidentals in the key signature (4 sharps,
/// 2 flats, etc.)
/// @param keyAccidentals Type and number of accidentals to set
/// @return True if the key accidentals was set, false if not
bool KeySignature::SetKeyAccidentals(uint8_t keyAccidentals)
{
    //------Last Checked------//
    // - Dec 11, 2004
    PTB_CHECK_THAT(IsValidKeyAccidentals(keyAccidentals), false);
    m_data &= ~keyAccidentalsMask;
    m_data |= keyAccidentals;
    return (true);
}

/// Gets the type and number of accidentals in the key signature (4 sharps,
/// 2 flats, etc.)
/// @return Type and number of accidentals in the key signature (see
/// keyAccidentals enum in .h for values)
uint8_t KeySignature::GetKeyAccidentals() const
{
    //------Last Checked------//
    // - Dec 10, 2004
    uint8_t returnValue = (uint8_t)(m_data & keyAccidentalsMask);

    // Cancellations store the key of the cancelled type, but we know that the
    // cancellations will always
    // occur on C Major/A Minor so set the key to 0
    if (IsCancellation())
        returnValue = 0;
    return (returnValue);
}

/// Gets the number of accidentals (including cancelled) in the key signature (4 sharps,
/// 2 flats, etc.)
/// @return Number of accidentals to be displayed in the key signature
uint8_t KeySignature::GetKeyAccidentalsIncludingCancel() const
{
    // Get the draw key
    uint8_t keyType = 0;
    uint8_t keyAccidentals = 0;
    GetDrawKey(keyType, keyAccidentals);

    // Determine the number of accidentals, be it sharps or flats
    uint8_t numAccidentals = ((keyAccidentals <= KeySignature::sevenSharps) ? keyAccidentals :
        (keyAccidentals - 7));
    return numAccidentals;
}


/// Gets the width of the key signature, in drawing units (100ths of an inch)
/// @return The width of the key signature
int KeySignature::GetWidth() const
{
    //------Last Checked------//
    // - Aug 30, 2007
    int returnValue = 0;

    // Key signature must be shown to have width
    if (IsShown())
    {
        // Determine the number of accidentals, be it sharps or flats
        int accidentals = GetKeyAccidentalsIncludingCancel();

        // There are 6 drawing units per accidental
        returnValue = accidentals * ACCIDENTAL_WIDTH;
    }

    return (returnValue);
}

}
