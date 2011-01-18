/////////////////////////////////////////////////////////////////////////////
// Name:            alternateending.cpp
// Purpose:         Stores and renders alternate ending symbols
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 3, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "alternateending.h"

#include <math.h>               // Needed for pow function
#include <sstream>

// Constructor/Destructor
/// Default Constructor
AlternateEnding::AlternateEnding()
{
	//------Last Checked------//
	// - Dec 4, 2004
}

/// Primary Constructor
/// @param system Zero-based index of the system where the alternate ending is
/// anchored
/// @param position Zero-based index of the position within the system where the
/// alternate ending is anchored
/// @param numbers Bits indicating which numbers are to be set (1st bit = 1.,
/// 2nd bit = 2., etc.)
AlternateEnding::AlternateEnding(uint32_t system, uint32_t position,
	uint16_t numbers)
{
	//------Last Checked------//
	// - Dec 4, 2004
	SetSystem(system);
	SetPosition(position);
	SetNumbers(numbers);
}

/// Copy Constructor
AlternateEnding::AlternateEnding(const AlternateEnding& alternateEnding) :
    SystemSymbol()
{
	//------Last Checked------//
	// - Dec 3, 2004
	*this = alternateEnding;
}

/// Destructor
AlternateEnding::~AlternateEnding()
{
	//------Last Checked------//
	// - Dec 3, 2004
}

// Operators
/// Assignment Operator
const AlternateEnding& AlternateEnding::operator=(
	const AlternateEnding& alternateEnding)
{
	//------Last Checked------//
	// - Dec 3, 2004
	
	// Check for assignment to self
	if (this != &alternateEnding)
		SystemSymbol::operator=(alternateEnding);
	return (*this);
}

/// Equality Operator
bool AlternateEnding::operator==(const AlternateEnding& alternateEnding) const
{
	//------Last Checked------//
	// - Jan 12, 2005
	return (SystemSymbol::operator==(alternateEnding));
}

/// Inequality Operator
bool AlternateEnding::operator!=(const AlternateEnding& alternateEnding) const
{
	//------Last Checked------//
	// - Jan 5, 2005
	return (!operator==(alternateEnding));
}
	
// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool AlternateEnding::Serialize(PowerTabOutputStream& stream)
{
	//------Last Checked------//
	// - Dec 27, 2004
	return (SystemSymbol::Serialize(stream));
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool AlternateEnding::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
	//------Last Checked------//
	// - Dec 27, 2004
	return (SystemSymbol::Deserialize(stream, version));
}

// Number Functions
/// Sets the numbers using a bit map
/// @param numbers Bit map of the numbers to set (bit 0 = 1, bit 1 = 2,
/// bit 2 = 3, etc.)
/// @return True if the numbers were set, false if not
bool AlternateEnding::SetNumbers(uint16_t numbers)
{
	//------Last Checked------//
	// - Jan 12, 2005
	CHECK_THAT(IsValidNumbers(numbers), false);
	m_data = MAKELONG(0, numbers);
	return (true);
}

/// Gets a bit map representing the numbers
/// @return Bit map representing the numbers
uint16_t AlternateEnding::GetNumbers() const
{
	//------Last Checked------//
	// - Jan 12, 2005
	return (HIWORD(m_data));
}
		
/// Sets a number
/// @param number The number to set (one based)
/// @return True if the number was set, false if not
bool AlternateEnding::SetNumber(uint32_t number)
{
	//------Last Checked------//
	// - Jan 12, 2005
	CHECK_THAT(IsValidNumber(number), false);
	
	// Note: Numbers are stored in zero-based form
	uint16_t numbers = GetNumbers();
	numbers |= (uint16_t)(pow((double)2, (double)(number - 1)));
	
	return (SetNumbers(numbers));
}

/// Determines if a number is set
/// @param number A one-based index of the number to test
/// @return True if the number is set, false if not
bool AlternateEnding::IsNumberSet(uint32_t number) const
{
	//------Last Checked------//
	// - Jan 12, 2005
	CHECK_THAT(IsValidNumber(number), false);

	// Number is one based, so subtract one
	number--;

	// Determine if bit is set
	uint16_t numbers = GetNumbers();
	uint16_t power = (uint16_t)pow((double)2, (double)number);
	return ((numbers & power) == power);
}

/// Clears a number
/// @param number The number to clear (one based)
/// @return True if the number was cleared, false if not
bool AlternateEnding::ClearNumber(uint32_t number)
{
	//------Last Checked------//
	// - Jan 12, 2005
	CHECK_THAT(IsValidNumber(number), false);
	
	uint16_t numbers = GetNumbers(); 
	numbers &= ~(uint16_t)(pow((double)2, (double)(number - 1)));
	SetNumbers(numbers);
	return (true);
}

/// Gets the alternate ending text (numbers + D.C./D.S./D.S.S.)
/// @return Text representation of the alternate ending
string AlternateEnding::GetText() const
{
	//------Last Checked------//
	// - Dec 3, 2004
	string returnValue;

	int32_t groupStart = -1;
	int32_t groupEnd = -1;
		
	// Construct the numbers
	uint32_t i = 1;
	uint32_t lastNumber = 8;
	for (; i <= lastNumber; i++)
	{
		bool numberSet = IsNumberSet(i);

		if (numberSet)
		{
			// Starting a new group of numbers
			if (groupStart == -1)
				groupStart = groupEnd = i;
			// Continuing existing group
			else
				groupEnd = i;
		}
		
		// Always treat the last number like the end of a group
		if (i == lastNumber)
			numberSet = false;

		// We've reached the end of a group, if groupStart != -1, then we have a
		// group
		if (!numberSet && groupStart != -1)
		{
			// Add a separator
			if (!returnValue.empty())
				returnValue += ", ";
				
			string temp;

			// Single number
			if (groupStart == groupEnd)
			{
				temp = GetNumberText(groupStart) + ".";
			}
			// 2 numbers
			else if (groupStart == (groupEnd - 1))
			{
				temp = GetNumberText(groupStart) + "., " + GetNumberText(groupEnd) + ".";
			}
			// > 2 numbers
			else 
			{
				temp = GetNumberText(groupStart) + ".-" + GetNumberText(groupEnd) + ".";
			}
				
			returnValue += temp;
				
			// Reset the group data
			groupStart = groupEnd = -1;
		}
	}
	
	// Construct the special symbols
	i = daCapo;
	for (; i <= dalSegnoSegno; i++)
	{
		if (IsNumberSet(i))
		{
			if (!returnValue.empty())
				returnValue += ", ";
			returnValue += GetNumberText(i).c_str();
		}
	}

	return (returnValue);
}

/// Gets the text for a number
/// @param number Number to get the text for (one based)
/// @return Text representation of the number
string AlternateEnding::GetNumberText(uint32_t number)
{
	//------Last Checked------//
	// - Dec 3, 2004
	CHECK_THAT(IsValidNumber(number), "");
	
	if (number == daCapo)
		return "D.C.";
	else if (number == dalSegno)
		return "D.S.";
	else if (number == dalSegnoSegno)
		return "D.S.S.";
	else
	{
		std::stringstream value;
		value << number;
		return value.str();
	}
}
