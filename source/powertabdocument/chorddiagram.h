/////////////////////////////////////////////////////////////////////////////
// Name:            chorddiagram.h
// Purpose:         Stores and renders a chord diagram
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 16, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHORDDIAGRAM_H__
#define __CHORDDIAGRAM_H__

#include "chordname.h"

#include "powertabobject.h"
#include "macros.h"

#include <vector>

/// Stores and renders a chord diagram
class ChordDiagram : public PowerTabObject
{
// Constants
    // Default Constants
    static const uint8_t DEFAULT_TOP_FRET;       ///< Default value for the top fret member variable

    // Top Fret Constants
    static const uint8_t MIN_TOP_FRET;           ///< Minimum allowed value for the top fret member variable
    static const uint8_t MAX_TOP_FRET;           ///< Maximum allowed value for the top fret member variable
    
    // Fret Number Constants
    static const uint8_t MIN_FRET_NUMBER;        ///< Minimum allowed fret number
    static const uint8_t MAX_FRET_NUMBER;        ///< Maximum allowed fret number

    enum flags
    {
        stringMuted = (uint8_t)0xfe,             ///< String is muted "x"
        notUsed     = (uint8_t)0xff              ///< String is not used
    };

// Member Variables
protected:
    ChordName       m_chordName;                ///< Chord name that appears above the chord diagram
    uint8_t          m_topFret;                  ///< Fret represented at the top of the chord diagram
    std::vector<uint8_t>     m_fretNumberArray;          ///< List of fret number offsets

// Constructor/Destructor
public:
    ChordDiagram();
    ChordDiagram(const ChordName& chordName, uint8_t topFret, uint8_t fretNumber1,
        uint8_t fretNumber2, uint8_t fretNumber3, uint8_t fretNumber4 = notUsed,
        uint8_t fretNumber5 = notUsed, uint8_t fretNumber6 = notUsed,
        uint8_t fretNumber7 = notUsed);
    ChordDiagram(uint8_t topFret, uint8_t fretNumber1, uint8_t fretNumber2,
        uint8_t fretNumber3, uint8_t fretNumber4 = notUsed,
        uint8_t fretNumber5 = notUsed, uint8_t fretNumber6 = notUsed,
        uint8_t fretNumber7 = notUsed);
    ChordDiagram(const ChordDiagram& chordDiagram);
    ~ChordDiagram();

// Operators
    const ChordDiagram& operator=(const ChordDiagram& chordDiagram);
    bool operator==(const ChordDiagram& chordDiagram) const;
    bool operator!=(const ChordDiagram& chordDiagram) const;
    // TODO: Add operator[]
    
// Serialize Functions
    bool Serialize(PowerTabOutputStream& stream);
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

// MFC Class Functions
public:    
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
        {return "CChordDiagram";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const                        
        {return ((uint16_t)1);}
    
// Chord Diagram Functions
    bool SetChordDiagram(const ChordName& chordName, uint8_t topFret,
        uint8_t fretNumber1, uint8_t fretNumber2, uint8_t fretNumber3,
        uint8_t fretNumber4 = notUsed, uint8_t fretNumber5 = notUsed,
        uint8_t fretNumber6 = notUsed, uint8_t fretNumber7 = notUsed);
    
// Chord Name Functions
    /// Sets the chord name
    /// @param chordName Chord name to set
    void SetChordName(const ChordName& chordName)       
        {m_chordName = chordName;}
    /// Gets the chord name
    /// @return The chord name
    ChordName GetChordName() const                      
        {return (m_chordName);}
    /// Gets a reference to the chord name
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
    
// Top Fret Functions
    /// Determines if a top fret value is valid
    /// @param topFret Top fret to validate
    /// @return True if the top fret value is valid, false if not
    static bool IsValidTopFret(uint8_t topFret)
        {return (topFret <= MAX_TOP_FRET);}
    /// Sets the top fret
    /// @param topFret Top fret to set
    /// @return True if the top fret was set, false if not
    bool SetTopFret(uint8_t topFret)
    {
        CHECK_THAT(IsValidTopFret(topFret), false);
        m_topFret = topFret;
        return (true);
    }
    /// Gets the top fret value
    /// @return The top fret
    uint8_t GetTopFret() const
        {return (m_topFret);}

// String Functions
    /// Determines if a string is valid
    /// @param string String to validate
    /// @return True if the string is valid, false if not
    bool IsValidString(uint32_t string) const
        {return (string < GetStringCount());}
    /// Gets the number of strings used in the chord diagram
    /// @return The number of strings used in the chord diagram
    size_t GetStringCount() const
        {return (m_fretNumberArray.size());}
    
// Fret Number Functions    
    /// Determines if a fret number is valid
    /// @param fretNumber Fret number to validate
    /// @return True if the fret number is valid, false if not
    static bool IsValidFretNumber(uint8_t fretNumber)
    {
        return ((fretNumber <= MAX_FRET_NUMBER) ||
            (fretNumber == stringMuted) ||
            (fretNumber == notUsed));
    }
    bool SetFretNumber(uint32_t string, uint8_t fretNumber);
    uint8_t GetFretNumber(uint32_t string) const;
protected:
    bool AddFretNumbers(uint8_t fretNumber1, uint8_t fretNumber2,
        uint8_t fretNumber3, uint8_t fretNumber4, uint8_t fretNumber5,
        uint8_t fretNumber6, uint8_t fretNumber7);
    /// Deletes the contents (and frees the memory) of the fret number array
    void DeleteFretNumberArrayContents()
        {m_fretNumberArray.clear();}

// Voicing Functions
public:
    bool IsSameVoicing(const ChordDiagram& chordDiagram) const;
    bool IsSameVoicing(uint8_t fretNumber1, uint8_t fretNumber2,
        uint8_t fretNumber3, uint8_t fretNumber4 = notUsed,
        uint8_t fretNumber5 = notUsed, uint8_t fretNumber6 = notUsed,
        uint8_t fretNumber7 = notUsed) const;
    
// Operations
    std::string GetSpelling() const;
};

#endif
