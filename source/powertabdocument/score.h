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

#ifndef SCORE_H
#define SCORE_H

#include "powertabobject.h"

#include <boost/shared_ptr.hpp>
#include <vector>

class Guitar;
class FloatingText;
class ChordDiagram;
class GuitarIn;
class TempoMarker;
class Dynamic;
class AlternateEnding;
class System;
class SystemLocation;
class Tuning;

/// Stores and renders a score
// Note: This is a class used to make life easier - it's not a class that exists in PTE v1.7
class Score : public PowerTabObject
{
public:
    static const uint8_t SYSTEM_SPACING = 50; ///< spacing between adjacent systems
    static const uint8_t MAX_NUM_GUITARS = 15; ///< maximum number of guitars allowed (limited by MIDI channels)

    // some useful typedefs for smart pointers
    typedef boost::shared_ptr<System> SystemPtr;
    typedef boost::shared_ptr<const System> SystemConstPtr;
    typedef boost::shared_ptr<Guitar> GuitarPtr;
    typedef boost::shared_ptr<const Guitar> GuitarConstPtr;
    typedef boost::shared_ptr<AlternateEnding> AlternateEndingPtr;
    typedef boost::shared_ptr<const AlternateEnding> AlternateEndingConstPtr;
    typedef boost::shared_ptr<ChordDiagram> ChordDiagramPtr;
    typedef boost::shared_ptr<Dynamic> DynamicPtr;
    typedef boost::shared_ptr<FloatingText> FloatingTextPtr;
    typedef boost::shared_ptr<GuitarIn> GuitarInPtr;
    typedef boost::shared_ptr<const GuitarIn> GuitarInConstPtr;
    typedef boost::shared_ptr<TempoMarker> TempoMarkerPtr;

// Constructor/Destructor
public:
    Score(const char* name);
    Score(const Score& score);

    void Init();

// Operators
    const Score& operator=(const Score& score);
    bool operator==(const Score& score) const;
    bool operator!=(const Score& score) const;

// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    // TODO - these should probably not be here, since this class was not part
    // of the original power tab file format
    std::string GetMFCClassName() const
    {
        return "Score";
    }
    uint16_t GetMFCClassSchema() const
    {
        return 1;
    }

// Guitar Functions
    bool IsValidGuitarIndex(uint32_t index) const;
    size_t GetGuitarCount() const;
    GuitarPtr GetGuitar(uint32_t index) const;

    bool InsertGuitar(GuitarPtr guitar);
    bool RemoveGuitar(size_t index);

    void SetTuning(GuitarPtr guitar, const Tuning& newTuning);

// Chord Diagram Functions
    bool IsValidChordDiagramIndex(uint32_t index) const;
    size_t GetChordDiagramCount() const;
    ChordDiagramPtr GetChordDiagram(uint32_t index) const;

// Floating Text Functions
    bool IsValidFloatingTextIndex(uint32_t index) const;
    size_t GetFloatingTextCount() const;
    FloatingTextPtr GetFloatingText(uint32_t index) const;

// Guitar In Functions
    bool IsValidGuitarInIndex(uint32_t index) const;
    size_t GetGuitarInCount() const;
    GuitarInPtr GetGuitarIn(uint32_t index) const;

// Tempo Marker Functions
    bool IsValidTempoMarkerIndex(uint32_t index) const;
    size_t GetTempoMarkerCount() const;
    TempoMarkerPtr GetTempoMarker(uint32_t index) const;

    void GetTempoMarkersInSystem(std::vector<TempoMarkerPtr>& tempoMarkers,
                                 SystemConstPtr system) const;
    TempoMarkerPtr FindTempoMarker(const SystemLocation& location) const;

    void InsertTempoMarker(TempoMarkerPtr marker);
    void RemoveTempoMarker(TempoMarkerPtr marker);

// Dynamic Functions
    bool IsValidDynamicIndex(uint32_t index) const;
    size_t GetDynamicCount() const;
    DynamicPtr GetDynamic(uint32_t index) const;
    void GetDynamicsInSystem(std::vector<DynamicPtr>& dynamics, SystemConstPtr system) const;
    DynamicPtr FindDynamic(uint32_t system, uint32_t staff, uint32_t positionIndex) const;

    void InsertDynamic(DynamicPtr dynamic);
    void RemoveDynamic(DynamicPtr dynamic);

// Alternate Ending Functions
    bool IsValidAlternateEndingIndex(uint32_t index) const;
    size_t GetAlternateEndingCount() const;
    AlternateEndingPtr GetAlternateEnding(uint32_t index) const;

    void GetAlternateEndingsInSystem(std::vector<AlternateEndingPtr>& endings, SystemConstPtr system) const;
    AlternateEndingPtr FindAlternateEnding(const SystemLocation& location) const;

    void InsertAlternateEnding(AlternateEndingPtr altEnding);
    void RemoveAlternateEnding(AlternateEndingPtr altEnding);

// System Functions
    bool IsValidSystemIndex(uint32_t index) const;
    size_t GetSystemCount() const;
    SystemPtr GetSystem(uint32_t index) const;

    bool RemoveSystem(size_t index);
    bool InsertSystem(SystemPtr system, size_t index);

    void UpdateSystemHeight(SystemPtr system);
    void UpdateAllSystemHeights();
    void ShiftFollowingSystems(SystemConstPtr system, const int heightDifference);

    void UpdateExtraSpacing(SystemPtr system);

    void ShiftForward(SystemPtr system, uint32_t positionIndex);
    void ShiftBackward(SystemPtr system, uint32_t positionIndex);

    int FindSystemIndex(const SystemConstPtr& system) const;

    void UpdateToVer2Structure();

    void MergeScore(const Score& otherScore);

    void FormatRehearsalSigns();

    std::string GetScoreName() const;

private:
    void PerformPositionShift(Score::SystemConstPtr system,
                              uint32_t positionIndex, int offset);

    std::vector<GuitarPtr> m_guitarArray; ///< Guitars used by the score.
    std::vector<ChordDiagramPtr> m_chordDiagramArray; ///< Chord diagrams used in the score.
    std::vector<FloatingTextPtr> m_floatingTextArray; ///< Floating text used in the score.
    std::vector<GuitarInPtr> m_guitarInArray; ///< Guitar Ins used in the score.
    std::vector<TempoMarkerPtr> m_tempoMarkerArray; ///< Tempo Markers used in the score.
    std::vector<DynamicPtr> m_dynamicArray; ///< Dynamic markers used in the score.
    std::vector<AlternateEndingPtr> m_alternateEndingArray; ///< Alternate endings used in the score.
    std::vector<SystemPtr> m_systemArray; ///< Systems used in the score.
    std::string m_scoreName; ///< Name of the score (e.g. Bass, Guitar, etc)
};

#endif // SCORE_H
