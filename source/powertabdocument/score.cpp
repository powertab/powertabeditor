/////////////////////////////////////////////////////////////////////////////
// Name:            score.cpp
// Purpose:         Stores and renders a score
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "score.h"

#include "staff.h"
#include "barline.h"
#include "powertabfileheader.h"
#include "system.h"
#include "guitar.h"
#include "chorddiagram.h"
#include "floatingtext.h"
#include "guitarin.h"
#include "tempomarker.h"
#include "alternateending.h"
#include "position.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"
#include "direction.h"
#include "dynamic.h"
#include "systemlocation.h"
#include "common.h"
#include "layout.h"

#include <map>
#include <bitset>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/iterator/indirect_iterator.hpp>

/// Default Constructor
Score::Score()
{
}

/// Copy Constructor
Score::Score(const Score& score)
{
    *this = score;
}

// Operators
/// Assignment Operator
const Score& Score::operator=(const Score& score)
{
    // Check for assignment to self
    if (this != &score)
    {
        deepCopy(score.m_guitarArray, m_guitarArray);
        deepCopy(score.m_chordDiagramArray, m_chordDiagramArray);
        deepCopy(score.m_floatingTextArray, m_floatingTextArray);
        deepCopy(score.m_guitarInArray, m_guitarInArray);
        deepCopy(score.m_tempoMarkerArray, m_tempoMarkerArray);
        deepCopy(score.m_dynamicArray, m_dynamicArray);
        deepCopy(score.m_alternateEndingArray, m_alternateEndingArray);
        deepCopy(score.m_systemArray, m_systemArray);
    }
    return *this;
}

/// Equality Operator
bool Score::operator==(const Score& score) const
{
    return (isDeepEqual(m_guitarArray, score.m_guitarArray) &&
            isDeepEqual(m_chordDiagramArray, score.m_chordDiagramArray) &&
            isDeepEqual(m_floatingTextArray, score.m_floatingTextArray) &&
            isDeepEqual(m_guitarInArray, score.m_guitarInArray) &&
            isDeepEqual(m_tempoMarkerArray, score.m_tempoMarkerArray) &&
            isDeepEqual(m_dynamicArray, score.m_dynamicArray) &&
            isDeepEqual(m_alternateEndingArray, score.m_alternateEndingArray) &&
            isDeepEqual(m_systemArray, score.m_systemArray));
}

/// Inequality Operator
bool Score::operator!=(const Score& score) const
{
    return (!operator==(score));
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Score::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    stream.WriteVector(m_guitarArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_chordDiagramArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_floatingTextArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_guitarInArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_tempoMarkerArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_dynamicArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_alternateEndingArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_systemArray);
    CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Score::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    stream.ReadVector(m_guitarArray, version);
    stream.ReadVector(m_chordDiagramArray, version);
    stream.ReadVector(m_floatingTextArray, version);
    stream.ReadVector(m_guitarInArray, version);
    stream.ReadVector(m_tempoMarkerArray, version);
    stream.ReadVector(m_dynamicArray, version);
    stream.ReadVector(m_alternateEndingArray, version);
    stream.ReadVector(m_systemArray, version);

    if (version == PowerTabFileHeader::FILEVERSION_1_7)
    {
        UpdateToVer2Structure();
    }

    return true;
}

/// Removes the system at the specified index
bool Score::RemoveSystem(size_t index)
{
    CHECK_THAT(IsValidSystemIndex(index), false);

    SystemConstPtr system = m_systemArray.at(index);

    // adjust height of following systems
    ShiftFollowingSystems(system, -(system->GetRect().GetHeight() + SYSTEM_SPACING));

    m_systemArray.erase(m_systemArray.begin() + index);

    return true;
}

/// Inserts a system into the score at the specified location
bool Score::InsertSystem(SystemPtr system, size_t index)
{
    CHECK_THAT(IsValidSystemIndex(index) || index == GetSystemCount(), false);

    m_systemArray.insert(m_systemArray.begin() + index, system);

    // ensure the system has enough staves for each guitar
    if (system->GetStaffCount() != GetGuitarCount())
    {
        std::vector<uint8_t> staffSizes;
        // make a list of the number of strings for each guitar
        using namespace std::placeholders;
        std::transform(m_guitarArray.begin(), m_guitarArray.end(),
                       std::back_inserter(staffSizes),
                       std::bind(&Guitar::GetStringCount, _1));
        system->Init(staffSizes);
    }

    system->CalculateHeight();
    ShiftFollowingSystems(system, system->GetRect().GetHeight() + SYSTEM_SPACING);

    return true;
}

// Remove guitar-ins and adjust the score structure, so that each guitar corresponds to its own staff
void Score::UpdateToVer2Structure()
{
    using std::pair;
    using std::multimap;

    multimap<uint32_t, GuitarInConstPtr> guitarInMap; // map systems to Guitar In symbols
    multimap<uint32_t, uint32_t> guitarToStaffMap; // map guitars to (possibly multiple) staves

    // find all guitar in symbols and the system they occur in
    for (uint32_t i=0; i < m_guitarInArray.size(); i++)
    {
        GuitarInConstPtr gtr_in = m_guitarInArray.at(i);
        guitarInMap.insert(pair<uint32_t, GuitarInConstPtr>(gtr_in->GetSystem(), gtr_in));
    }

    for (uint32_t i=0; i < m_systemArray.size(); i++) // iterate through all systems
    {
        // if there are guitar ins in the system, readjust the Guitar->Staff mapping
        // otherwise, let it continue from the previous system
        if ((guitarInMap.count(i)) >= 1)
        {
            //std::cerr << "In System " << i << std::endl;
            auto range = guitarInMap.equal_range(i);
            for (auto i = range.first; i != range.second; ++i)
            {
                GuitarInConstPtr currentGuitarIn = i->second;
                // only readjust the Guitar->Staff mapping if we're actually changing the staff guitar, not just the rhythm slash
                if (currentGuitarIn->HasStaffGuitarsSet())
                {
                    guitarToStaffMap.erase(currentGuitarIn->GetStaff());
                }
                //std::cerr << "At Position: " << i->second->GetPosition() << std::endl;

                // Map the new guitars to the appropriate staves
                std::bitset<8> guitarBitmap(currentGuitarIn->GetStaffGuitars());
                for (uint8_t gtr = 0; gtr < 8; gtr++)
                {
                    if (guitarBitmap.test(gtr) == true)
                    {
                        guitarToStaffMap.insert(pair<uint32_t, uint32_t>(currentGuitarIn->GetStaff(), gtr));
                        //std::cerr << "Guitar In: " << (int)gtr + 1 << std::endl;
                    }
                }
            }
        }

        SystemPtr currentSystem = m_systemArray.at(i);

        std::vector<System::StaffPtr> newStaves(m_guitarArray.size()); // one staff per guitar

        for (uint32_t j=0; j < currentSystem->GetStaffCount(); j++) // go through staves
        {
            System::StaffPtr currentStaff = currentSystem->GetStaff(j);

            auto range = guitarToStaffMap.equal_range(j); // find guitars for this staff
            for (auto k = range.first; k != range.second; ++k)
            {
                newStaves[k->second].reset(currentStaff->CloneObject());
            }
        }

        for (uint32_t m=0; m < newStaves.size(); m++) // insert blank staves for any guitars that aren't currently used
        {
            if (!newStaves.at(m))
            {
                System::StaffPtr newStaff = std::make_shared<Staff>();

                std::vector<System::BarlineConstPtr> barlines;
                currentSystem->GetBarlines(barlines);

                // just insert a whole rest after each barline, except for the last one
                for (size_t n = 0; n < barlines.size() - 1; n++)
                {
                    System::BarlineConstPtr barline = barlines.at(n);
                    Position* newPosition = new Position(barline->GetPosition() + 1, 1, 0);
                    newPosition->SetRest(true);
                    newStaff->positionArrays[0].push_back(newPosition);
                }

                newStaves[m] = newStaff;
            }
        }

        // clear out the old staves, and swap in the new ones
        currentSystem->m_staffArray = newStaves;

        // readjust locations / heights
        currentSystem->CalculateHeight();
        if (i != 0) // adjust the location of every staff after the first
        {
            Rect currentRect = currentSystem->GetRect();
            Rect prevRect = m_systemArray.at(i-1)->GetRect();
            currentRect.SetY(prevRect.GetBottom() + SYSTEM_SPACING);
            currentSystem->SetRect(currentRect);
        }

        // recalculate the beaming for the member staves, using the current beaming algorithm
        currentSystem->CalculateBeamingForStaves();
    }
}

// Finds the index of a system within the score
int Score::FindSystemIndex(SystemConstPtr system) const
{
    auto result = std::find(m_systemArray.begin(), m_systemArray.end(), system);
    return std::distance(m_systemArray.begin(), result);
}

// Helper function for GetAlternateEndingsInSystem, GetTempoMarkersInSystem, etc
template<class Symbol>
void GetSymbolsInSystem(std::vector<Symbol>& output, const std::vector<Symbol>& symbolList, const uint32_t systemIndex)
{
    output.clear();

    for (size_t i = 0; i < symbolList.size(); i++)
    {
        Symbol symbol = symbolList.at(i);
        if (symbol->GetSystem() == systemIndex)
        {
            output.push_back(symbol);
        }
    }
}

/// Finds all of the tempo markers that are in the given system
void Score::GetTempoMarkersInSystem(std::vector<TempoMarkerPtr>& tempoMarkers, SystemConstPtr system) const
{
    GetSymbolsInSystem(tempoMarkers, m_tempoMarkerArray, FindSystemIndex(system));
}

void Score::GetAlternateEndingsInSystem(std::vector<AlternateEndingPtr>& endings, SystemConstPtr system) const
{
    GetSymbolsInSystem(endings, m_alternateEndingArray, FindSystemIndex(system));
}

/// Returns all of the dynamics located in the given system
void Score::GetDynamicsInSystem(std::vector<Score::DynamicPtr> &dynamics, Score::SystemConstPtr system) const
{
    GetSymbolsInSystem(dynamics, m_dynamicArray, FindSystemIndex(system));
}

/// Returns the alternate ending at the given location, or NULL if it is not found
Score::AlternateEndingPtr Score::FindAlternateEnding(const SystemLocation& location) const
{
    BOOST_FOREACH(AlternateEndingPtr ending, m_alternateEndingArray)
    {
        if (location == SystemLocation(ending->GetSystem(), ending->GetPosition()))
        {
            return ending;
        }
    }

    return AlternateEndingPtr();
}

/// Updates the height of the system, and adjusts the height of subsequent systems as necessary
/// @param system The system to update the height of
void Score::UpdateSystemHeight(SystemPtr system)
{
    Layout::CalculateStdNotationHeight(this, system);

    // Store the original height, recalculate the height, then find the height difference
    
    const int originalHeight = system->GetRect().GetHeight();
    
    for (size_t i = 0; i < system->GetStaffCount(); i++)
    {
        System::StaffPtr currentStaff = system->GetStaff(i);
        Layout::CalculateTabStaffBelowSpacing(currentStaff);
        Layout::CalculateSymbolSpacing(this, system, currentStaff);
    }

    system->CalculateBeamingForStaves();
    
    UpdateExtraSpacing(system);
    
    system->CalculateHeight();
    const int spacingDifference = system->GetRect().GetHeight() - originalHeight;

    ShiftFollowingSystems(system, spacingDifference);
}

/// Shifts all following systems by the given height difference
void Score::ShiftFollowingSystems(SystemConstPtr system, const int heightDifference)
{
    uint32_t systemIndex = FindSystemIndex(system) + 1;
    for (; systemIndex < GetSystemCount(); systemIndex++)
    {
        SystemPtr currentSystem = m_systemArray.at(systemIndex);

        Rect rect = currentSystem->GetRect();
        rect.SetTop(rect.GetTop() + heightDifference);
        currentSystem->SetRect(rect);
    }
}

/// Updates the extra spacing at the top of the system (for rehearsal signs, etc)
void Score::UpdateExtraSpacing(SystemPtr system)
{
    // get list of tempo markers
    std::vector<TempoMarkerPtr> markers;
    GetTempoMarkersInSystem(markers, system);
    
    // get list of alternate endings
    std::vector<AlternateEndingPtr> endings;
    GetAlternateEndingsInSystem(endings, system);

    // Find how many different items occur in the system (i.e. is there at least one rehearsal sign, tempo marker, etc)
    const int numItems = system->HasRehearsalSign() +
            (system->GetChordTextCount() > 0) +
            !markers.empty() +
            system->MaxDirectionSymbolCount() +
            !endings.empty();
    
    // Each type of item gets a line to itself
    system->SetExtraSpacing(numItems * System::SYSTEM_SYMBOL_SPACING);
}

void Score::Init()
{
    // create a guitar
    GuitarPtr guitar = std::make_shared<Guitar>();
    guitar->GetTuning().SetToStandard();
    m_guitarArray.push_back(guitar);

    // create a system and initialize the staves
    std::vector<uint8_t> staffSizes;
    staffSizes.push_back(guitar->GetStringCount());

    SystemPtr newSystem(new System);
    newSystem->Init(staffSizes);
    InsertSystem(newSystem, 0);
}

/// Inserts a guitar into the score, and creates a corresponding staff
bool Score::InsertGuitar(GuitarPtr guitar)
{
    CHECK_THAT(m_guitarArray.size() < MAX_NUM_GUITARS, false);

    m_guitarArray.push_back(guitar);
    guitar->SetNumber(m_guitarArray.size() - 1);

    // add staff to each system
    for (size_t i = 0; i < m_systemArray.size(); i++)
    {
        SystemPtr system = m_systemArray[i];
        system->m_staffArray.push_back(std::make_shared<Staff>(guitar->GetStringCount(), Staff::TREBLE_CLEF));
        UpdateSystemHeight(system);
    }

    return true;
}

/// Removes the guitar (and staff) at the specified index
bool Score::RemoveGuitar(size_t index)
{
    CHECK_THAT(IsValidGuitarIndex(index), false);

    m_guitarArray.erase(m_guitarArray.begin() + index);

    // remove staff from each system
    for (size_t i = 0; i < m_systemArray.size(); i++)
    {
        SystemPtr system = m_systemArray[i];
        system->m_staffArray.erase(system->m_staffArray.begin() + index);
        UpdateSystemHeight(system);
    }

    return true;
}

void Score::MergeScore(const Score &otherScore)
{
    using namespace std::placeholders;

    std::for_each(otherScore.m_guitarArray.begin(), otherScore.m_guitarArray.end(),
                  std::bind(&Score::InsertGuitar, this, _1));
}

/// Determines if a alternate ending index is valid
/// @param index alternate ending index to validate
/// @return True if the alternate ending index is valid, false if not
bool Score::IsValidAlternateEndingIndex(uint32_t index) const
{
    return index < GetAlternateEndingCount();
}

/// Gets the number of alternate endings in the score
/// @return The number of alternate endings in the score
size_t Score::GetAlternateEndingCount() const
{
    return m_alternateEndingArray.size();
}

/// Gets the nth alternate ending in the score
/// @param index Index of the alternate ending to get
/// @return The nth alternate ending in the score
Score::AlternateEndingPtr Score::GetAlternateEnding(uint32_t index) const
{
    CHECK_THAT(IsValidAlternateEndingIndex(index), AlternateEndingPtr());
    return m_alternateEndingArray[index];
}

/// Inserts a new alternate ending into the score
void Score::InsertAlternateEnding(AlternateEndingPtr altEnding)
{
    m_alternateEndingArray.push_back(altEnding);

    // sort the alternate endings by system, then position
    using boost::make_indirect_iterator;
    std::sort(make_indirect_iterator(m_alternateEndingArray.begin()),
              make_indirect_iterator(m_alternateEndingArray.end()));
}

/// Removes the specified alternate ending from the score, if possible
void Score::RemoveAlternateEnding(AlternateEndingPtr altEnding)
{
    m_alternateEndingArray.erase(std::remove(m_alternateEndingArray.begin(),
                                             m_alternateEndingArray.end(),
                                             altEnding));
}

/// Determines if a chord diagram index is valid
/// @param index ChordDiagram index to validate
/// @return True if the chord diagram index is valid, false if not
bool Score::IsValidChordDiagramIndex(uint32_t index) const
{
    return index < GetChordDiagramCount();
}

/// Gets the number of chord diagrams in the score
/// @return The number of chord diagrams in the score
size_t Score::GetChordDiagramCount() const
{
    return m_chordDiagramArray.size();
}

/// Gets the nth chord diagram in the score
/// @param index Index of the chord diagram to get
/// @return The nth chord diagram in the score
Score::ChordDiagramPtr Score::GetChordDiagram(uint32_t index) const
{
    CHECK_THAT(IsValidChordDiagramIndex(index), ChordDiagramPtr());
    return m_chordDiagramArray[index];
}

// Guitar Functions
/// Determines if a guitar index is valid
/// @param index guitar index to validate
/// @return True if the guitar index is valid, false if not
bool Score::IsValidGuitarIndex(uint32_t index) const
{
    return index < GetGuitarCount();
}

/// Gets the number of guitars in the score
/// @return The number of guitars in the score
size_t Score::GetGuitarCount() const
{
    return m_guitarArray.size();
}

/// Gets the nth guitar in the score
/// @param index Index of the guitar to get
/// @return The nth guitar in the score
Score::GuitarPtr Score::GetGuitar(uint32_t index) const
{
    CHECK_THAT(IsValidGuitarIndex(index), GuitarPtr());
    return m_guitarArray[index];
}

// System Functions
/// Determines if a system index is valid
/// @param index system index to validate
/// @return True if the system index is valid, false if not
bool Score::IsValidSystemIndex(uint32_t index) const
{
    return index < GetSystemCount();
}

/// Gets the number of systems in the score
/// @return The number of systems in the score
size_t Score::GetSystemCount() const
{
    return m_systemArray.size();
}

/// Gets the nth system in the score
/// @param index Index of the system to get
/// @return The nth system in the score
Score::SystemPtr Score::GetSystem(uint32_t index) const
{
    CHECK_THAT(IsValidSystemIndex(index), SystemPtr());
    return m_systemArray[index];
}

// Dynamic Functions
/// Determines if a dynamic index is valid
/// @param index dynamic index to validate
/// @return True if the dynamic index is valid, false if not
bool Score::IsValidDynamicIndex(uint32_t index) const
{
    return index < GetDynamicCount();
}

/// Gets the number of dynamics in the score
/// @return The number of dynamics in the score
size_t Score::GetDynamicCount() const
{
    return m_dynamicArray.size();
}

/// Gets the nth dynamic in the score
/// @param index Index of the dynamic to get
/// @return The nth dynamic in the score
Score::DynamicPtr Score::GetDynamic(uint32_t index) const
{
    CHECK_THAT(IsValidDynamicIndex(index), DynamicPtr());
    return m_dynamicArray[index];
}

// Floating Text Functions
/// Determines if a floating text index is valid
/// @param index floating text index to validate
/// @return True if the floating text index is valid, false if not
bool Score::IsValidFloatingTextIndex(uint32_t index) const
{
    return index < GetFloatingTextCount();
}

/// Gets the number of floating text items in the score
/// @return The number of floating text items in the score
size_t Score::GetFloatingTextCount() const
{
    return m_floatingTextArray.size();
}

/// Gets the nth floating text in the score
/// @param index Index of the floating text to get
/// @return The nth floating text in the score
Score::FloatingTextPtr Score::GetFloatingText(uint32_t index) const
{
    CHECK_THAT(IsValidFloatingTextIndex(index), FloatingTextPtr());
    return m_floatingTextArray[index];
}

// Guitar In Functions
/// Determines if a guitar in index is valid
/// @param index guitar in index to validate
/// @return True if the guitar in index is valid, false if not
bool Score::IsValidGuitarInIndex(uint32_t index) const
{
    return index < GetGuitarInCount();
}

/// Gets the number of guitar ins in the score
/// @return The number of guitar ins in the score
size_t Score::GetGuitarInCount() const
{
    return m_guitarInArray.size();
}

/// Gets the nth guitar in in the score
/// @param index Index of the guitar in to get
/// @return The nth guitar in in the score
Score::GuitarInPtr Score::GetGuitarIn(uint32_t index) const
{
    CHECK_THAT(IsValidGuitarInIndex(index), GuitarInPtr());
    return m_guitarInArray[index];
}

// Tempo Marker Functions
/// Determines if a tempo marker index is valid
/// @param index tempo marker index to validate
/// @return True if the tempo marker index is valid, false if not
bool Score::IsValidTempoMarkerIndex(uint32_t index) const
{
    return index < GetTempoMarkerCount();
}

/// Gets the number of tempo markers in the score
/// @return The number of tempo markers in the score
size_t Score::GetTempoMarkerCount() const
{
    return m_tempoMarkerArray.size();
}

/// Gets the nth tempo marker in the score
/// @param index Index of the tempo marker to get
/// @return The nth tempo marker in the score
Score::TempoMarkerPtr Score::GetTempoMarker(uint32_t index) const
{
    CHECK_THAT(IsValidTempoMarkerIndex(index), TempoMarkerPtr());
    return m_tempoMarkerArray[index];
}

/// Inserts a tempo marker into the score
void Score::InsertTempoMarker(Score::TempoMarkerPtr marker)
{
    m_tempoMarkerArray.push_back(marker);

    // sort the tempo markers by system, then position
    using boost::make_indirect_iterator;
    std::sort(make_indirect_iterator(m_tempoMarkerArray.begin()),
              make_indirect_iterator(m_tempoMarkerArray.end()));
}

/// Returns the dynamic at the specified location, or NULL if none exists
Score::DynamicPtr Score::FindDynamic(uint32_t system, uint32_t staff, uint32_t positionIndex) const
{
    BOOST_FOREACH(DynamicPtr dynamic, m_dynamicArray)
    {
        if (dynamic->GetSystem() == system &&
            dynamic->GetStaff() == staff &&
            dynamic->GetPosition() == positionIndex)
        {
            return dynamic;
        }
    }

    return DynamicPtr();
}
