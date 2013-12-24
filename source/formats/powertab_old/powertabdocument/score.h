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

#include <memory>
#include <vector>

namespace PowerTabDocument {

class Guitar;
class FloatingText;
class ChordDiagram;
class GuitarIn;
class TempoMarker;
class Dynamic;
class AlternateEnding;
class System;
class Tuning;

/// Stores and renders a score
// Note: This is a class used to make life easier - it's not a class that exists in PTE v1.7
class Score : public PowerTabObject
{
public:
    static const uint8_t SYSTEM_SPACING = 50; ///< spacing between adjacent systems
    static const uint8_t MAX_NUM_GUITARS = 7; ///< maximum number of guitars allowed (limited by MIDI channels)

    // some useful typedefs for smart pointers
    typedef std::shared_ptr<System> SystemPtr;
    typedef std::shared_ptr<const System> SystemConstPtr;
    typedef std::shared_ptr<Guitar> GuitarPtr;
    typedef std::shared_ptr<const Guitar> GuitarConstPtr;
    typedef std::shared_ptr<AlternateEnding> AlternateEndingPtr;
    typedef std::shared_ptr<const AlternateEnding> AlternateEndingConstPtr;
    typedef std::shared_ptr<ChordDiagram> ChordDiagramPtr;
    typedef std::shared_ptr<Dynamic> DynamicPtr;
    typedef std::shared_ptr<FloatingText> FloatingTextPtr;
    typedef std::shared_ptr<GuitarIn> GuitarInPtr;
    typedef std::shared_ptr<const GuitarIn> GuitarInConstPtr;
    typedef std::shared_ptr<TempoMarker> TempoMarkerPtr;

// Constructor/Destructor
public:
    Score(const char* name);

// Serialization Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // TODO - these should probably not be here, since this class was not part
    // of the original power tab file format
    std::string GetMFCClassName() const override
    {
        return "Score";
    }
    uint16_t GetMFCClassSchema() const override
    {
        return 1;
    }

// Guitar Functions
    bool IsValidGuitarIndex(size_t index) const;
    size_t GetGuitarCount() const;
    GuitarPtr GetGuitar(size_t index) const;

// Chord Diagram Functions
    bool IsValidChordDiagramIndex(size_t index) const;
    size_t GetChordDiagramCount() const;
    ChordDiagramPtr GetChordDiagram(size_t index) const;

// Floating Text Functions
    bool IsValidFloatingTextIndex(size_t index) const;
    size_t GetFloatingTextCount() const;
    FloatingTextPtr GetFloatingText(size_t index) const;

// Guitar In Functions
    bool IsValidGuitarInIndex(size_t index) const;
    size_t GetGuitarInCount() const;
    GuitarInPtr GetGuitarIn(size_t index) const;
    void GetGuitarInsInSystem(std::vector<GuitarInPtr>& guitarIns,
                              SystemConstPtr system) const;

// Tempo Marker Functions
    bool IsValidTempoMarkerIndex(size_t index) const;
    size_t GetTempoMarkerCount() const;
    TempoMarkerPtr GetTempoMarker(size_t index) const;

    void GetTempoMarkersInSystem(std::vector<TempoMarkerPtr>& tempoMarkers,
                                 SystemConstPtr system) const;

// Dynamic Functions
    bool IsValidDynamicIndex(size_t index) const;
    size_t GetDynamicCount() const;
    DynamicPtr GetDynamic(size_t index) const;
    void GetDynamicsInSystem(std::vector<DynamicPtr>& dynamics, SystemConstPtr system) const;

// Alternate Ending Functions
    bool IsValidAlternateEndingIndex(size_t index) const;
    size_t GetAlternateEndingCount() const;
    AlternateEndingPtr GetAlternateEnding(size_t index) const;

    void GetAlternateEndingsInSystem(std::vector<AlternateEndingPtr>& endings, SystemConstPtr system) const;

// System Functions
    bool IsValidSystemIndex(size_t index) const;
    size_t GetSystemCount() const;
    SystemPtr GetSystem(size_t index) const;

    int FindSystemIndex(const SystemConstPtr& system) const;

    std::string GetScoreName() const;

private:
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

}

#endif // SCORE_H
