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
#include "powertabfileheader.h"
#include "system.h"
#include "guitar.h"
#include "chorddiagram.h"
#include "floatingtext.h"
#include "guitarin.h"
#include "tempomarker.h"
#include "alternateending.h"
#include "position.h"

#include <map>
#include <bitset>

/// Default Constructor
Score::Score()
{
}

/// Copy Constructor
Score::Score(const Score& score)
{
    //------Last Checked------//
    // - Jan 5, 2005
    *this = score;
}

/// Destructor
Score::~Score()
{
    //------Last Checked------//
    // - Jan 5, 2005
    for (uint32_t i = 0; i < m_guitarArray.size(); i++)
    {
        delete m_guitarArray.at(i);
    }
    for (uint32_t i = 0; i < m_chordDiagramArray.size(); i++)
    {
        delete m_chordDiagramArray.at(i);
    }
    for (uint32_t i = 0; i < m_floatingTextArray.size(); i++)
    {
        delete m_floatingTextArray.at(i);
    }
    for (uint32_t i = 0; i < m_guitarInArray.size(); i++)
    {
        delete m_guitarInArray.at(i);
    }
    for (uint32_t i = 0; i < m_tempoMarkerArray.size(); i++)
    {
        delete m_tempoMarkerArray.at(i);
    }
    for (uint32_t i = 0; i < m_alternateEndingArray.size(); i++)
    {
        delete m_alternateEndingArray.at(i);
    }
    for (uint32_t i = 0; i < m_systemArray.size(); i++)
    {
        delete m_systemArray.at(i);
    }
    m_guitarArray.clear();
    m_chordDiagramArray.clear();
    m_floatingTextArray.clear();
    m_guitarInArray.clear();
    m_tempoMarkerArray.clear();
    m_dynamicArray.clear();
    m_alternateEndingArray.clear();
    m_systemArray.clear();
}

// Operators
/// Assignment Operator
const Score& Score::operator=(const Score& score)
{
    //------Last Checked------//
    // - Jan 5, 2005

    // Check for assignment to self
    if (this != &score)
    {
        m_guitarArray = score.m_guitarArray;
        m_chordDiagramArray = score.m_chordDiagramArray;
        m_floatingTextArray = score.m_floatingTextArray;
        m_guitarInArray = score.m_guitarInArray;
        m_tempoMarkerArray = score.m_tempoMarkerArray;
        m_dynamicArray = score.m_dynamicArray;
        m_alternateEndingArray = score.m_alternateEndingArray;
        m_systemArray = score.m_systemArray;
    }
    return (*this);
}

/// Equality Operator
bool Score::operator==(const Score& score) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (
        (m_guitarArray == score.m_guitarArray) &&
        (m_chordDiagramArray == score.m_chordDiagramArray) &&
        (m_floatingTextArray == score.m_floatingTextArray) &&
        (m_guitarInArray == score.m_guitarInArray) &&
        (m_tempoMarkerArray == score.m_tempoMarkerArray) &&
        (m_dynamicArray == score.m_dynamicArray) &&
        (m_alternateEndingArray == score.m_alternateEndingArray) &&
        (m_systemArray == score.m_systemArray));
}

/// Inequality Operator
bool Score::operator!=(const Score& score) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (!operator==(score));
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Score::Serialize(PowerTabOutputStream& stream)
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
    //------Last Checked------//
    // - Jan 5, 2005
    stream.ReadVector(m_guitarArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_chordDiagramArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_floatingTextArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_guitarInArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_tempoMarkerArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_dynamicArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_alternateEndingArray, version);
    CHECK_THAT(stream.CheckState(), false);

    stream.ReadVector(m_systemArray, version);
    CHECK_THAT(stream.CheckState(), false);

    if (version == PowerTabFileHeader::FILEVERSION_1_7)
    {
        UpdateToVer2Structure();
    }

    return (stream.CheckState());
}

// Remove guitar-ins and adjust the score structure, so that each guitar corresponds to its own staff
void Score::UpdateToVer2Structure()
{
    using std::pair;
    using std::multimap;

    multimap<uint32_t, GuitarIn*> guitarInMap; // map systems to Guitar In symbols
    multimap<uint32_t, uint32_t> guitarToStaffMap; // map guitars to (possibly multiple) staves

    // find all guitar in symbols and the system they occur in
    for (uint32_t i=0; i < m_guitarInArray.size(); i++)
    {
        GuitarIn* gtr_in = m_guitarInArray.at(i);
        guitarInMap.insert(pair<uint32_t, GuitarIn*>(gtr_in->GetSystem(), gtr_in));
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
                GuitarIn* currentGuitarIn = i->second;
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

        System* currentSystem = m_systemArray.at(i);

        std::vector<Staff*> newStaves(m_guitarArray.size(), NULL); // one staff per guitar

        for (uint32_t j=0; j < currentSystem->m_staffArray.size(); j++) // go through staves
        {
            Staff* currentStaff = currentSystem->m_staffArray.at(j);

            auto range = guitarToStaffMap.equal_range(j); // find guitars for this staff
            for (auto k = range.first; k != range.second; ++k)
            {
                newStaves[k->second] = currentStaff->CloneObject();
            }
        }

        for (uint32_t m=0; m < newStaves.size(); m++) // insert blank staves for any guitars that aren't currently used
        {
            if (newStaves.at(m) == NULL)
            {
                Staff* newStaff = new Staff;

                std::vector<Barline*> barlines;
                currentSystem->GetBarlines(barlines);

                // just insert a whole rest after each barline, except for the last one
                for (size_t n = 0; n < barlines.size() - 1; n++)
                {
                    Barline* barline = barlines.at(n);
                    Position* newPosition = new Position(barline->GetPosition() + 1, 1, 0);
                    newPosition->SetRest(true);
                    newStaff->positionArrays[0].push_back(newPosition);
                }

                newStaves[m] = newStaff;
            }
        }

        // clear out the old staves, and swap in the new ones
        for (uint32_t n=0; n < currentSystem->m_staffArray.size(); n++)
        {
            delete currentSystem->m_staffArray.at(n);
        }
        currentSystem->m_staffArray = newStaves;

        // readjust locations / heights
        currentSystem->CalculateHeight();
        if (i != 0) // adjust the location of every staff after the first
        {
            Rect currentRect = currentSystem->GetRect();
            Rect prevRect = m_systemArray.at(i-1)->GetRect();
            currentRect.SetY(prevRect.GetBottom() + 50);
            currentSystem->SetRect(currentRect);
        }

        // recalculate the beaming for the member staves, using the current beaming algorithm
        currentSystem->CalculateBeamingForStaves();
    }
}

// Finds the index of a system within the score
int Score::FindSystemIndex(System* system) const
{
    auto result = std::find(m_systemArray.begin(), m_systemArray.end(), system);
    return std::distance(m_systemArray.begin(), result);
}

// Helper function for GetAlternateEndingsInSystem and GetTempoMarkersInSystem
template<class Symbol>
void GetSymbolsInSystem(std::vector<Symbol*>& output, const std::vector<Symbol*>& symbolList, const uint32_t systemIndex)
{
    output.clear();

    for (size_t i = 0; i < symbolList.size(); i++)
    {
        Symbol* symbol = symbolList.at(i);
        if (symbol->GetSystem() == systemIndex)
        {
            output.push_back(symbol);
        }
    }
}

// Finds all of the tempo markers that are in the given system
void Score::GetTempoMarkersInSystem(std::vector<TempoMarker*>& tempoMarkers, System *system) const
{
    GetSymbolsInSystem(tempoMarkers, m_tempoMarkerArray, FindSystemIndex(system));
}

void Score::GetAlternateEndingsInSystem(std::vector<AlternateEnding*>& endings, System *system) const
{
    GetSymbolsInSystem(endings, m_alternateEndingArray, FindSystemIndex(system));
}

/// Updates the height of the system, and adjusts the height of subsequent systems as necessary
/// @param system The system to update the height of
void Score::UpdateSystemHeight(System *system)
{
    // Store the original height, recalculate the height, then find the height difference
    
    const int originalHeight = system->GetRect().GetHeight();
    
    for (size_t i = 0; i < system->GetStaffCount(); i++)
    {
        Staff* currentStaff = system->GetStaff(i);
        currentStaff->CalculateTabStaffBelowSpacing();
        currentStaff->CalculateSymbolSpacing();
    }

    system->CalculateBeamingForStaves();
    
    UpdateExtraSpacing(system);
    
    system->CalculateHeight();
    const int spacingDifference = system->GetRect().GetHeight() - originalHeight;

    // adjust position of subsequent systems based on the height change
    uint32_t systemIndex = FindSystemIndex(system) + 1;
    for (; systemIndex < GetSystemCount(); systemIndex++)
    {
        System* currentSystem = m_systemArray.at(systemIndex);
        Rect rect = currentSystem->GetRect();
        rect.SetTop(rect.GetTop() + spacingDifference);
        currentSystem->SetRect(rect);
    }
}

/// Updates the extra spacing at the top of the system (for rehearsal signs, etc)
void Score::UpdateExtraSpacing(System* system)
{
    // get list of tempo markers
    std::vector<TempoMarker*> markers;
    GetTempoMarkersInSystem(markers, system);
    
    // get list of alternate endings
    std::vector<AlternateEnding*> endings;
    GetAlternateEndingsInSystem(endings, system);
    
    // Find how many different items occur in the system (i.e. is there at least one rehearsal sign, tempo marker, etc)
    const int numItems = system->HasRehearsalSign() +
            (system->GetChordTextCount() > 0) +
            !markers.empty() +
            (system->GetDirectionCount() > 0) +
            !endings.empty();
    
    // Each type of item gets a line to itself
    system->SetExtraSpacing(numItems * System::SYSTEM_SYMBOL_SPACING);
}

void Score::Init()
{
    m_guitarArray.push_back(new Guitar);

    System* newSystem = new System;
    newSystem->Init();
    m_systemArray.push_back(newSystem);
}
