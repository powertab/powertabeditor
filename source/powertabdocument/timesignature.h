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

// Constructor/Destructor
public:
    TimeSignature();
    TimeSignature(uint8_t beatsPerMeasure, uint8_t beatAmount);
    TimeSignature(const TimeSignature& timeSignature);
    ~TimeSignature();

// Operators
    const TimeSignature& operator=(const TimeSignature& timeSignature);
    bool operator==(const TimeSignature& timeSignature) const;
    bool operator!=(const TimeSignature& timeSignature) const;
    
// MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
        {return "CTimeSignature2";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const                            
        {return ((uint16_t)1);}
    
// Serialize Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

// Meter Functions
public:
    bool SetMeter(uint8_t beatsPerMeasure, uint8_t beatAmount);
    void GetMeter(uint8_t & beatsPerMeasure, uint8_t & beatAmount) const;
    /// Determines if the meter is the same as that of another TimeSignature
    /// object
    /// @param timeSignature TimeSignature object to compare with
    /// @return True if the time signatures have the same meter, false if not
    bool IsSameMeter(const TimeSignature& timeSignature) const
    {
        return ((GetBeatsPerMeasure() == timeSignature.GetBeatsPerMeasure()) &&
        (GetBeatAmount() == timeSignature.GetBeatAmount()));
    }
    // Sets the time signature to use the cut time symbol and meter
    void SetCutTime()                               
        {SetMeter(2,2); SetFlag(cutTime); /* note: Flag must be set after call to SetMeter, cause it clears both cut and common flags) */}
    /// Determines if the time signature is in cut time
    /// @return True if the time signature is in cut time, false if not
    bool IsCutTime() const                          
        {return (IsFlagSet(cutTime));}
    /// Sets the time signature to use common time symbol and meter
    void SetCommonTime()                            
        {SetMeter(4,4); SetFlag(commonTime); /* see note above */}
    /// Determines if the time signature is in common time
    /// @return True if the time signature is in common time, false if not
    bool IsCommonTime() const                       
        {return (IsFlagSet(commonTime));}
    bool IsCompoundTime() const;
    bool IsQuadrupleTime() const;
    uint32_t GetBasicBeat() const;
    uint32_t GetMeasureTotal() const;

// Beats Per Measure Functions
    /// Determines if a beats per measure value is valid
    /// @param beatsPerMeasure Beats per measure value to validate
    /// @return True if the beats per measure value is valid, false if not
    static bool IsValidBeatsPerMeasure(uint8_t beatsPerMeasure)      
    {
        return ((beatsPerMeasure >= MIN_BEATSPERMEASURE) &&
            (beatsPerMeasure <= MAX_BEATSPERMEASURE));
    }
    bool SetBeatsPerMeasure(uint8_t beatsPerMeasure);
    uint8_t GetBeatsPerMeasure() const;
    
// Beat Amount Functions
    /// Determines if a beat amount is valid
    /// @param beatAmount Beat amount to validate
    /// @return True if the beat amount is valid, false if not
    static bool IsValidBeatAmount(uint8_t beatAmount)                
    {
        return beatAmount == 1 || beatAmount == 2 || beatAmount == 4 ||
            beatAmount == 8 || beatAmount == 16 || beatAmount == 32;
    }
    bool SetBeatAmount(uint8_t beatAmount);
    uint8_t GetBeatAmount() const;
    
// Beaming Pattern Functions
    /// Determines if the beaming pattern is valid.
    static bool IsValidBeamingPattern(uint8_t beat1, uint8_t beat2,
                                      uint8_t beat3, uint8_t beat4);
    /// Determines if a beaming pattern beat value is valid.
    static bool IsValidBeamingPatternBeat(uint8_t beat, bool beat1);

    bool SetBeamingPattern(uint8_t beat1, uint8_t beat2 = 0, uint8_t beat3 = 0,
        uint8_t beat4 = 0);
    void GetBeamingPattern(uint8_t & beat1, uint8_t & beat2, uint8_t & beat3,
        uint8_t & beat4) const;
    bool SetBeamingPatternFromuint32_t(uint32_t beamingPattern);
    uint32_t GetBeamingPatternAsuint32_t() const;

// Show Functions
    /// Makes the time signature visible
    void Show()                                     
        {SetFlag(show);}
    /// Makes the time signature invisible
    void Hide()                                     
        {ClearFlag(show);}
    /// Determines if the time signature is shown
    /// @return True if the time signature is shown, false if not
    bool IsShown() const                            
        {return (IsFlagSet(show));}
    void SetShown(bool set)
    {
        if (set)
        {
            Show();
        }
        else
        {
            Hide();
        }
    }
        
// Pulse Functions
    bool IsValidPulses(uint8_t pulses) const;
    bool SetPulses(uint8_t pulses);
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
    bool SetFlag(uint32_t flag);
    /// Clears a flag used by the TimeSignature object
    /// @param flag The flag to clear
    bool ClearFlag(uint32_t flag)                   
        {PTB_CHECK_THAT(IsValidFlag(flag), false); m_data &= ~flag; return (true);}
    /// Determines if a flag used by the TimeSignature object is set
    /// @param flag The flag to test
    /// @return True if the flag is set, false if not
    bool IsFlagSet(uint32_t flag) const             
        {PTB_CHECK_THAT(IsValidFlag(flag), false); return ((m_data & flag) == flag);}

// Operations    
public:
    std::string GetText(uint32_t type) const;
    int GetWidth() const;
};

#endif // TIMESIGNATURE_H
