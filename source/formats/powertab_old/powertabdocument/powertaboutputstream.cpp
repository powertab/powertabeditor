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
#include "powertabobject.h"
#include "rect.h"
#include "colour.h"

namespace PowerTabDocument {

using std::string;

// Constructor/Destructor
/// Primary Constructor
PowerTabOutputStream::PowerTabOutputStream(std::ostream& stream) :
    m_mapsInitialized(false), m_mapCount(0),
    m_lastPowerTabError(POWERTABSTREAM_NO_ERROR),
    m_stream(stream)
{
    //m_stream.open(filename.c_str(), ofstream::out | ofstream::binary);
}

// Write Functions
/// Writes a count value to the stream in MFC format
/// @param count Count value to write
/// @return True if the count value was written, false if not
bool PowerTabOutputStream::WriteCount(uint32_t count)
{
    //------Last Checked------//
    // - Dec 20, 2004
    PTB_CHECK_THAT(CheckState(), false);

    // 16-bit count
    if (count < 0xffff)
        *this << (uint16_t)count;
    // 32-bit count
    else
        *this << count;

    return (CheckState());
}

/// Writes a string object to the stream
/// @param string String to write
/// @return True if the string was written, false if not
bool PowerTabOutputStream::WriteMFCString(const string& str)
{
    const uint32_t length = static_cast<uint32_t>(str.length());
    // Write the string length
    if (!WriteMFCStringLength(length, false))
        return (false);

    // Write the text
    m_stream.write(str.data(), length);

    return CheckState();
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
        PTB_CHECK_THAT(CheckState(), false);
        *this << (uint16_t)UNICODE_MARKER;
        PTB_CHECK_THAT(CheckState(), false);
    }

    // If length is less than byte placeholder, write the length as a byte value
    if (length < BYTE_PLACEHOLDER)
        *this << (uint8_t)length;
    // If length is less than the Unicode marker, write the length as a word
    // value
    else if (length < UNICODE_MARKER)
    {
        *this << (uint8_t)BYTE_PLACEHOLDER;
        PTB_CHECK_THAT(CheckState(), false);
        *this << (uint16_t)length;
    }
    // Write the length as a double word
    else
    {
        *this << (uint8_t)BYTE_PLACEHOLDER;
        PTB_CHECK_THAT(CheckState(), false);
        *this << (uint16_t)WORD_PLACEHOLDER;
        PTB_CHECK_THAT(CheckState(), false);
        *this << (uint32_t)length;
    }

    return (CheckState());
}

/// Writes a Color object to the stream
/// @param colour Colour to write
/// @return True if the color was written, false if not
bool PowerTabOutputStream::WriteWin32ColorRef(const Colour& colour)
{
    *this << colour.Red() << colour.Green() << colour.Blue() << colour.Alpha();
    return CheckState();
}

/// Writes a Rect object to the stream
/// @param rect Rect object whose values are to be written
/// @return True if the rect was written, false if not
bool PowerTabOutputStream::WriteMFCRect(const Rect& rect)
{
    *this << rect.GetLeft() << rect.GetTop() << rect.GetRight() << rect.GetBottom();
    return CheckState();
}

/// Writes a Power Tab object to the stream, in the format of MFC's CArchive
/// @param object Power Tab object to write
/// @return True if the object was written, false if not
bool PowerTabOutputStream::WriteObject(const PowerTabObject* object)
{
    // object can be NULL

    // Make sure maps are initialized
    if (!MapObject(nullptr))
        return (false);

    // Save out NULL tag to represent NULL pointer
    if (object == nullptr)
        *this << NULL_TAG;
    else
    {
        // Lookup the object in the map
        if (m_objectHashMap.find(object) != m_objectHashMap.end())
            // Assumes initialized to 0 map
        {
            uint32_t nObjectIndex = m_objectHashMap[object];

            // Save out index of already stored object
            if (nObjectIndex < BIG_OBJECT_TAG)
                *this << (uint16_t)nObjectIndex;
            else
            {
                *this << BIG_OBJECT_TAG;
                PTB_CHECK_THAT(CheckState(), false);
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
    if (object == nullptr)
    {
        m_lastPowerTabError = POWERTABSTREAM_BAD_CLASS;
        return (false);
    }

    // Make sure the maps are initialized
    MapObject(nullptr);

    // Write out class id of object, with high bit set to indicate
    // new object follows
    string classId = object->GetMFCClassName();

    // -0 is the id for classes that don't use MFC class information,
    // and thus can't be saved using this format
    if (classId == "-0")
    {
        m_lastPowerTabError = POWERTABSTREAM_BAD_CLASS;
        return (false);
    }

    // ASSUME: initialized to 0 map
    if (m_classInfoHashMap.find(classId) != m_classInfoHashMap.end())
    {
        const uint32_t nClassIndex = m_classInfoHashMap[classId];
        // Previously seen class, write out the index tagged by high bit
        if (nClassIndex < BIG_OBJECT_TAG)
            *this << (uint16_t)(CLASS_TAG | nClassIndex);
        else
        {
            *this << BIG_OBJECT_TAG;
            PTB_CHECK_THAT(CheckState(), false);
            *this << (BIG_CLASS_TAG | nClassIndex);
        }
    }
    else
    {
        // Store new class
        *this << NEW_CLASS_TAG;
        PTB_CHECK_THAT(CheckState(), false);

        // Write MFC Class Information
        *this << object->GetMFCClassSchema();
        PTB_CHECK_THAT(CheckState(), false);

        const string className = object->GetMFCClassName();
        const uint16_t length = static_cast<uint16_t>(className.length());
        *this << length;
        m_stream.write(className.data(), length);

        PTB_CHECK_THAT(CheckState(), false);

        // Store new class reference in map, checking for overflow
        if (!CheckCount())
            return (false);
        m_classInfoHashMap[classId] = (uint32_t)m_mapCount++;
    }

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
    // Initialize the internal maps
    if (!m_mapsInitialized)
    {
        m_mapsInitialized = true;

        m_objectHashMap[nullptr] = (uint32_t)NULL_TAG;
        m_mapCount = 1;
    }

    // Map the object
    if (object != nullptr)
    {
        if (!CheckCount())
            return false;
        m_objectHashMap[object] = (uint32_t)m_mapCount++;
    }

    return (true);
}

}
