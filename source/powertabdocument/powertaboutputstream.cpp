/////////////////////////////////////////////////////////////////////////////
// Name:            powertaboutputstream.cpp
// Purpose:         Handles serialization of MFC based Power Tab data
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 20, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "powertaboutputstream.h"
#include <string.h>

// Constructor/Destructor
/// Primary Constructor
PowerTabOutputStream::PowerTabOutputStream(const string& filename) :
	m_mapsInitialized(false), m_mapCount(0),
	m_lastPowerTabError(POWERTABSTREAM_NO_ERROR)
{
    m_stream.open(filename.c_str());
}

/// Destructor
PowerTabOutputStream::~PowerTabOutputStream()
{
    m_stream.close();
}

// Write Functions
/// Writes a count value to the stream in MFC format
/// @param count Count value to write
/// @return True if the count value was written, false if not
bool PowerTabOutputStream::WriteCount(uint32_t count)
{
    //------Last Checked------//
    // - Dec 20, 2004
    CHECK_THAT(CheckState(), false);

    // 16-bit count
    if (count < 0xffff)
        *this << (uint16_t)count;
    // 32-bit count
    else
        *this << count;

    return (CheckState());
}

/// Writes ANSI based text to the stream
/// @param text Text to write
/// @return True if the text was written, false if not
bool PowerTabOutputStream::WriteAnsiText(const string& text)
{
    //------Last Checked------//
    // - Dec 24, 2004

    // Get an ANSI representation of the text
    const char * buffer = text.c_str();

    // Write text out, one letter at a time
    // TODO: Update this to write chunks in wxWidgets 2.5
    size_t i = 0;
    size_t length = strlen(buffer);
    for (; i < length; i++)
    {
        int8_t letter = buffer[i];
        *this << letter;
        CHECK_THAT(CheckState(), false);
    }

    return (CheckState());
}

/// Writes a string object to the stream using the Microsoft based (MFC)
/// string format
/// @param string String to write
/// @return True if the string was written, false if not
bool PowerTabOutputStream::WriteMFCString(const string& string)
{
    //------Last Checked------//
    // - Dec 27, 2004
    // Note: all Power Tab MFC strings are stored as ANSI text

    // Write the string length
    if (!WriteMFCStringLength(string.length(), false))
        return (false);

    // Write the text
    return (WriteAnsiText(string));
}

/// Writes the length of a Microsoft based (MFC) string to the output stream
/// @param length Length to write
/// @param unicode Is text Unicode or ANSI format?
/// @return True if the string length was written, false if not
bool PowerTabOutputStream::WriteMFCStringLength(uint32_t length, bool unicode)
{
    //------Last Checked------//
    // - Dec 27, 2004

    // Tag the Unicode strings
    if (unicode)
    {
        *this << (uint8_t)BYTE_PLACEHOLDER;
        CHECK_THAT(CheckState(), false);
        *this << (uint16_t)UNICODE_MARKER;
        CHECK_THAT(CheckState(), false);
    }

    // If length is less than byte placeholder, write the length as a byte value
    if (length < BYTE_PLACEHOLDER)
        *this << (uint8_t)length;
    // If length is less than the Unicode marker, write the length as a word
    // value
    else if (length < UNICODE_MARKER)
    {
        *this << (uint8_t)BYTE_PLACEHOLDER;
        CHECK_THAT(CheckState(), false);
        *this << (uint16_t)length;
    }
    // Write the length as a double word
    else
    {
        *this << (uint8_t)BYTE_PLACEHOLDER;
        CHECK_THAT(CheckState(), false);
        *this << (uint16_t)WORD_PLACEHOLDER;
        CHECK_THAT(CheckState(), false);
        *this << (uint32_t)length;
    }

    return (CheckState());
}

/// Writes a Color object to the stream using Windows 32-bit COLORREF format
/// @param color Color to write
/// @return True if the color was written, false if not
bool PowerTabOutputStream::WriteWin32ColorRef(Colour color)
{
    //------Last Checked------//
    // - Dec 27, 2004
    // COLORREF format = 0x00bbggrr
    uint32_t colorref = MAKELONG(MAKEWORD(color.Red(), color.Green()),
                                 MAKEWORD(color.Blue(), 0));
    *this << colorref;
    return (CheckState());
}

/// Writes a Rect object to the stream using the Microsoft CRect class format
/// @param rect Rect object whose values are to be written
/// @return True if the rect was written, false if not
bool PowerTabOutputStream::WriteMFCRect(const Rect& rect)
{
    //------Last Checked------//
    // - Dec 27, 2004

    // CRect format = left, top, right, bottom, all 32 bit integers
    *this << (int32_t)rect.GetLeft() << (int32_t)rect.GetTop() <<
            (int32_t)rect.GetRight() << (int32_t)rect.GetBottom();
    return (CheckState());
}

/// Writes a Power Tab object to the stream
/// @param object Power Tab object to write
/// @return True if the object was written, false if not
bool PowerTabOutputStream::WriteObject(PowerTabObject* object)
{
    //------Last Checked------//
    // - Dec 21, 2004
    // object can be NULL

    // Make sure maps are initialized
    if (!MapObject(NULL))
        return (false);

    // Save out NULL tag to represent NULL pointer
    if (object == NULL)
        *this << NULL_TAG;
    else
    {
        // Lookup the object in the map
        if (m_objectHashMap.find((PowerTabObject *)object) != m_objectHashMap.end())
            // Assumes initialized to 0 map
        {
            uint32_t nObjectIndex = (uint32_t)m_objectHashMap[(PowerTabObject *)object];

            // Save out index of already stored object
            if (nObjectIndex < BIG_OBJECT_TAG)
                *this << (uint16_t)nObjectIndex;
            else
            {
                *this << BIG_OBJECT_TAG;
                CHECK_THAT(CheckState(), false);
                *this << nObjectIndex;
            }
        }
        else
        {
            // Write class of object first
            if (!WriteClassInformation(object))
                return (false);

            // Enter in stored object table, checking for overflow
            if (!MapObject(object))
                return (false);

            // Cause the object to serialize itself
            object->Serialize(*this);
        }
    }

    return (CheckState());
}

/// Writes a Power Tab object's class information to the stream
/// @param object Power Tab object whose class information is to be written
/// @return True if the class information was written, false if not
bool PowerTabOutputStream::WriteClassInformation(const PowerTabObject* object)
{
    //------Last Checked------//
    // - Dec 21, 2004

    if (object == NULL)
    {
        m_lastPowerTabError = POWERTABSTREAM_BAD_CLASS;
        return (false);
    }

    // Make sure the maps are initialized
    MapObject(NULL);

    // Write out class id of object, with high bit set to indicate
    // new object follows
    //string classId = object->GetMFCClassId();

    // -0 is the id for classes that don't use MFC class information,
    // and thus can't be saved using this format
    /*if (classId == "-0")
	{
		m_lastPowerTabError = POWERTABSTREAM_BAD_CLASS;
		return (false);
        }

	// ASSUME: initialized to 0 map
	uint32_t nClassIndex = 0;
	if (m_classInfoHashMap.find(classId) != m_classInfoHashMap.end())
	{
		nClassIndex = (uint32_t)m_classInfoHashMap[classId];
		// Previously seen class, write out the index tagged by high bit
		if (nClassIndex < BIG_OBJECT_TAG)
			*this << (uint16_t)(CLASS_TAG | nClassIndex);
		else
		{
			*this << BIG_OBJECT_TAG;
			CHECK_THAT(CheckState(), false);
			*this << (BIG_CLASS_TAG | nClassIndex);
		}
	}
	else
	{
		// Store new class
		*this << NEW_CLASS_TAG;
		CHECK_THAT(CheckState(), false);
		
                //object->WriteMFCClassInformation(*this);
		CHECK_THAT(CheckState(), false);

		// Store new class reference in map, checking for overflow
		if (!CheckCount())
			return (false);
		m_classInfoHashMap[classId] = (uint32_t)m_mapCount++;
        }*/

    return (CheckState());
}

/// Gets the error message associated with the last error that occurred in the
/// stream
/// @return The error message associated with the last error that occurred in
/// the stream
string PowerTabOutputStream::GetLastErrorMessage()
{
    //------Last Checked------//
    // - Apr 22, 2007
    string returnValue = "";

    if (CheckState())
        returnValue = "No error";
    else if (m_stream.eof())
        returnValue = "End of file reached";
    else if (fail())
        returnValue = "Generic write error";
    else if (m_lastPowerTabError == POWERTABSTREAM_INVALID_MARKER)
        returnValue = "Invalid file marker";
    else if (m_lastPowerTabError == POWERTABSTREAM_INVALID_FILE_VERSION)
        returnValue = "Invalid file version";
    else if (m_lastPowerTabError == POWERTABSTREAM_INVALID_FILE_TYPE)
        returnValue = "Invalid file type";
    else if (m_lastPowerTabError == POWERTABSTREAM_BAD_INDEX)
        returnValue = "Bad index";
    else if (m_lastPowerTabError == POWERTABSTREAM_BAD_CLASS)
        returnValue = "Bad class";
    else if (m_lastPowerTabError == POWERTABSTREAM_BAD_SCHEMA)
        returnValue = "Bad schema";

    return (returnValue);
}

/// Validates the internal map count (checks for overflow)
/// @return True if the map count is valid, false if not
bool PowerTabOutputStream::CheckCount()
{
    //------Last Checked------//
    // - Dec 21, 2004
    bool returnValue = (m_mapCount < MAX_MAP_COUNT);
    if (!returnValue)
        m_lastPowerTabError = POWERTABSTREAM_BAD_INDEX;
    return (returnValue);
}

// Operations
/// Maps a Power Tab object to its object index
/// @param object Power Tab object to map
/// @return True if the object was mapped, false if not
bool PowerTabOutputStream::MapObject(const PowerTabObject* object)
{
    //------Last Checked------//
    // - Dec 20, 2004

    // Initialize the internal maps
    if (!m_mapsInitialized)
    {
        m_mapsInitialized = true;

        // Note: use void* to void* because it is used for HANDLE maps too
        //m_objectHashMap[NULL] = (void*)(uint32_t)NULL_TAG;
        //m_mapCount = 1;
    }

    // Map the object
    if (object != NULL)
    {
        if (!CheckCount())
            return (false);
        m_objectHashMap[(PowerTabObject *)object] = (uint32_t)m_mapCount++;
    }

    return (true);
}
