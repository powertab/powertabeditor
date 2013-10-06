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

#ifndef CHORDDIAGRAM_H
#define CHORDDIAGRAM_H

#include "chordname.h"

#include "powertabobject.h"
#include "macros.h"

#include <vector>

namespace PowerTabDocument {

/// Stores and renders a chord diagram
class ChordDiagram : public PowerTabObject
{
public:
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
private:
    ChordName               m_chordName;                ///< Chord name that appears above the chord diagram
    uint8_t                 m_topFret;                  ///< Fret represented at the top of the chord diagram
    std::vector<uint8_t>    m_fretNumberArray;          ///< List of fret number offsets

    // Constructor/Destructor
public:
    ChordDiagram();
    ChordDiagram(const ChordName& chordName, uint8_t topFret,
                 const std::vector<uint8_t>& fretNumbers);
    ChordDiagram(uint8_t topFret, const std::vector<uint8_t>& fretNumbers);
    ChordDiagram(const ChordDiagram& chordDiagram);

    // Operators
    const ChordDiagram& operator=(const ChordDiagram& chordDiagram);
    bool operator==(const ChordDiagram& chordDiagram) const;
    bool operator!=(const ChordDiagram& chordDiagram) const;
    // TODO: Add operator[]
    
    // Serialize Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {return "CChordDiagram";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {return ((uint16_t)1);}
    
    // Chord Diagram Functions
    bool SetChordDiagram(const ChordName& chordName, uint8_t topFret,
                         const std::vector<uint8_t>& fretNumbers);
    
    // Chord Name Functions
    void SetChordName(const ChordName& chordName);
    const ChordName& GetChordName() const;
    ChordName& GetChordName();

    // Top Fret Functions
    static bool IsValidTopFret(uint8_t topFret);
    bool SetTopFret(uint8_t topFret);
    uint8_t GetTopFret() const;

    // String Functions
    bool IsValidString(uint32_t string) const;
    size_t GetStringCount() const;
    
    // Fret Number Functions
    static bool IsValidFretNumber(uint8_t fretNumber);
    bool SetFretNumber(uint32_t string, uint8_t fretNumber);
    uint8_t GetFretNumber(uint32_t string) const;

private:
    bool AddFretNumbers(const std::vector<uint8_t>& fretNumbers);

    // Voicing Functions
public:
    bool IsSameVoicing(const ChordDiagram& chordDiagram) const;
    bool IsSameVoicing(const std::vector<uint8_t>& fretNumbers) const;
    
    // Operations
    std::string GetSpelling() const;
};

}

#endif // CHORDDIAGRAM_H
