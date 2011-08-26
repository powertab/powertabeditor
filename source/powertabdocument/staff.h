/////////////////////////////////////////////////////////////////////////////
// Name:            staff.h
// Purpose:         Stores and renders a staff
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef STAFF_H
#define STAFF_H

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "note.h"

class Position;
class System;
class Barline;
class KeySignature;
class Tuning;

/// Stores and renders a staff
class Staff : public PowerTabObject
{
    // Constants
public:
    // Default Constants
    static const uint8_t DEFAULT_DATA;                                   ///< Default value for the data member variable
    static const uint8_t DEFAULT_CLEF;                                   ///< Default clef type
    static const uint8_t DEFAULT_TABLATURE_STAFF_TYPE;                   ///< Default tablature staff type
    static const uint8_t DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING;  ///< Default value for the standard notation staff above spacing member variable
    static const uint8_t DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING;  ///< Default value for the standard notation staff below spacing member variable
    static const uint8_t DEFAULT_SYMBOL_SPACING;                         ///< Default value for the symbol spacing member variable
    static const uint8_t DEFAULT_TABLATURE_STAFF_BELOW_SPACING;          ///< Default value for the tablature staff below spacing member variable
    static const uint8_t STD_NOTATION_LINE_SPACING;
    static const uint8_t STD_NOTATION_STAFF_TYPE;
    static const uint8_t STAFF_BORDER_SPACING; /// padding around top and bottom of staves
    static const uint8_t TAB_SYMBOL_HEIGHT; /// height allocated for drawing a tab symbol 

    // Clef Constants
    static const uint8_t TREBLE_CLEF;                                ///< Treble clef
    static const uint8_t BASS_CLEF;                                  ///< Bass clef

    // Tablature Staff Type Constants
    static const uint8_t MIN_TABULATURE_STAFF_TYPE;                   ///< Minimum allowed value for tablature staff type
    static const uint8_t MAX_TABULATURE_STAFF_TYPE;                   ///< Maximum allowed value for tablature staff type

    enum flags
    {
        clefMask                    = 0xf0,                 ///< Mask used to retrieve the clef type
        tablatureStaffTypeMask      = 0xf                   ///< Mask used to retrieve the tablature type (3 - 7 string)
    };

    enum { NUM_STAFF_VOICES = 2 }; ///< Number of voices in a staff

    // Member Variables
private:
    uint8_t m_data;                                 ///< Top 4 bits = clef type, bottom 4 bits = tablature type
    uint8_t m_standardNotationStaffAboveSpacing;    ///< Amount of space alloted from the top line of the standard notation staff
    uint8_t m_standardNotationStaffBelowSpacing;    ///< Amount of space alloted from the last line of the standard notation staff
    uint8_t m_symbolSpacing;                        ///< Amount of space alloted for symbols located between the standard notation and tablature staff
    uint8_t m_tablatureStaffBelowSpacing;           ///< Amount of space alloted from the last line of the tablature staff
    bool m_isShown;

public:
    boost::array<std::vector<Position*>, NUM_STAFF_VOICES> positionArrays; ///< collection of position arrays, one per voice

    // Constructor/Destructor
public:
    Staff();
    Staff(uint8_t tablatureStaffType, uint8_t clef);
    Staff(const Staff& staff);
    ~Staff();

    // Operators
    const Staff& operator=(const Staff& staff);
    bool operator==(const Staff& staff) const;
    bool operator!=(const Staff& staff) const;

    // Serialize Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    Staff* CloneObject() const;

public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
    {
        return "CStaff";
    }

    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
    {
        return 1;
    }

    bool IsShown() const;
    void SetShown(bool set = true);

    // Clef Functions
    static bool IsValidClef(uint8_t clef);
    bool SetClef(uint8_t type);
    uint8_t GetClef() const;
    void CalculateClef(const Tuning& tuning);

    // Tablature Staff Type Functions
    static bool IsValidTablatureStaffType(uint8_t type);
    bool SetTablatureStaffType(uint8_t type);
    uint8_t GetTablatureStaffType() const;

    // Standard Notation Staff Above Spacing Functions
    void SetStandardNotationStaffAboveSpacing(uint8_t spacing);
    uint8_t GetStandardNotationStaffAboveSpacing() const;

    // Standard Notation Staff Below Spacing Functions
    void SetStandardNotationStaffBelowSpacing(uint8_t spacing);
    uint8_t GetStandardNotationStaffBelowSpacing() const;

    // Symbol Spacing Functions
    void SetSymbolSpacing(uint8_t spacing);
    uint8_t GetSymbolSpacing() const;

    // Tablature Staff Below Spacing Functions
    void SetTablatureStaffBelowSpacing(uint8_t spacing);
    uint8_t GetTablatureStaffBelowSpacing() const;

    static bool IsValidVoice(uint32_t voice);

    // Position Functions
    bool IsValidPositionIndex(uint32_t voice, uint32_t index) const;

    size_t GetPositionCount(uint32_t voice) const;
    Position* GetPosition(uint32_t voice, uint32_t index) const;

    bool InsertPosition(uint32_t voice, Position* position);
    bool RemovePosition(uint32_t voice, uint32_t index);

    Position* GetLastPosition() const;
    Position* GetPositionByPosition(uint32_t voice, uint32_t index) const;
    size_t GetIndexOfPosition(uint32_t voice, const Position* position) const;
    size_t GetIndexOfNextPosition(uint32_t voice, boost::shared_ptr<const System> system,
                                  const Position* position) const;

    bool IsOnlyPositionInBar(const Position* position, boost::shared_ptr<const System> system) const;

    bool CanHammerOn(const Position* position, const Note* note) const;
    bool CanPullOff(const Position* position, const Note* note) const;
    bool CanTieNote(const Position* position, const Note* note) const;
    bool CanSlideBetweenNotes(const Position* position, const Note* note) const;
    int8_t GetSlideSteps(const Position* position, const Note* note) const;

    int GetHeight() const;
    
    void GetPositionsInRange(std::vector<Position*>& positionsInRange, uint32_t voice, size_t startPos, size_t endPos);
    void CalculateBeamingForBar(boost::shared_ptr<const Barline> startBar,
                                boost::shared_ptr<const Barline> endBar);
    void CalculateBeamingForGroup(std::vector<Position*>& positions);

    void UpdateTabNumber(Position *position, Note *note, uint8_t fretNumber);
    void UpdateNote(Position *prevPosition, Note *previousNote, Note *nextNote);
    
    int GetNoteLocation(const Note* note, const KeySignature& activeKeySig,
                        const Tuning& tuning) const;

    enum SearchDirection
    {
        NextNote = 1,
        PrevNote = -1
    };

    Note* GetAdjacentNoteOnString(SearchDirection searchDirection, const Position* position,
                                  const Note* note, uint32_t voice = 0) const;

protected:
    /// Compares the fret numbers of two consecutive notes on the same string,
    /// using the given comparision function (binary predicate)
    template<typename FretComparison>
    bool CompareWithNote(SearchDirection searchDirection, const Position* position,
                         const Note* note, FretComparison comp) const
    {
        const Note* nextNote = GetAdjacentNoteOnString(searchDirection, position, note);
        
        // check if a note was found on the same string in the next position,
        // and if the fret number comparision is satisfied
        return (nextNote != NULL && comp(note->GetFretNumber(), nextNote->GetFretNumber()));
    }
};

#endif // STAFF_H
