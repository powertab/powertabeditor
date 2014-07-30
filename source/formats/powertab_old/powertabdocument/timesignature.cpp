/////////////////////////////////////////////////////////////////////////////
// Name:            timesignature.cpp
// Purpose:         Stores and renders time signatures
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 12, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <cmath>           // Needed for pow()

#include "timesignature.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

const uint32_t   TimeSignature::DEFAULT_DATA            = 0x1a018000;
const uint8_t     TimeSignature::DEFAULT_PULSES          = 4;

const uint8_t     TimeSignature::MIN_BEATSPERMEASURE     = 1;
const uint8_t     TimeSignature::MAX_BEATSPERMEASURE     = 32;

const uint8_t     TimeSignature::MIN_BEATAMOUNT          = 1;
const uint8_t     TimeSignature::MAX_BEATAMOUNT          = 32;

const uint8_t     TimeSignature::MIN_PULSES              = 0;
const uint8_t     TimeSignature::MAX_PULSES              = 32;

// Constructor/Destructor
/// Default Constructor
TimeSignature::TimeSignature() :
    m_data(DEFAULT_DATA), m_pulses(DEFAULT_PULSES)
{
}

/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool TimeSignature::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_data << m_pulses;
    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool TimeSignature::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream >> m_data >> m_pulses;
    return true;
}

/// Gets the time signature's beats per measure value (numerator)
/// @return The time signature's beats per measure value
uint8_t TimeSignature::GetBeatsPerMeasure() const
{
    //------Last Checked------//
    // - Dec 12, 2004

    // Common Time
    if (IsCommonTime())
        return (4);
    // Cut Time
    else if (IsCutTime())
        return (2);

    // beatsPerMeasure is stored in zero-based format, so add one
    uint8_t beatsPerMeasure = (uint8_t)((m_data & beatsPerMeasureMask) >> 27);
    beatsPerMeasure++;
    return (beatsPerMeasure);
}

/// Gets the time signature's beat amount (denominator)
/// @return The time signature's beat amount
uint8_t TimeSignature::GetBeatAmount() const
{
    //------Last Checked------//
    // - Dec 13, 2004

    // Common Time
    if (IsCommonTime())
        return (4);
    // Cut Time
    else if (IsCutTime())
        return (2);

    // The beat amount is stored in power of 2 form
    uint8_t beatAmount = (uint8_t)((m_data & beatAmountMask) >> 24);
    beatAmount = (uint8_t)(pow((double)2, (double)beatAmount));
    return (beatAmount);
}

/// Gets the beaming pattern used by the time signature
/// @param beat1 - Beaming pattern for the first beat in the time signature
/// @param beat2 - Beaming pattern for the second beat in the time signature
/// @param beat3 - Beaming pattern for the third beat in the time signature
/// @param beat4 - Beaming pattern for the fourth beat in the time signature
void TimeSignature::GetBeamingPattern(uint8_t & beat1, uint8_t & beat2,
    uint8_t & beat3, uint8_t & beat4) const
{
    //------Last Checked------//
    // - Dec 13, 2004

    // All the beam data is stored in zero-based format
    beat1 = (uint8_t)((m_data & beamingPatternBeat1Mask) >> 15);
    beat1++;

    beat2 = (uint8_t)((m_data & beamingPatternBeat2Mask) >> 10);
    if (beat2 > 0)
        beat2++;

    beat3 = (uint8_t)((m_data & beamingPatternBeat3Mask) >> 5);
    if (beat3 > 0)
        beat3++;

    beat4 = (uint8_t)(m_data & beamingPatternBeat4Mask);
    if (beat4 > 0)
        beat4++;
}

/// Gets the number of pulses in a measure
/// @return The number of pulses in a measure
uint8_t TimeSignature::GetPulses() const
{
    return m_pulses;
}

/// Gets the width of the time signature, in drawing units
/// @return the width of the time signature
int TimeSignature::GetWidth() const
{
    // Time signature must be shown to have width
    // All time signatures are 18 units wide
    return IsShown() ? 18 : 0;
}

}
