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
#include <map>
#include <bitset>

/// Default Constructor
Score::Score()
{
    m_systemArray.push_back(new System);
    //------Last Checked------//
    // - Jan 5, 2005
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

    UpdateToVer2Structure();

    return (stream.CheckState());
}

void Score::UpdateToVer2Structure()
{
    using std::pair;
    using std::multimap;

    multimap<uint32_t, GuitarIn*> guitarInMap;
    multimap<uint32_t, uint32_t> guitarToStaffMap;
    typedef multimap<uint32_t, GuitarIn*>::iterator guitarInIt;
    typedef multimap<uint32_t, uint32_t>::iterator guitarToStaffIt; // keeps track of which staffs contain which guitars

    // find all guitar in symbols and the system they occur in
    for (uint32_t i=0; i < m_guitarInArray.size(); i++)
    {
        GuitarIn* gtr_in = m_guitarInArray.at(i);
        guitarInMap.insert(pair<uint32_t, GuitarIn*>(gtr_in->GetSystem(), gtr_in));
    }

    for (uint32_t i=0; i < m_systemArray.size(); i++) // iterate through all systems
    {
        // if there are guitar ins, readjust the Guitar->Staff mapping
        if ((guitarInMap.count(i)) >= 1)
        {
            //std::cerr << "In System " << i << std::endl;
            pair<guitarInIt, guitarInIt> range = guitarInMap.equal_range(i);
            for (auto i = range.first; i != range.second; ++i)
            {
                guitarToStaffMap.erase(i->second->GetStaff());
                //std::cerr << "At Position: " << i->second->GetPosition() << std::endl;
                std::bitset<8> guitarBitmap(i->second->GetStaffGuitars());
                for (uint8_t gtr = 0; gtr < 8; gtr++)
                {
                    if (guitarBitmap.test(gtr) == true)
                    {
                        guitarToStaffMap.insert(pair<uint32_t, uint32_t>(i->second->GetStaff(), gtr));
                        //std::cerr << "Guitar In: " << (int)gtr + 1 << std::endl;
                    }
                }
            }
        }

        System* currentSystem = m_systemArray.at(i);

        vector<Staff*> newStaves(m_guitarArray.size(), NULL); // one staff per track

        for (uint32_t j=0; j < currentSystem->m_staffArray.size(); j++) // go through staves
        {
            Staff* currentStaff = currentSystem->m_staffArray.at(j);

            pair<guitarToStaffIt, guitarToStaffIt> range = guitarToStaffMap.equal_range(j); // find guitars for this staff
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
                Staff* exampleStaff = currentSystem->m_staffArray.at(0);
                // copy old staff but with rests instead
                for (uint32_t n=0; n < exampleStaff->highMelodyPositionArray.size(); n++)
                {
                    newStaff->SetStandardNotationStaffAboveSpacing(15);
                    Position* newPosition = exampleStaff->highMelodyPositionArray.at(n)->CloneObject();
                    newPosition->SetRest();
                    for(uint32_t q=0; q < newPosition->m_noteArray.size(); q++)
                    {
                        delete newPosition->m_noteArray.at(q);
                    }
                    newPosition->m_noteArray.clear();
                    newStaff->highMelodyPositionArray.push_back(newPosition);
                }

                newStaves[m] = newStaff;
            }
        }

        // clear out the old staves, and swap in the new ones
        for (uint32_t n=0; n < currentSystem->m_staffArray.size(); n++)
        {
            delete currentSystem->m_staffArray.at(n);
        }
        currentSystem->m_staffArray.swap(newStaves);

        // readjust locations / heights
        currentSystem->CalculateHeight();
        Rect rect = currentSystem->GetRect();
        if (i == 0)
        {
            currentSystem->SetRect(Rect(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight()));
        }
        else
        {
            Rect prevRect = m_systemArray.at(i-1)->GetRect();
            currentSystem->SetRect(Rect(rect.GetX(), prevRect.GetBottom() + 50, rect.GetWidth(), rect.GetHeight()));
        }

    }
}
