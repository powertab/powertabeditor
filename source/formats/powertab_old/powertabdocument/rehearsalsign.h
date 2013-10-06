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

#ifndef POWERTABDOCUMENT_REHEARSAL_SIGN_H
#define POWERTABDOCUMENT_REHEARSAL_SIGN_H

#include "powertabobject.h"

namespace PowerTabDocument {

/// Stores and renders Rehearsal Sign symbols
class RehearsalSign : public PowerTabObject
{
public:
    static const int8_t         DEFAULT_LETTER;         ///< Default value for the rehearsal sign letter
    static const std::string    DEFAULT_DESCRIPTION;    ///< Default value for the rehearsal sign description

    enum flags
    {
        notSet = 0x7f   ///< Marker used to indicate the rehearsal sign is not set (not used)
    };

public:
    RehearsalSign();
    RehearsalSign(int8_t letter, const std::string& description);

    bool operator==(const RehearsalSign& rehearsalSign) const;
    bool operator!=(const RehearsalSign& rehearsalSign) const;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {
        return "CRehearsalSign2";
    }

    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {
        return 1;
    }

    // Serialization Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // Letter Functions
    static bool IsValidLetter(int8_t letter);
    bool SetLetter(int8_t letter);
    int8_t GetLetter() const;

    // Description functions
    bool SetDescription(const std::string& description);
    std::string GetDescription() const;

    // Operations
    bool IsSet() const;
    void Clear();

    std::string GetFormattedText() const;

    // Member Variables
private:
    /// The letter used to uniquely identify the rehearsal sign
    /// (i.e. A, B, F, etc. - must be a capital letter).
    int8_t      m_letter;

    /// A description that indicates the passage the rehearsal sign is
    /// marking (i.e. Chorus, Intro, etc.).
    std::string m_description;

};

}

#endif // POWERTABDOCUMENT_REHEARSAL_SIGN_H
