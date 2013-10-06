/////////////////////////////////////////////////////////////////////////////
// Name:            tempomarker.h
// Purpose:         Stores and renders tempo markers
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 13, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef POWERTABDOCUMENT_TEMPO_MARKER_H
#define POWERTABDOCUMENT_TEMPO_MARKER_H

#include "systemsymbol.h"

namespace PowerTabDocument {

/// Stores and renders tempo markers.
class TempoMarker : public SystemSymbol
{
public:
    /// Default value for the description member variable.
    static const std::string DEFAULT_DESCRIPTION;
    /// Default value for the beat type value.
    static const uint8_t DEFAULT_BEAT_TYPE;
    /// Default value for the beats per minute value.
    static const uint32_t DEFAULT_BEATS_PER_MINUTE;
    /// Minimum allowed value for the beats per minute.
    static const uint32_t MIN_BEATS_PER_MINUTE;
    /// Maximum allowed value for the beats per minute.
    static const uint32_t MAX_BEATS_PER_MINUTE;

    enum Type
    {
        /// Metronome marker is not shown.
        notShown = 0x00,
        /// Standard beats per minute marker (i.e. quarter note = 120).
        standardMarker = 0x01,
        /// Listesso marker (i.e. quarter note = half note).
        listesso = 0x02,
        /// Alteration of pace (accel. or rit.).
        alterationOfPace = 0x03
    };

    enum BeatType
    {
        half                = 0x00,
        halfDotted          = 0x01,
        quarter             = 0x02,
        quarterDotted       = 0x03,
        eighth              = 0x04,
        eighthDotted        = 0x05,
        sixteenth           = 0x06,
        sixteenDotted       = 0x07,
        thirtySecond        = 0x08,
        thirtySecondDotted  = 0x09
    };

    enum TripletFeelType
    {
        noTripletFeel                   = 0x00,
        tripletFeelEighth               = 0x01,
        tripletFeelSixteenth            = 0x02,
        tripletFeelEighthOff            = 0x03,
        tripletFeelSixteenthOff         = 0x04
    };

    enum Flags
    {
        /// Mask used to retrieve the beats per minute value.
        beatsPerMinuteMask = 0xffff,
        /// Mask used to retrieve the triplet feel type.
        tripletFeelTypeMask = 0x70000,
        /// Mask used to retrieve the listesso beat type.
        listessoBeatTypeMask = 0x780000,
        /// Mask used to retrieve the beat type.
        beatTypeMask = 0x7800000,
        /// Mask used to retrieve the tempo marker type.
        typeMask = 0x18000000
    };

    // Constructor/Destructor.
    TempoMarker();
    TempoMarker(uint32_t system, uint32_t position, uint8_t beatType,
                uint32_t beatsPerMinute, const std::string& description,
                uint8_t tripletFeelType);
    TempoMarker(uint32_t system, uint32_t position, uint8_t beatType,
                uint8_t listessoBeatType, const std::string& description);
    TempoMarker(uint32_t system, uint32_t position, bool accelerando);

    // Operators
    bool operator==(const TempoMarker& tempoMarker) const;
    bool operator!=(const TempoMarker& tempoMarker) const;

    // Serialize Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {
        return "CTempoMarker";
    }
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {
        return 1;
    }

    // Type Functions
    static bool IsValidType(uint8_t type);
    bool SetType(uint8_t type);
    uint8_t GetType() const;

    bool IsMetronomeMarkerShown() const;

    // Standard Marker Functions
    bool SetStandardMarker(uint8_t beatType, uint32_t beatsPerMinute,
                           const std::string& description,
                           uint8_t tripletFeelType = noTripletFeel);
    bool IsStandardMarker() const;

    // Listesso Functions
    bool SetListesso(uint8_t beatType, uint8_t listessoBeatType,
                     const std::string& description = "");
    bool IsListesso() const;

    // Alteration Of Pace Functions
    bool IsAlterationOfPace() const;
    bool SetAlterationOfPace(bool accelerando);
    bool IsAccelerando() const;
    bool IsRitardando() const;

    // Beat Type Functions
    static bool IsValidBeatType(uint8_t beatType);
    bool SetBeatType(uint8_t beatType);
    uint8_t GetBeatType() const;

    // Listesso Beat Type Functions
    bool SetListessoBeatType(uint8_t beatType);
    uint8_t GetListessoBeatType() const;

    // Triplet Feel Functions
    static bool IsValidTripletFeelType(uint8_t tripletFeelType);
    bool SetTripletFeelType(uint8_t tripletFeelType);
    uint8_t GetTripletFeelType() const;
    bool HasTripletFeel() const;

    // Beats Per Minute Functions
    static bool IsValidBeatsPerMinute(uint32_t beatsPerMinute);
    bool SetBeatsPerMinute(uint32_t beatsPerMinute);
    uint32_t GetBeatsPerMinute() const;

    // Description Functions
    bool SetDescription(const std::string& description);
    std::string GetDescription() const;

private:
    std::string m_description;
};

}

#endif // POWERTABDOCUMENT_TEMPO_MARKER_H
