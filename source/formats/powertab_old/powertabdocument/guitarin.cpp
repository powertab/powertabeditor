/////////////////////////////////////////////////////////////////////////////
// Name:            guitarin.cpp
// Purpose:         Stores and renders guitar in symbols
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 16, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "guitarin.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default constants
const uint16_t        GuitarIn::DEFAULT_SYSTEM                = 0;
const uint8_t        GuitarIn::DEFAULT_STAFF                 = 0;
const uint8_t        GuitarIn::DEFAULT_POSITION              = 0;
const uint16_t        GuitarIn::DEFAULT_DATA                  = 0;
	
// System Constants
const uint32_t      GuitarIn::MIN_SYSTEM                    = 0;
const uint32_t      GuitarIn::MAX_SYSTEM                    = 65535;

// Staff Constants
const uint32_t      GuitarIn::MIN_STAFF                     = 0;
const uint32_t      GuitarIn::MAX_STAFF                     = 2;

// Position Constants
const uint32_t      GuitarIn::MIN_POSITION                  = 0;
const uint32_t      GuitarIn::MAX_POSITION                  = 255;

/// Default Constructor
GuitarIn::GuitarIn() :
	m_system(DEFAULT_SYSTEM), m_staff(DEFAULT_STAFF),
	m_position(DEFAULT_POSITION), m_data(DEFAULT_DATA)
{
	//------Last Checked------//
	// - Jan 25, 2005
}

/// Primary Constructor
/// @param system Zero-based index of the system where the guitar in is anchored
/// @param staff Zero-based inded of the staff within the system where the
/// guitar in is anchored
/// @param position Zero-based index of the position within the system where the
/// guitar in is anchored
/// @param staffGuitars A bit map representing the guitars to set on the staff
/// (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
/// @param rhythmSlashGuitars A bit map representing the guitars to set for the
/// rhythm slashes (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
GuitarIn::GuitarIn(uint32_t system, uint32_t staff, uint32_t position,
	uint8_t staffGuitars, uint8_t rhythmSlashGuitars) :
	m_system(DEFAULT_SYSTEM), m_staff(DEFAULT_STAFF),
	m_position(DEFAULT_POSITION), m_data(DEFAULT_DATA)
{
	//------Last Checked------//
	// - Jan 25, 2005
	assert(IsValidSystem(system));
	assert(IsValidStaff(staff));
	assert(IsValidPosition(position));
	SetSystem(system);
	SetStaff(staff);
	SetPosition(position);
	SetStaffGuitars(staffGuitars);
	SetRhythmSlashGuitars(rhythmSlashGuitars);
}

/// Copy Constructor
GuitarIn::GuitarIn(const GuitarIn& guitarIn) :
	PowerTabObject(), m_system(DEFAULT_SYSTEM), m_staff(DEFAULT_STAFF),
	m_position(DEFAULT_POSITION), m_data(DEFAULT_DATA)
{
	//------Last Checked------//
	// - Dec 16, 2004
	*this = guitarIn;
}

/// Destructor
GuitarIn::~GuitarIn()
{
	//------Last Checked------//
	// - Dec 16, 2004
}

/// Assignment Operator
const GuitarIn& GuitarIn::operator=(const GuitarIn& guitarIn)
{
	//------Last Checked------//
	// - Jan 25, 2005
	
	// Check for assignment to self
	if (this != &guitarIn)
	{
		m_system = guitarIn.m_system;
		m_staff = guitarIn.m_staff;
		m_position = guitarIn.m_position;
		m_data = guitarIn.m_data;
	}
	return (*this);
}

/// Equality Operator
bool GuitarIn::operator==(const GuitarIn& guitarIn) const
{
	//------Last Checked------//
	// - Jan 25, 2005
	return (
		(m_system == guitarIn.m_system) &&
		(m_staff == guitarIn.m_staff) &&
		(m_position == guitarIn.m_position) &&
		(m_data == guitarIn.m_data)
	);    
}

/// Inequality Operator
bool GuitarIn::operator!=(const GuitarIn& guitarIn) const
{
	//------Last Checked------//
	// - Jan 5, 2005
	return (!operator==(guitarIn));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool GuitarIn::Serialize(PowerTabOutputStream& stream) const
{
	//------Last Checked------//
	// - Jan 25, 2005
	stream << m_system << m_staff << m_position << m_data;
	PTB_CHECK_THAT(stream.CheckState(), false);
	
	return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool GuitarIn::Deserialize(PowerTabInputStream& stream, uint16_t)
{
	stream >> m_system >> m_staff >> m_position >> m_data;
    return true;
}

// Guitars Functions
/// Sets the guitars
/// @param rhythmSlashes True to set the rhythm slash guitars, false to set the
/// staff guitars
/// @param guitars A bit map representing the guitars to set (bit 1 = guitar 0,
/// bit 2 = guitar 1, etc.)
/// @return True if the guitars was set, false if not
bool GuitarIn::SetGuitars(bool rhythmSlashes, uint8_t guitars)
{
	//------Last Checked------//
	// - Jan 25, 2005   
	if (rhythmSlashes)
		m_data = MAKEWORD(guitars, GetStaffGuitars());
	else
		m_data = MAKEWORD(GetRhythmSlashGuitars(), guitars);
		
	return (true);
}

/// Gets the guitars
/// @param rhythmSlashes True to get the rhythm slash guitars, false to get the
/// staff guitars
/// @return A bit map representing the guitars (bit map where bit 1 = guitar 0,
/// bit 2 = guitar 1, etc.)
uint8_t GuitarIn::GetGuitars(bool rhythmSlashes) const
{
	//------Last Checked------//
	// - Jan 25, 2005
	if (rhythmSlashes)
		return (LOBYTE(m_data));
	return (HIBYTE(m_data));
}

/// Determines if the guitars is set
/// @param rhythmSlashes True to test if the rhythm slash guitars are set, false
/// to test if the staff guitars are set
/// @return True if the guitars is set, false if not
bool GuitarIn::HasGuitarsSet(bool rhythmSlashes) const
{
	//------Last Checked------//
	// - Jan 25, 2005
	uint8_t guitars = ((rhythmSlashes) ? GetRhythmSlashGuitars() :
		GetStaffGuitars());
	return (guitars != 0);
}

// Operations


/// Returns a text representation of the guitar in (i.e. Gtr I, II)
/// @param rhythmSlashes If true, gets the rhythm slash guitars text, otherwise
/// the staff guitars text
/// @return A text representation of the guitars
std::string GuitarIn::GetText(bool rhythmSlashes) const
{
	//------Last Checked------//
	// - Jan 25, 2005
	uint8_t guitars = GetGuitars(rhythmSlashes);

	if (guitars == 0)
		return "";

        std::string returnValue;
        std::string romanNumerals;
	
	// Loop through, add guitars as we go - there are only 7 guitars    
	uint8_t mask = 1;
	uint32_t guitarCount = 0;
	uint32_t guitarNumber = 1;
	for (;; mask *= 2, guitarNumber++)
	{
		// Guitar is set
		if ((guitars & mask) == mask)
		{
			// Add a comma if we already have a numeral
			if (!romanNumerals.empty())
				romanNumerals += ", ";
			
			// Add the roman numeral
			romanNumerals += ArabicToRoman(guitarNumber, true);
			guitarCount++;
		}
		
		// block overflow
		if (mask == 64)
			break;
	}

	// Add the symbol Gtr before
	if (guitarCount == 1)
		returnValue = "Gtr " + romanNumerals;
	else
		returnValue = "Gtrs " + romanNumerals;

	return (returnValue);
}

}
