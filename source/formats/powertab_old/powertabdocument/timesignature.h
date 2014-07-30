/////////////////////////////////////////////////////////////////////////////
// Name:            timesignature.h
// Purpose:         Stores and renders time signatures
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 12, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef TIMESIGNATURE_H
#define TIMESIGNATURE_H

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders time signatures
class TimeSignature : public PowerTabObject
{
// Constants
public:
    // Default Constants
    static const uint32_t   DEFAULT_DATA;           ///< Default value for the data member variable
    static const uint8_t     DEFAULT_PULSES;         ///< Default value for the pulses data member variable

    static const uint8_t     MIN_BEATSPERMEASURE;    ///< Minimum allowed value for beats per measure
    static const uint8_t     MAX_BEATSPERMEASURE;    ///< Maximum allowed value for beats per measure
    
    static const uint8_t     MIN_BEATAMOUNT;         ///< Minimum allowed value for the beat amount
    static const uint8_t     MAX_BEATAMOUNT;         ///< Maximum allowed value for the beat amount
    
    static const uint8_t     MIN_PULSES;             ///< Minimum allowed value for pulses
    static const uint8_t     MAX_PULSES;             ///< Maximum allowed value for pulses
        
    enum flags
    {
        meterMask                   = (uint32_t)0xff000000,     ///< Bit mask used to retrieve the meter
        beatsPerMeasureMask         = (uint32_t)0xf8000000,     ///< Bit mask used to retrieve the beats per measure value
        beatAmountMask              = (uint32_t)0x07000000,     ///< Bit mask used to retrieve the beat amount value
        beamingPatternMask          = (uint32_t)0xfffff,        ///< Bit mask used to retrieve the beaming pattern
        beamingPatternBeat1Mask     = (uint32_t)0xf8000,        ///< Bit mask used to retrieve beat 1 of the beaming pattern
        beamingPatternBeat2Mask     = (uint32_t)0x7c00,         ///< Bit mask used to retrieve beat 2 of the beaming pattern
        beamingPatternBeat3Mask     = (uint32_t)0x3e0,          ///< Bit mask used to retrieve beat 3 of the beaming pattern
        beamingPatternBeat4Mask     = (uint32_t)0x1f,           ///< Bit mask used to retrieve beat 4 of the beaming pattern
        show                        = (uint32_t)0x100000,       ///< Show the time signature
        brackets                    = (uint32_t)0x200000,       ///< Place brackets around the time signature
        commonTime                  = (uint32_t)0x400000,       ///< Use common time symbol
        cutTime                     = (uint32_t)0x800000        ///< Use cut time symbol
    };

    enum textFlags
    {
        textBeatsPerMeasure     = 0,                            ///< Numerator text
        textBeatAmount          = 1,                            ///< Denominator text
        textFull                = 2                             ///< Full fractional text 4/4
    };
    
// Member Variables
private:
    uint32_t    m_data;             ///< Stores the meter, beaming pattern and any flags (see flags enum for bit breakdown)
    uint8_t      m_pulses;           ///< Number of pulses in a measure

public:
    TimeSignature();
    
// MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CTimeSignature2";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}
    
// Serialize Functions
        bool Serialize(PowerTabOutputStream &stream) const override;
        bool Deserialize(PowerTabInputStream &stream,
                         uint16_t version) override;

    /// Determines if the time signature is in cut time
    /// @return True if the time signature is in cut time, false if not
    bool IsCutTime() const                          
        {return (IsFlagSet(cutTime));}
    /// Determines if the time signature is in common time
    /// @return True if the time signature is in common time, false if not
    bool IsCommonTime() const                       
        {return (IsFlagSet(commonTime));}

    uint8_t GetBeatsPerMeasure() const;
    
    uint8_t GetBeatAmount() const;
    
    void GetBeamingPattern(uint8_t & beat1, uint8_t & beat2, uint8_t & beat3,
        uint8_t & beat4) const;

    /// Determines if the time signature is shown
    /// @return True if the time signature is shown, false if not
    bool IsShown() const                            
        {return (IsFlagSet(show));}
        
    uint8_t GetPulses() const;
    
// Flag Functions
    /// Determines if a flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidFlag(uint32_t flag)          
    {
        return ((flag >= show) &&
            (flag <= (show | brackets | commonTime | cutTime)));
    }
    /// Determines if a flag used by the TimeSignature object is set
    /// @param flag The flag to test
    /// @return True if the flag is set, false if not
    bool IsFlagSet(uint32_t flag) const             
        {PTB_CHECK_THAT(IsValidFlag(flag), false); return ((m_data & flag) == flag);}

    int GetWidth() const;
};

}

#endif // TIMESIGNATURE_H
