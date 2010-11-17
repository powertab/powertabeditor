/////////////////////////////////////////////////////////////////////////////
// Name:            chorddiagram.cpp
// Purpose:         Stores and renders a chord diagram
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 16, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "chorddiagram.h"

// Default Constants
const uint8_t ChordDiagram::DEFAULT_TOP_FRET     = 0;

// Top Fret Constants
const uint8_t ChordDiagram::MIN_TOP_FRET         = 0;
const uint8_t ChordDiagram::MAX_TOP_FRET         = 20;

// Fret Number Constants
const uint8_t ChordDiagram::MIN_FRET_NUMBER      = 0;
const uint8_t ChordDiagram::MAX_FRET_NUMBER      = 24;

/// Default Constructor
ChordDiagram::ChordDiagram() :
	m_topFret(DEFAULT_TOP_FRET)
{
	//------Last Checked------//
	// - Jan 14, 2005
}

/// Primary Constructor
/// @param chordName Chord name represented in the chord diagram
/// @param topFret The fret represented at the top of the chord diagram (zero =
/// the nut)
/// @param fretNumber1 Fret number for the 1st string in the chord diagram (for
/// standard guitar tuning, this is the high E string)
/// @param fretNumber2 Fret number for the 2nd string in the chord diagram
/// @param fretNumber3 Fret number for the 3rd string in the chord diagram
/// @param fretNumber4 Fret number for the 4th string in the chord diagram
/// @param fretNumber5 Fret number for the 5th string in the chord diagram
/// @param fretNumber6 Fret number for the 6th string in the chord diagram
/// @param fretNumber7 Fret number for the 7th string in the chord diagram
ChordDiagram::ChordDiagram(const ChordName& chordName, uint8_t topFret,
	uint8_t fretNumber1, uint8_t fretNumber2, uint8_t fretNumber3,
	uint8_t fretNumber4, uint8_t fretNumber5, uint8_t fretNumber6,
	uint8_t fretNumber7) : m_topFret(topFret)
{
	//------Last Checked------//
	// - Jan 15, 2005
	assert(IsValidTopFret(topFret));
	SetChordName(chordName);
	AddFretNumbers(fretNumber1, fretNumber2, fretNumber3, fretNumber4,
		fretNumber5, fretNumber6, fretNumber7);
}

/// Secondary Constructor
/// @param topFret The fret represented at the top of the chord diagram (zero =
/// the nut)
/// @param fretNumber1 Fret number for the 1st string in the chord diagram (for
/// standard guitar tuning, this is the high E string)
/// @param fretNumber2 Fret number for the 2nd string in the chord diagram
/// @param fretNumber3 Fret number for the 3rd string in the chord diagram
/// @param fretNumber4 Fret number for the 4th string in the chord diagram
/// @param fretNumber5 Fret number for the 5th string in the chord diagram
/// @param fretNumber6 Fret number for the 6th string in the chord diagram
/// @param fretNumber7 Fret number for the 7th string in the chord diagram
ChordDiagram::ChordDiagram(uint8_t topFret, uint8_t fretNumber1,
	uint8_t fretNumber2, uint8_t fretNumber3, uint8_t fretNumber4,
	uint8_t fretNumber5, uint8_t fretNumber6, uint8_t fretNumber7) :
	m_topFret(topFret)
{
	//------Last Checked------//
	// - Jan 15, 2005
	assert(IsValidTopFret(topFret));
	AddFretNumbers(fretNumber1, fretNumber2, fretNumber3, fretNumber4,
		fretNumber5, fretNumber6, fretNumber7);
}

/// Copy Constructor
ChordDiagram::ChordDiagram(const ChordDiagram& chordDiagram) :
	m_topFret(DEFAULT_TOP_FRET)
{
	//------Last Checked------//
	// - Dec 16, 2004
	*this = chordDiagram;
}

/// Destructor
ChordDiagram::~ChordDiagram()
{
	//------Last Checked------//
	// - Jan 14, 2005
}

// Operators
/// Assignment Operator
const ChordDiagram& ChordDiagram::operator=(const ChordDiagram& chordDiagram)
{
	//------Last Checked------//
	// - Jan 14, 2005
	
	// Check for assignment to self
	if (this != &chordDiagram)
	{
		m_chordName = chordDiagram.m_chordName;
		m_topFret = chordDiagram.m_topFret;

		DeleteFretNumberArrayContents();

		size_t i = 0;
		size_t count = chordDiagram.m_fretNumberArray.size();
		for (; i < count; i++)
			m_fretNumberArray.push_back(chordDiagram.m_fretNumberArray[i]);
	}
	
	return *this;
}

/// Equality Operator
bool ChordDiagram::operator==(const ChordDiagram& chordDiagram) const
{
	//------Last Checked------//
	// - Jan 14, 2005
	
	size_t thisStringCount = GetStringCount();
	size_t otherStringCount = chordDiagram.GetStringCount();    
	if (thisStringCount != otherStringCount)
		return (false);
		
	size_t i = 0;
	for (; i < thisStringCount; i++)
	{
		if (m_fretNumberArray[i] != chordDiagram.m_fretNumberArray[i])
			return (false);
	}

	return (
		(m_chordName == chordDiagram.m_chordName) &&
		(m_topFret == chordDiagram.m_topFret)
	);
}

/// Inequality Operator
bool ChordDiagram::operator!=(const ChordDiagram& chordDiagram) const
{
	//------Last Checked------//
	// - Jan 5, 2005
	return (!operator==(chordDiagram));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool ChordDiagram::Serialize(PowerTabOutputStream& stream)
{
	//------Last Checked------//
	// - Jan 14, 2005
	m_chordName.Serialize(stream);
	CHECK_THAT(stream.CheckState(), false);
	
	stream << m_topFret;
	CHECK_THAT(stream.CheckState(), false);

	size_t count = GetStringCount();
	stream << (uint8_t)count;
	CHECK_THAT(stream.CheckState(), false);
	
	size_t i = 0;
	for (; i < count; i++)
	{
		stream << m_fretNumberArray[i];
		CHECK_THAT(stream.CheckState(), false);
	}
		
	return (true);
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool ChordDiagram::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
	//------Last Checked------//
	// - Jan 14, 2005
	m_chordName.Deserialize(stream, version);
	CHECK_THAT(stream.CheckState(), false);
	
	stream >> m_topFret;
	CHECK_THAT(stream.CheckState(), false);
	
	uint8_t count = 0;
	stream >> count;
	CHECK_THAT(stream.CheckState(), false);

	size_t i = 0;
	for (; i < count; i++)
	{
		uint8_t fretNumber = 0;
		stream >> fretNumber;
		CHECK_THAT(stream.CheckState(), false);
		
		m_fretNumberArray.push_back(fretNumber);
	}
		
	return (true);
}

// Chord Diagram Functions
/// Sets the contents of the ChordDiagram object
/// @param chordName Chord name represented in the chord diagram
/// @param topFret The fret represented at the top of the chord diagram (zero =
/// the nut)
/// @param fretNumber1 Fret number for the 1st string in the chord diagram (for
/// standard guitar tuning, this is the high E string)
/// @param fretNumber2 Fret number for the 2nd string in the chord diagram
/// @param fretNumber3 Fret number for the 3rd string in the chord diagram
/// @param fretNumber4 Fret number for the 4th string in the chord diagram
/// @param fretNumber5 Fret number for the 5th string in the chord diagram
/// @param fretNumber6 Fret number for the 6th string in the chord diagram
/// @param fretNumber7 Fret number for the 7th string in the chord diagram
/// @return True if the chord diagram was set, false if not
bool ChordDiagram::SetChordDiagram(const ChordName& chordName, uint8_t topFret,
	uint8_t fretNumber1, uint8_t fretNumber2, uint8_t fretNumber3,
	uint8_t fretNumber4, uint8_t fretNumber5, uint8_t fretNumber6,
	uint8_t fretNumber7)
{
	//------Last Checked------//
	// - Jan 15, 2005
	SetChordName(chordName);
	if (!SetTopFret(topFret))
		return (false);
	return (AddFretNumbers(fretNumber1, fretNumber2, fretNumber3, fretNumber4,
		fretNumber5, fretNumber6, fretNumber7));
}

// Fret Number Functions
/// Sets the fret number for an existing string in the chordDiagram
/// @param string String to set the fret number for
/// @param fretNumber Fret number to set
/// @return True if the fret number was set, false if not
bool ChordDiagram::SetFretNumber(uint32_t string, uint8_t fretNumber)
{
	//------Last Checked------//
	// - Jan 15, 2005
	CHECK_THAT(IsValidString(string), false);
	CHECK_THAT(IsValidFretNumber(fretNumber), false);
	m_fretNumberArray[string] = fretNumber;
	return (true);
}

/// Gets the fret number on a given string
/// @param string String to get the fret number for
/// @return The fret number on the string
uint8_t ChordDiagram::GetFretNumber(uint32_t string) const
{
	//------Last Checked------//
	// - Jan 15, 2005
	CHECK_THAT(IsValidString(string), 0); 
	return (m_fretNumberArray[string]);
}
		
/// Sets the fret numbers for the chord diagram
/// @param fretNumber1 Fret number value for the 1st string in the chord diagram
/// (for standard guitar chord diagram, this is the high E string)
/// @param fretNumber2 Fret number value for the 2nd string in the chord diagram
/// @param fretNumber3 Fret number value for the 3rd string in the chord diagram
/// @param fretNumber4 Fret number value for the 4th string in the chord diagram
/// @param fretNumber5 Fret number value for the 5th string in the chord diagram
/// @param fretNumber6 Fret number value for the 6th string in the chord diagram
/// @param fretNumber7 Fret number value for the 7th string in the chord diagram
/// @return True if the chord diagram fret numbers were set, false if not
bool ChordDiagram::AddFretNumbers(uint8_t fretNumber1, uint8_t fretNumber2,
	uint8_t fretNumber3, uint8_t fretNumber4, uint8_t fretNumber5,
	uint8_t fretNumber6, uint8_t fretNumber7)
{
	//------Last Checked------//
	// - Jan 15, 2005
	CHECK_THAT(IsValidFretNumber(fretNumber1), false);
	CHECK_THAT(IsValidFretNumber(fretNumber2), false);
	CHECK_THAT(IsValidFretNumber(fretNumber3), false);
	CHECK_THAT(IsValidFretNumber(fretNumber4), false);
	CHECK_THAT(IsValidFretNumber(fretNumber5), false);
	CHECK_THAT(IsValidFretNumber(fretNumber6), false);
	CHECK_THAT(IsValidFretNumber(fretNumber7), false);
		
	DeleteFretNumberArrayContents();

	m_fretNumberArray.push_back(fretNumber1);
	m_fretNumberArray.push_back(fretNumber2);
	m_fretNumberArray.push_back(fretNumber3);

	// CASE: Notes 4-7 are not mandatory, if set to notUsed, ignore them
	if (fretNumber4 == notUsed)
		return (true);
	m_fretNumberArray.push_back(fretNumber4);

	if (fretNumber5 == notUsed)
		return (true);
	m_fretNumberArray.push_back(fretNumber5);

	if (fretNumber6 == notUsed)
		return (true);
	m_fretNumberArray.push_back(fretNumber6);
	if (fretNumber7 == notUsed)
		return (true);
	m_fretNumberArray.push_back(fretNumber7);
	
	return (true);
}

// Voicing Functions
/// Determines if the chord diagram's voicing is the same as that of another
/// ChordDiagram object
/// @param chordDiagram ChordDiagram object to compare with
/// @return True if the chord diagrams have the same voicing, false if not
bool ChordDiagram::IsSameVoicing(const ChordDiagram& chordDiagram) const
{
	//------Last Checked------//
	// - Jan 15, 2005
	
	size_t thisStringCount = GetStringCount();
	size_t otherStringCount = chordDiagram.GetStringCount();
	
	// Chord diagrams have a different number of strings
	if (thisStringCount != otherStringCount)
		return (false);

	// Check each string for matching fret numbers
	size_t i = 0;
	for (; i < thisStringCount; i++)
	{
		if (m_fretNumberArray[i] != chordDiagram.m_fretNumberArray[i])
			return (false);
	}

	return (true);
}

/// Determines if the chord diagram's voicing is the same
/// @param fretNumber1 Fret number for the 1st string in the chord diagram (for
/// standard guitar tuning, this is the high E string)
/// @param fretNumber2 Fret number for the 2nd string in the chord diagram
/// @param fretNumber3 Fret number for the 3rd string in the chord diagram
/// @param fretNumber4 Fret number for the 4th string in the chord diagram
/// @param fretNumber5 Fret number for the 5th string in the chord diagram
/// @param fretNumber6 Fret number for the 6th string in the chord diagram
/// @param fretNumber7 Fret number for the 7th string in the chord diagram
/// @return True if all of the chord diagram fret numbers match, false if not
bool ChordDiagram::IsSameVoicing(uint8_t fretNumber1, uint8_t fretNumber2,
	uint8_t fretNumber3, uint8_t fretNumber4, uint8_t fretNumber5,
	uint8_t fretNumber6, uint8_t fretNumber7) const
{
	//------Last Checked------//
	// - Jan 15, 2005
	ChordDiagram temp;
	temp.AddFretNumbers(fretNumber1, fretNumber2, fretNumber3, fretNumber4,
		fretNumber5, fretNumber6, fretNumber7);
	return (IsSameVoicing(temp));
}

// Operations
/// Gets the spelling for the chord diagram (i.e. 0 2 2 1 0 0)
/// @return The spelling for the chord diagram
string ChordDiagram::GetSpelling() const
{
	//------Last Checked------//
	// - Jan 15, 2005
	string returnValue;

	size_t i = GetStringCount();
	for (; i > 0; i--)
	{
		std::stringstream fretNumber;
		if (m_fretNumberArray[i - 1] == stringMuted)
			returnValue += "x";
		else
			fretNumber << m_fretNumberArray[i - 1];
		returnValue += fretNumber.str();

		// Add a space between numbers
		if (i > 1)
			returnValue += " ";
	}

	return (returnValue);
}
