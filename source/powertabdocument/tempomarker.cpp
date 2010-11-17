/////////////////////////////////////////////////////////////////////////////
// Name:            tempomarker.cpp
// Purpose:         Stores and renders tempo markers
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 16, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "tempomarker.h"

#include "powertabfileheader.h"                     // Needed for file version constants

// Default constants
const char*       TempoMarker::DEFAULT_DESCRIPTION                = "";
const uint8_t        TempoMarker::DEFAULT_BEAT_TYPE                  = quarter;
const uint32_t      TempoMarker::DEFAULT_BEATS_PER_MINUTE           = 120;

// Beats Per Minute Constants
const uint32_t      TempoMarker::MIN_BEATS_PER_MINUTE               = 40;
const uint32_t      TempoMarker::MAX_BEATS_PER_MINUTE               = 300;

// Constructor/Destructor
/// Default Constructor
TempoMarker::TempoMarker() :
    m_description(DEFAULT_DESCRIPTION)
{
    //------Last Checked------//
    // - Jan 13, 2005
    SetType(standardMarker);
    SetBeatType(DEFAULT_BEAT_TYPE);
    SetBeatsPerMinute(DEFAULT_BEATS_PER_MINUTE);
}

/// Primary Constructor
/// @param system Zero-based index of the system where the dynamic is anchored
/// @param position Zero-based index of the position within the system where the
/// dynamic is anchored
/// @param beatType Beat type (see beatTypes enum for values)
/// @param beatsPerMinute Beats per minute
/// @param description Description for the tempo
/// @param tripletFeelType Triplet feel type to set (see tripletFeelTypes for
/// values)
TempoMarker::TempoMarker(uint32_t system, uint32_t position, uint8_t beatType,
    uint32_t beatsPerMinute, const char* description, uint8_t tripletFeelType) :
    m_description(DEFAULT_DESCRIPTION)
{
    //------Last Checked------//
    // - Jan 13, 2005
    assert(IsValidSystem(system));
    assert(IsValidPosition(position));
    assert(IsValidBeatType(beatType));
    assert(IsValidBeatsPerMinute(beatsPerMinute));
    assert(description != NULL);
    assert(IsValidTripletFeelType(tripletFeelType));
    
    SetSystem(system);
    SetPosition(position);    
    SetStandardMarker(beatType, beatsPerMinute, description, tripletFeelType);
}

/// Listesso Constructor
/// @param system Zero-based index of the system where the dynamic is anchored
/// @param position Zero-based index of the position within the system where the
/// dynamic is anchored
/// @param beatType Beat type to set
/// @param listessoBeatType Listesso beat type to set
/// @param description Description to set
TempoMarker::TempoMarker(uint32_t system, uint32_t position, uint8_t beatType,
    uint8_t listessoBeatType, const char* description) :
    m_description(DEFAULT_DESCRIPTION)
{
    //------Last Checked------//
    // - Jan 13, 2005
    assert(IsValidSystem(system));
    assert(IsValidPosition(position));
    assert(IsValidBeatType(beatType));
    assert(IsValidBeatType(listessoBeatType));
    assert(description != NULL);

    SetSystem(system);
    SetPosition(position);    
    SetListesso(beatType, listessoBeatType, description);
}

/// Alteration Of Pace Constructor
/// @param system Zero-based index of the system where the dynamic is anchored
/// @param position Zero-based index of the position within the system where the
/// dynamic is anchored
/// @param accelerando True sets an accelerando, false sets a ritardando
TempoMarker::TempoMarker(uint32_t system, uint32_t position, bool accelerando) :
    m_description(DEFAULT_DESCRIPTION)
{
    //------Last Checked------//
    // - Jan 13, 2005
    assert(IsValidSystem(system));
    assert(IsValidPosition(position));
    
    SetSystem(system);
    SetPosition(position);
    SetAlterationOfPace(accelerando);
}

/// Copy Constructor
TempoMarker::TempoMarker(const TempoMarker& tempoMarker) :
    m_description(DEFAULT_DESCRIPTION)
{
    //------Last Checked------//
    // - Jan 13, 2005
    *this = tempoMarker;
}
   
/// Destructor
TempoMarker::~TempoMarker()
{
    //------Last Checked------//
    // - Jan 13, 2005
}

/// Assignment Operator
const TempoMarker& TempoMarker::operator=(const TempoMarker& tempoMarker)
{
    //------Last Checked------//
    // - Jan 13, 2005

    // Check for assignment to self
    if (this != &tempoMarker)
    {
        SystemSymbol::operator=(tempoMarker);
        m_description = tempoMarker.m_description;
    }
    return (*this);
}

/// Equality Operator
bool TempoMarker::operator==(const TempoMarker& tempoMarker) const
{
    //------Last Checked------//
    // - Jan 13, 2005
    return (
        (SystemSymbol::operator==(tempoMarker)) &&
        (m_description == tempoMarker.m_description)
    );
}

/// Inequality Operator
bool TempoMarker::operator!=(const TempoMarker& tempoMarker) const
{
    //------Last Checked------//
    // - Dec 10, 2004
    return (!operator==(tempoMarker));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool TempoMarker::Serialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Jan 13, 2005
    SystemSymbol::Serialize(stream);
    CHECK_THAT(stream.CheckState(), false);
    
    stream.WriteMFCString(m_description);
    CHECK_THAT(stream.CheckState(), false);
    
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool TempoMarker::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    //------Last Checked------//
    // - Jan 13, 2005
    SystemSymbol::Deserialize(stream, version);
    CHECK_THAT(stream.CheckState(), false);
    
    stream.ReadMFCString(m_description);
    CHECK_THAT(stream.CheckState(), false);
        
    // Version 1.0.2 or less
    if (version == PowerTabFileHeader::FILEVERSION_1_0 ||
        version == PowerTabFileHeader::FILEVERSION_1_0_2)
    {
        // Beat types are stored differently now
        uint8_t beatType = HIBYTE(HIWORD(m_data));
        
        if (beatType == 0)
            beatType = 4;
        else if (beatType == 1)
            beatType = 5;
        else if (beatType == 4)
            beatType = 0;
        else if (beatType == 5)
            beatType = 1;

        assert(IsValidBeatType(beatType));

        uint8_t tripletFeel = LOBYTE(HIWORD(m_data));
        assert(IsValidTripletFeelType(tripletFeel));

        uint16_t beatsPerMinute = LOWORD(m_data);
        assert(IsValidBeatsPerMinute(beatsPerMinute));

        // All old tempos are standard markers
        SetStandardMarker(beatType, beatsPerMinute, m_description.c_str(), tripletFeel);
    }
 
    return (stream.CheckState());
}

// Type Functions
/// Sets the tempo marker type (see types enum for values)
/// @param type Type to set
/// @return True if the type was set, false if not
bool TempoMarker::SetType(uint8_t type)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidType(type), false);
    
    m_data &= ~typeMask;
    m_data |= (uint32_t)(type << 27);
    
    return (true);
}

/// Gets the tempo marker type (see types enum for values)
/// @return The type of tempo marker
uint8_t TempoMarker::GetType() const
{
    //------Last Checked------//
    // - Jan 13, 2005
    return ((uint8_t)((m_data & typeMask) >> 27));
}

/// Sets the standard marker data
/// @param beatType 
/// @param beatsPerMinute
/// @param description
/// @param tripletFeelType
/// @return True if the data was set, false if not
bool TempoMarker::SetStandardMarker(uint8_t beatType, uint32_t beatsPerMinute,
    const char* description, uint8_t tripletFeelType)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidBeatType(beatType), false);
    CHECK_THAT(IsValidBeatsPerMinute(beatsPerMinute), false);
    CHECK_THAT(description != NULL, false);
    CHECK_THAT(IsValidTripletFeelType(tripletFeelType), false);
    
    m_data = 0;
    SetType(standardMarker);
    SetBeatType(beatType);
    SetBeatsPerMinute(beatsPerMinute);
    SetDescription(description);
    SetTripletFeelType(tripletFeelType);
    
    return (true);
}

/// Sets listesso data
/// @param beatType Beat type to set
/// @param listessoBeatType Listesso beat type to set
/// @param description Description to set
/// @return True if the data was set, false if not
bool TempoMarker::SetListesso(uint8_t beatType, uint8_t listessoBeatType,
    const char* description)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidBeatType(beatType), false);
    CHECK_THAT(IsValidBeatType(listessoBeatType), false);
    CHECK_THAT(description != NULL, false);
    
    // Clear all current data
    m_data = 0;
    
    SetType(listesso);
    SetBeatType(beatType);
    SetListessoBeatType(listessoBeatType);
    SetDescription(description);
    
    return (true);
}

/// Sets alteration of pace data
/// @param accelerando True to set an accelerando, false to set a ritardando
/// @return True if the data was set, false if not
bool TempoMarker::SetAlterationOfPace(bool accelerando)
{
    //------Last Checked------//
    // - Jan 13, 2005
       
    // Clear all current data
    m_data = 0;
    m_description.clear();

    // Set the type    
    SetType(alterationOfPace);
    
    // Quirky stuff: accelerando uses beat type 0, ritardando used beat type 1
    if (accelerando)
        SetBeatType(0);
    else
        SetBeatType(1);
    
    return (true);
}
   
// Beat Type Functions
/// Sets the beat type (see beatTypes enum for values)
/// @param beatType Beat type to set
/// @return True if the beat type was set, false if not
bool TempoMarker::SetBeatType(uint8_t beatType)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidBeatType(beatType), false);
    
    m_data &= ~beatTypeMask;
    m_data |= (uint32_t)(beatType << 23);
    
    return (true);
}

/// Gets the beat type (see beatTypes enum for values)
/// @return The beat type
uint8_t TempoMarker::GetBeatType() const
{
    //------Last Checked------//
    // - Jan 13, 2005
    return ((uint8_t)((m_data & beatTypeMask) >> 23));
}

// Listesso Beat Type Functions
/// Sets the listesso beat type (see beatTypes enum for values)
/// @param beatType Beat type to set
/// @return True if the beat type was set, false if not
bool TempoMarker::SetListessoBeatType(uint8_t beatType)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidBeatType(beatType), false);
    
    m_data &= ~listessoBeatTypeMask;
    m_data |= (uint32_t)(beatType << 19);
    
    return (true);
}

/// Gets the listesso beat type (see beatTypes enum for values)
/// @return The listesso beat type
uint8_t TempoMarker::GetListessoBeatType() const
{
    //------Last Checked------//
    // - Jan 13, 2005
    return ((uint8_t)((m_data & listessoBeatTypeMask) >> 19));
}

// Triplet Feel Functions
/// Sets the triplet feel type
/// @return True if the triplet feel type was set, false if not
bool TempoMarker::SetTripletFeelType(uint8_t tripletFeelType)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidTripletFeelType(tripletFeelType), false);
    
    m_data &= ~tripletFeelTypeMask;
    m_data |= (uint32_t)(tripletFeelType << 16);
    
    return (true);
}

/// Gets the triplet feel type
/// @return The triplet feel type
uint8_t TempoMarker::GetTripletFeelType() const
{
    //------Last Checked------//
    // - Jan 13, 2005
    return (uint8_t)((m_data & tripletFeelTypeMask) >> 16);
}

// Beats Per Minute Functions
/// Sets the beats per minute
/// @param beatsPerMinute Beats per minute value to set
/// @return True if the beats per minute value was set, false if not
bool TempoMarker::SetBeatsPerMinute(uint32_t beatsPerMinute)
{
    //------Last Checked------//
    // - Jan 13, 2005
    CHECK_THAT(IsValidBeatsPerMinute(beatsPerMinute), false);
    
    m_data &= ~beatsPerMinuteMask;
    m_data |= beatsPerMinute;
    
    return (true);
}
