/////////////////////////////////////////////////////////////////////////////
// Name:            chordtext.h
// Purpose:         Stores and renders chord text
// Author:          Brad Larsen
// Modified by:
// Created:         Jan 3, 2005
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef CHORDTEXT_H
#define CHORDTEXT_H

#include "chordname.h"
#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders chord text
class ChordText : public PowerTabObject
{
// Constants
public:
    // Default Constants
    static const uint8_t DEFAULT_POSITION;       ///< Default value for the position member variable

// Member Variables
private:
    uint8_t          m_position;                 ///< Zero-based index of the position within the system where the chord text is anchored
    ChordName       m_chordName;	            ///< Chord name data (see ChordName class for details)

// Constructor/Destructor
public:
    ChordText();
    ChordText(uint32_t position, const ChordName& chordName);
    ChordText(const ChordText& chordText);
    ~ChordText();

// Operators
    const ChordText& operator=(const ChordText& chordText);
    bool operator==(const ChordText& chordText) const;
    bool operator!=(const ChordText& chordText) const;

// Serialization functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CChordText";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}

// Position Functions
    /// Determines whether a position is valid
    /// @param position Position to validate
    /// @return True if the position is valid, false if not
    static bool IsValidPosition(uint32_t position)
                {return (position <= 255);}
    /// Sets the position within the system where the chord text is anchored
    /// @param position Zero-based index within the system where the chord text
    /// is anchored
    /// @return True if the position was set, false if not
    bool SetPosition(uint32_t position)
    {
        PTB_CHECK_THAT(IsValidPosition(position), false);
        m_position = (uint8_t)position;
        return (true);
    }
    /// Gets the position within the system where the chord text is anchored
    /// @return The position within the system where the chord text is anchored
    uint32_t GetPosition() const
        {return (m_position);}

// Chord Name Functions
    /// Sets the chord name
    void SetChordName(const ChordName& chordName)
        {m_chordName = chordName;}
    /// Gets the chord name
    /// @return The chord name
    ChordName GetChordName() const
        {return (m_chordName);}
    /// Gets a referenced to the chord name
    /// @return A reference to the chord name
    ChordName& GetChordNameRef()
        {return (m_chordName);}
    /// Gets a constant reference to the chord name
    /// @return A constant reference to the chord name
    const ChordName& GetChordNameConstRef() const
        {return (m_chordName);}
    /// Gets a pointer to the chord name
    /// @return A pointer to the chord name
    ChordName* GetChordNamePtr()
        {return (&m_chordName);}

    inline std::string GetText() const { return m_chordName.GetText(); }
};

}

#endif // CHORDTEXT_H
