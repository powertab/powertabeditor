/////////////////////////////////////////////////////////////////////////////
// Name:            powertabinputstream.cpp
// Purpose:         Input stream used to deserialize MFC based Power Tab data
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 19, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "powertabinputstream.h"
#include "colour.h"
#include "rect.h"

// Constructor/Destructor
/// Primary Constructor
PowerTabInputStream::PowerTabInputStream(const string& filename)
{
    m_stream.open(filename.c_str(), ifstream::in | ifstream::binary);
}

/// Destructor
PowerTabInputStream::~PowerTabInputStream()
{
	m_stream.close();
}

// Read Functions
/// Reads a count value in MFC format from the stream
/// @return The count value
uint32_t PowerTabInputStream::ReadCount()
{
	//------Last Checked------//
	// - Dec 30, 2004
	uint32_t returnValue = 0;

	uint16_t wordCount = 0;
	*this >> wordCount;
	CHECK_THAT(CheckState(), 0);
	
	// 16-bit count
	if (wordCount != 0xffff)
		returnValue = wordCount;
	// 32-bit count
	else
	{
		*this >> returnValue;
		CHECK_THAT(CheckState(), 0);
	}

	return (returnValue);
}

/// Loads an Microsoft based (MFC) string from the stream and copies the text to
/// a string object
/// @param string string object to copy the text to
/// @return True if the string was read, false if not
bool PowerTabInputStream::ReadMFCString(string& str)
{
	str = "";

	uint32_t length = ReadMFCStringLength();
	str.reserve(length);

	for (uint32_t i=0; i< length; i++)
	{
		char temp;
		*this >> temp;
		str += temp;
	}
	
	return (CheckState());
}

/// Reads a Win32 format COLORREF type from the stream
/// @param color Color 
/// @return True if the color was read, false if not
bool PowerTabInputStream::ReadWin32ColorRef(Colour& color)
{
	//------Last Checked------//
	// - Dec 27, 2004
	color.Set(0,0,0);

	uint32_t colorref = 0;
	*this >> colorref;
	CHECK_THAT(CheckState(), false);
		
	color.Set(LOBYTE(LOWORD(colorref)), HIBYTE(LOWORD(colorref)),
		LOBYTE(HIWORD(colorref)));
	
	return (true);
}

/// Loads a Microsoft CRect object from the stream and copies its values to a
/// Rect object
/// @param rect Rect object to copy the CRect values to
/// @return True if the rect was read, false if not
bool PowerTabInputStream::ReadMFCRect(Rect& rect)
{
	//------Last Checked------//
	// - Dec 27, 2004

	rect = Rect(0,0,0,0);

	int32_t left = 0, top = 0, right = 0, bottom = 0;
	*this >> left >> top >> right >> bottom;
	CHECK_THAT(CheckState(), false);
	
	rect.SetLeft(left);
	rect.SetTop(top);
	rect.SetRight(right);
	rect.SetBottom(bottom);
	   
	return (CheckState());
}

void PowerTabInputStream::ReadClassInformation()
{
	//const uint16_t   NULL_TAG        = 0;            // Special tag indicating NULL pointers
	const uint16_t   NEW_CLASS_TAG   = 0xffff;       // Indicates that class that is new to the archive context
	const uint16_t   CLASS_TAG       = 0x8000;       // Indicates that the class has already been "seen" in this context
	const uint32_t   BIG_CLASS_TAG   = 0x80000000;   // 0x80000000 indicates big class tag (OR'd)
	const uint16_t   BIG_OBJECT_TAG  = 0x7fff;       // Indicates that a 32-bit object ID tag should be read
	//const uint32_t   MAX_MAP_COUNT   = 0x3ffffffe;   // 0x3ffffffe last valid mapCount

	uint16_t word_tag = 0;
	uint32_t obj_tag = 0;

	// read the ID tag - if it is prefixed by BIG_OBJECT_TAG, then a 32-bit ID follows
	// NOTE - the ID tag doesn't have much meaning for our purposes - it is used by MFC CArchive for mapping
	*this >> word_tag;

	if (word_tag == BIG_OBJECT_TAG)
	{
		*this >> obj_tag; // read the 32-bit ID tag
	}
	else
	{
		obj_tag = ((word_tag & CLASS_TAG) << 16) | (word_tag & ~CLASS_TAG);
	}

	// check for object tag
	if (!(obj_tag & BIG_CLASS_TAG))
	{
		return;
	}

	// object is a new class
	if (word_tag == NEW_CLASS_TAG)
	{
		// read the class information for the object (class schema and class name)
		uint16_t schema = 0;
		uint16_t length = 0;
		uint8_t character = 0;

		*this >> schema;
		*this >> length;
		for (uint16_t i=0; i< length; i++)
		{
			*this >> character;
		}
	}

	else
	{
		// existing class index in obj_tag followed by new object
		// no more reading to do here
		return;
	}

}


/// Reads the length of a Microsoft based (MFC) string from a data input stream.
/// Also determines the character size of the text (ANSI or Unicode)
/// @param charSize Size of each character in the string; 1 - ANSI, 2 - Unicode
/// @return The length of the string, in characters
uint32_t PowerTabInputStream::ReadMFCStringLength()
{
	uint8_t byteLength = 0;
	uint16_t wordLength = 0;
	uint32_t doubleWordLength = 0;
	
	// First, try to read a one-byte length
	*this >> byteLength;
	CHECK_THAT(CheckState(), 0);
	
	if (byteLength < 0xff)
		return byteLength;

	// Try a two-byte length
	*this >> wordLength;
	CHECK_THAT(CheckState(), 0);

	if (wordLength < 0xffff)
		return wordLength;

	// 4-byte length
	*this >> doubleWordLength;
	CHECK_THAT(CheckState(), 0);
	
	return (doubleWordLength);
}
