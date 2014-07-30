/////////////////////////////////////////////////////////////////////////////
// Name:            barline.cpp
// Purpose:         Stores and renders barlines
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 4, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "barline.h"

#include "powertabfileheader.h"         // Needed for FILE_VERSION constants
#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default Constants
const uint8_t Barline::DEFAULT_POSITION      = 0;
const uint8_t Barline::DEFAULT_DATA          = (uint8_t)(bar << 5);

// Repeat Count Constants
const uint8_t Barline::MIN_REPEAT_COUNT      = 2;
const uint8_t Barline::MAX_REPEAT_COUNT      = 31;
	
// Constructor/Destructor
/// Default Constructor
Barline::Barline() : 
	m_position(DEFAULT_POSITION), m_data(DEFAULT_DATA)
{
	//------Last Checked------//
	// - Jan 4, 2005
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Barline::Serialize(PowerTabOutputStream& stream) const
{
	//------Last Checked------//
	// - Jan 4, 2005
	stream << m_position << m_data;
	PTB_CHECK_THAT(stream.CheckState(), false);
	
	m_keySignature.Serialize(stream);
	PTB_CHECK_THAT(stream.CheckState(), false);
	
	m_timeSignature.Serialize(stream);
	PTB_CHECK_THAT(stream.CheckState(), false);
	
	m_rehearsalSign.Serialize(stream);
	PTB_CHECK_THAT(stream.CheckState(), false);

	return (true);
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Barline::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
	//------Last Checked------//
	// - Jan 4, 2005
	
	// Version 1.0/1.0.2 (key was stored in word)
    if (version == PowerTabFileHeader::Version_1_0 ||
        version == PowerTabFileHeader::Version_1_0_2)
	{
		uint16_t symbol;
		stream >> m_position >> symbol;

		uint8_t temp = HIBYTE(symbol);
		uint8_t keyType = (uint8_t)((temp >> 4) & 0xf);
		uint8_t keyAccidentals = (uint8_t)(temp & 0xf);

		// Key signature was set
		if (keyType > 0)
		{
			// Set the key to be shown
			m_keySignature.Show();

			// Cancellation
			if (keyType > 2)
				m_keySignature.SetCancellation();

			keyType = (uint8_t)(((keyType % 2) == 1) ? KeySignature::majorKey :
				KeySignature::minorKey);

			m_keySignature.SetKey(keyType, keyAccidentals);
		}
		
		// Update the bar data (stored in low byte of symbol)
		m_data = LOBYTE(symbol);
	}
	// CASE: Version 1.5 and up
	else
	{        
		stream >> m_position >> m_data;
		
		m_keySignature.Deserialize(stream, version);
		
		m_timeSignature.Deserialize(stream, version);
		
		m_rehearsalSign.Deserialize(stream, version);
	}

    return true;
}

// Barline Data Functions
/// Sets the barline data
/// @param type The type to set
/// @param repeatCount The repeat count to set
/// @return True if the data was set, false if not
bool Barline::SetBarlineData(uint8_t type, uint8_t repeatCount)
{
	//------Last Checked------//
	// - Jan 4, 2005
	if (!SetType(type))
		return (false);
	if (!SetRepeatCount(repeatCount))
		return (false);
	return (true);
}
// Type Functions
/// Sets the type of bar
/// @param type Type of bar to set
/// @return True if the bar type was set, false if not
bool Barline::SetType(uint8_t type)
{
	//------Last Checked------//
	// - Jan 4, 2005
	PTB_CHECK_THAT(IsValidType(type), false);
	
	m_data &= ~typeMask;
	m_data |= (uint8_t)(type << 5);
	
	return (true);
}

// Repeat Count Functions
/// Sets the repeat count for a repeat ending bar
/// @param repeatCount Repeat count to set
/// @return True if the repeat count was set, false if not
bool Barline::SetRepeatCount(uint32_t repeatCount)
{
	//------Last Checked------//
	// - Jan 4, 2005
	PTB_CHECK_THAT(IsValidRepeatCount(repeatCount), false);
	
	m_data &= ~repeatCountMask;
	m_data |= (uint8_t)repeatCount;
	
    return (true);
}

KeySignature &Barline::GetKeySignature()
{
    return m_keySignature;
}

/// Gets the key signature
const KeySignature& Barline::GetKeySignature() const
{
    return m_keySignature;
}

/// Gets the time signature
const TimeSignature& Barline::GetTimeSignature() const
{
    return m_timeSignature;
}

/// Gets the rehearsal sign
const RehearsalSign& Barline::GetRehearsalSign() const
{
    return m_rehearsalSign;
}

/// Gets the width of the key and time signature on the barline
/// @return The width of the key and time signature on the barline
int Barline::GetKeyAndTimeSignatureWidth() const
{
    int returnValue = 0;

    // Add the width of the key signature
    returnValue += m_keySignature.GetWidth();

    // If the key signature has width, we need to adjust to account the right
    // side of the barline
    if (returnValue > 0)
    {
        // Some bars are thicker than others
        if (IsDoubleBar())
            returnValue += 2;
        else if (IsRepeatStart())
            returnValue += 5;
        else if (GetType() >= repeatEnd)
            returnValue += 6;
    }

    // Add the width of the time signature
    int timeSignatureWidth = m_timeSignature.GetWidth();
    if (timeSignatureWidth > 0)
    {
        // 3 units of space from barline or key signature
        returnValue += (3 + timeSignatureWidth);
    }

    return (returnValue);
}
}
