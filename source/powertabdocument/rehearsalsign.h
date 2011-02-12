/////////////////////////////////////////////////////////////////////////////
// Name:            rehearsalsign.h
// Purpose:         Stores and renders Rehearsal Sign symbols
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 10, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __REHEARSALSIGN_H__
#define __REHEARSALSIGN_H__

#include "powertabobject.h"
#include "macros.h"

/// Stores and renders Rehearsal Sign symbols
class RehearsalSign : public PowerTabObject
{
// Constants
public:
    // Default constants
    static const int8_t         DEFAULT_LETTER;             ///< Default value for the letter member variable
    static const char*        DEFAULT_DESCRIPTION;        ///< Default value for the description member variable

    enum flags
    {
        notSet = (int8_t)0x7f           ///< Marker used to indicate the rehearsal sign is not set (not used)
    };

// Member Variables
protected:
    int8_t      m_letter;               ///< The letter used to uniquely identify the rehearsal sign (i.e. A, B, F, etc. - must be a capital letter)
    string    m_description;          ///< A description that indicates the passage the rehearsal sign is marking (i.e. Chorus, Intro, etc.)

// Construction/Destruction
public:
    RehearsalSign();
    RehearsalSign(int8_t letter, const char* description);
    RehearsalSign(const RehearsalSign& rehearsalSign);
    ~RehearsalSign();

// Operators
    const RehearsalSign& operator=(const RehearsalSign& rehearsalSign);
    bool operator==(const RehearsalSign& rehearsalSign) const;
    bool operator!=(const RehearsalSign& rehearsalSign) const;

// MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    string GetMFCClassName() const
        {return "CRehearsalSign2";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
        {return ((uint16_t)1);}

// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream);
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

// Letter Functions
public:
    /// Determines if a letter is valid
    /// @param letter The letter to validate
    /// @return True if the letter is valid, false if not
    static bool IsValidLetter(int8_t letter)
        {return (((letter >= 'A') && (letter <= 'Z')) || (letter == notSet));}
    bool SetLetter(int8_t letter);
    /// Gets the letter used to uniquely identify the rehearsal sign
    /// @return The letter used to uniquely identify the rehearsal sign
    int8_t GetLetter() const
        {return (m_letter);}

// Description functions
    /// Sets the rehearsal sign description
    /// @param description Description to set
    /// @return True if the description was set, false if not
    bool SetDescription(const string& description)
    {
        CHECK_THAT(!description.empty(), false);
        m_description = description;
        return true;
    }
    /// Gets the rehearsal sign description
    /// @return The rehearsal sign description
    string GetDescription() const
        {return (m_description);}

// Operations
    /// Determines if a RehearsalSign object is set (in use)
    /// @return True if the RehearsalSign object is set, false if not
    bool IsSet() const
        {return (GetLetter() != notSet);}
    /// Clears the rehearsal sign letter and description, and sets to not set
    void Clear()
        {SetLetter(notSet); m_description.clear();}
    string GetFormattedText() const;
};

#endif
