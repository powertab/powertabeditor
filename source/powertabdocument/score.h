/////////////////////////////////////////////////////////////////////////////
// Name:            score.h
// Purpose:         Stores and renders a score
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __SCORE_H__
#define __SCORE_H__

#include "guitar.h"
#include "chorddiagram.h"
#include "floatingtext.h"
#include "guitarin.h"
#include "tempomarker.h"
#include "dynamic.h"
#include "alternateending.h"
#include "system.h"

/// Stores and renders a score
// Note: This is a class used to make life easier - it's not a class that exists in PTE v1.7
class Score : public PowerTabObject
{
// Member Variables
public:
    std::vector<Guitar*>                 m_guitarArray;              ///< Guitars used by the score
    std::vector<ChordDiagram*>           m_chordDiagramArray;        ///< Chord diagrams used in the score
    std::vector<FloatingText*>           m_floatingTextArray;        ///< Floating text used in the score
    std::vector<GuitarIn*>               m_guitarInArray;            ///< Guitar Ins used in the score
    std::vector<TempoMarker*>            m_tempoMarkerArray;         ///< Tempo Markers used in the score
    std::vector<Dynamic*>                m_dynamicArray;             ///< Dynamic markers used in the score
    std::vector<AlternateEnding*>        m_alternateEndingArray;     ///< Alternate endings used in the score
    std::vector<System*>                 m_systemArray;              ///< Systems used in the score

// Constructor/Destructor
public:
    Score();
    Score(const Score& score);
    ~Score();

// Operators
    const Score& operator=(const Score& score);
    bool operator==(const Score& score) const;
    bool operator!=(const Score& score) const;

// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream);
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

        // TODO - these should probably not be here, since this class was not part
        // of the original power tab file format
        string GetMFCClassName() const
        {
            return "Score";
        }
        uint16_t GetMFCClassSchema() const
        {
            return 1;
        }

// Guitar Functions
public:
    /// Determines if a guitar index is valid
    /// @param index guitar index to validate
    /// @return True if the guitar index is valid, false if not
    bool IsValidGuitarIndex(uint32_t index) const
        {return (index < GetGuitarCount());}
    /// Gets the number of guitars in the score
    /// @return The number of guitars in the score
    size_t GetGuitarCount() const
        {return (m_guitarArray.size());}
    /// Gets the nth guitar in the score
    /// @param index Index of the guitar to get
    /// @return The nth guitar in the score
    Guitar* GetGuitar(uint32_t index) const
    {
        CHECK_THAT(IsValidGuitarIndex(index), NULL);
        return (m_guitarArray[index]);
    }

// Chord Diagram Functions
    /// Determines if a guitar index is valid
    /// @param index ChordDiagram index to validate
    /// @return True if the guitar index is valid, false if not
    bool IsValidChordDiagramIndex(uint32_t index) const
        {return (index < GetChordDiagramCount());}
    /// Gets the number of guitars in the score
    /// @return The number of guitars in the score
    size_t GetChordDiagramCount() const
        {return (m_chordDiagramArray.size());}
    /// Gets the nth guitar in the score
    /// @param index Index of the guitar to get
    /// @return The nth guitar in the score
    ChordDiagram* GetChordDiagram(uint32_t index) const
    {
        CHECK_THAT(IsValidChordDiagramIndex(index), NULL);
        return (m_chordDiagramArray[index]);
    }

// Floating Text Functions
    /// Determines if a floating text index is valid
    /// @param index floating text index to validate
    /// @return True if the floating text index is valid, false if not
    bool IsValidFloatingTextIndex(uint32_t index) const
        {return (index < GetFloatingTextCount());}
    /// Gets the number of floating text items in the score
    /// @return The number of floating text items in the score
    size_t GetFloatingTextCount() const
        {return (m_floatingTextArray.size());}
    /// Gets the nth floating text in the score
    /// @param index Index of the floating text to get
    /// @return The nth floating text in the score
    FloatingText* GetFloatingText(uint32_t index) const
    {
        CHECK_THAT(IsValidFloatingTextIndex(index), NULL);
        return (m_floatingTextArray[index]);
    }

// Guitar In Functions
    /// Determines if a guitar in index is valid
    /// @param index guitar in index to validate
    /// @return True if the guitar in index is valid, false if not
    bool IsValidGuitarInIndex(uint32_t index) const
        {return (index < GetGuitarInCount());}
    /// Gets the number of guitar ins in the score
    /// @return The number of guitar ins in the score
    size_t GetGuitarInCount() const
        {return (m_guitarInArray.size());}
    /// Gets the nth guitar in in the score
    /// @param index Index of the guitar in to get
    /// @return The nth guitar in in the score
    GuitarIn* GetGuitarIn(uint32_t index) const
    {
        CHECK_THAT(IsValidGuitarInIndex(index), NULL);
        return (m_guitarInArray[index]);
    }

// Tempo Marker Functions
    /// Determines if a tempo marker index is valid
    /// @param index tempo marker index to validate
    /// @return True if the tempo marker index is valid, false if not
    bool IsValidTempoMarkerIndex(uint32_t index) const
        {return (index < GetTempoMarkerCount());}
    /// Gets the number of tempo markers in the score
    /// @return The number of tempo markers in the score
    size_t GetTempoMarkerCount() const
        {return (m_tempoMarkerArray.size());}
    /// Gets the nth tempo marker in the score
    /// @param index Index of the tempo marker to get
    /// @return The nth tempo marker in the score
    TempoMarker* GetTempoMarker(uint32_t index) const
    {
        CHECK_THAT(IsValidTempoMarkerIndex(index), NULL);
        return (m_tempoMarkerArray[index]);
    }

    void GetTempoMarkersInSystem(std::vector<TempoMarker*>& tempoMarkers, System* system) const;

// Dynamic Functions
    /// Determines if a dynamic index is valid
    /// @param index dynamic index to validate
    /// @return True if the dynamic index is valid, false if not
    bool IsValidDynamicIndex(uint32_t index) const
        {return (index < GetDynamicCount());}
    /// Gets the number of dynamics in the score
    /// @return The number of dynamics in the score
    size_t GetDynamicCount() const
        {return (m_dynamicArray.size());}
    /// Gets the nth dynamic in the score
    /// @param index Index of the dynamic to get
    /// @return The nth dynamic in the score
    Dynamic* GetDynamic(uint32_t index) const
    {
        CHECK_THAT(IsValidDynamicIndex(index), NULL);
        return (m_dynamicArray[index]);
    }

// Alternate Ending Functions
    /// Determines if a alternate ending index is valid
    /// @param index alternate ending index to validate
    /// @return True if the alternate ending index is valid, false if not
    bool IsValidAlternateEndingIndex(uint32_t index) const
        {return (index < GetAlternateEndingCount());}
    /// Gets the number of alternate endings in the score
    /// @return The number of alternate endings in the score
    size_t GetAlternateEndingCount() const
        {return (m_alternateEndingArray.size());}
    /// Gets the nth alternate ending in the score
    /// @param index Index of the alternate ending to get
    /// @return The nth alternate ending in the score
    AlternateEnding* GetAlternateEnding(uint32_t index) const
    {
        CHECK_THAT(IsValidAlternateEndingIndex(index), NULL);
        return (m_alternateEndingArray[index]);
    }

    void GetAlternateEndingsInSystem(std::vector<AlternateEnding*>& endings, System *system) const;

// System Functions
    /// Determines if a system index is valid
    /// @param index system index to validate
    /// @return True if the system index is valid, false if not
    bool IsValidSystemIndex(uint32_t index) const
        {return (index < GetSystemCount());}
    /// Gets the number of systems in the score
    /// @return The number of systems in the score
    size_t GetSystemCount() const
        {return (m_systemArray.size());}
    /// Gets the nth system in the score
    /// @param index Index of the system to get
    /// @return The nth system in the score
    System* GetSystem(uint32_t index) const
    {
        CHECK_THAT(IsValidSystemIndex(index), NULL);
        return (m_systemArray[index]);
    }

    void UpdateSystemHeight(System* system);
    void UpdateExtraSpacing(System* system);

    int FindSystemIndex(System* system) const;

    void UpdateToVer2Structure();

};

#endif
