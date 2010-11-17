/////////////////////////////////////////////////////////////////////////////
// Name:            rehearsalsign.cpp
// Purpose:         Stores and renders Rehearsal Sign symbols
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 10, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "rehearsalsign.h"

// Default constants
const int8_t    RehearsalSign::DEFAULT_LETTER           = RehearsalSign::notSet;
const char*   RehearsalSign::DEFAULT_DESCRIPTION      = "";

/// Default Constructor
RehearsalSign::RehearsalSign() :
	m_letter(DEFAULT_LETTER), m_description(DEFAULT_DESCRIPTION)
{
	//------Last Checked------//
	// - Dec 10, 2004
}

/// Primary Constructor
/// @param letter Unique letter identifying the rehearsal sign
/// @param description Description for the rehearsal sign (i.e. Intro)
RehearsalSign::RehearsalSign(int8_t letter, const char* description) :
	m_letter(letter), m_description(description)
{
	//------Last Checked------//
	// - Dec 10, 2004
	assert(IsValidLetter(letter));
	assert(description != NULL);
}

/// Copy Constructor
RehearsalSign::RehearsalSign(const RehearsalSign& rehearsalSign) :
	m_letter(DEFAULT_LETTER), m_description(DEFAULT_DESCRIPTION)
{
	//------Last Checked------//
	// - Dec 10, 2004
	*this = rehearsalSign;
}

/// Destructor
RehearsalSign::~RehearsalSign()
{
	//------Last Checked------//
	// - Dec 10, 2004
}

/// Assignment Operator
const RehearsalSign& RehearsalSign::operator=(
	const RehearsalSign& rehearsalSign)
{
	//------Last Checked------//
	// - Dec 10, 2004

	// Check for assignment to self
	if (this != &rehearsalSign)
	{
		m_letter = rehearsalSign.m_letter;
		m_description = rehearsalSign.m_description;
	}
	return (*this);
}

/// Equality Operator
bool RehearsalSign::operator==(const RehearsalSign& rehearsalSign) const
{
	//------Last Checked------//
	// - Dec 10, 2004
	return ((m_letter == rehearsalSign.m_letter) &&
		(m_description == rehearsalSign.m_description));
}

/// Inequality Operator
bool RehearsalSign::operator!=(const RehearsalSign& rehearsalSign) const
{
	//------Last Checked------//
	// - Dec 10, 2004
	return (!operator==(rehearsalSign));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool RehearsalSign::Serialize(PowerTabOutputStream& stream)
{
	//------Last Checked------//
	// - Dec 10, 2004
	stream << m_letter;
	CHECK_THAT(stream.CheckState(), false);
	
	stream.WriteMFCString(m_description);
	CHECK_THAT(stream.CheckState(), false);
	
	return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool RehearsalSign::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
	//------Last Checked------//
	// - Apr 22, 2007
	stream >> m_letter;
	CHECK_THAT(stream.CheckState(), false);
	
	stream.ReadMFCString(m_description);
	CHECK_THAT(stream.CheckState(), false);
	
	return (stream.CheckState());
}

// Letter Functions
/// Sets the letter used by the rehearsal sign
/// @param letter Letter to set
bool RehearsalSign::SetLetter(int8_t letter)
{
	//------Last Checked------//
	// - Dec 10, 2004
	CHECK_THAT(IsValidLetter(letter), false);
	m_letter = letter;

	// If the rehearsal sign is being unset, clear the description
	if (letter == notSet)
		m_description.clear();
	return (true);
}

// Operations
/// Gets a formatted text representation of the rehearsal sign (combines the
/// letter and the description)
/// @return The formatted text representation of the rehearsal sign
string RehearsalSign::GetFormattedText() const
{
	//------Last Checked------//
	// - Dec 10, 2004
	return GetLetter() + " -- " + GetDescription();
}
