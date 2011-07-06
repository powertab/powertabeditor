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

/// Stores and renders tempo markers
class TempoMarker : public SystemSymbol
{
// Constants
public:
	// Default constants
        static const std::string    DEFAULT_DESCRIPTION;            ///< Default value for the description member variable
        static const uint8_t        DEFAULT_BEAT_TYPE;              ///< Default value for the beat type value
	static const uint32_t       DEFAULT_BEATS_PER_MINUTE;       ///< Default value for the beats per minute value
	
	// Beats Per Minute Constants
	static const uint32_t       MIN_BEATS_PER_MINUTE;          ///< Minimum allowed value for the beats per minute
	static const uint32_t       MAX_BEATS_PER_MINUTE;          ///< Maximum allowed value for the beats per minute
	
	enum types
	{
                notShown                = 0x00,         ///< Metronome marker is not shown
                standardMarker          = 0x01,         ///< Standard beats per minute marker (i.e. quarter note = 120)
                listesso                = 0x02,         ///< Listesso marker (i.e. quarter note = half note)
                alterationOfPace        = 0x03          ///< Alteration of pace (accel. or rit.)
	};

	enum beatTypes
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
	
	enum tripletFeelTypes
	{
                noTripletFeel                   = 0x00,
                tripletFeelEighth               = 0x01,
                tripletFeelSixteenth            = 0x02,
                tripletFeelEighthOff            = 0x03,
                tripletFeelSixteenthOff         = 0x04
	};

	enum flags
	{
                beatsPerMinuteMask              = 0xffff,         ///< Mask used to retrieve the beats per minute value
                tripletFeelTypeMask             = 0x70000,        ///< Mask used to retrieve the triplet feel type
                listessoBeatTypeMask            = 0x780000,       ///< Mask used to retrieve the listesso beat type
                beatTypeMask                    = 0x7800000,      ///< Mask used to retrieve the beat type
                typeMask                        = 0x18000000      ///< Mask used to retrieve the tempo marker type
	};

// Member Variables
protected:
        std::string m_description;

// Constructor/Destructor
public:
	TempoMarker();
	TempoMarker(uint32_t system, uint32_t position, uint8_t beatType,
                uint32_t beatsPerMinute, const std::string& description,
		uint8_t tripletFeelType);
	TempoMarker(uint32_t system, uint32_t position, uint8_t beatType,
                uint8_t listessoBeatType, const std::string& description);
	TempoMarker(uint32_t system, uint32_t position, bool accelerando);
	TempoMarker(const TempoMarker& tempoMarker);
	~TempoMarker();

// Operators
	const TempoMarker& operator=(const TempoMarker& tempoMarker);
	bool operator==(const TempoMarker& tempoMarker) const;
	bool operator!=(const TempoMarker& tempoMarker) const;

// Serialize Functions
        bool Serialize(PowerTabOutputStream& stream) const;
	bool Deserialize(PowerTabInputStream& stream, uint16_t version);

// MFC Class Functions
public:
	/// Gets the MFC Class Name for the object
	/// @return The MFC Class Name
        std::string GetMFCClassName() const
		{return "CTempoMarker";}
	/// Gets the MFC Class Schema for the object
	/// @return The MFC Class Schema
	uint16_t GetMFCClassSchema() const                            
		{return ((uint16_t)1);}
	 
// Type Functions
	/// Determines if a type is valid
	/// @param type Type to validate
	/// @return True if the type is valid, false if not
	static bool IsValidType(uint8_t type)
		{return (type <= alterationOfPace);}
	bool SetType(uint8_t type);
	uint8_t GetType() const;
	
	/// Determines if the metronome marker is shown
	/// @return True if the metronome marker is shown, false if not
	bool IsMetronomeMarkerShown() const     
		{return (GetType() != notShown);}
	
// Standard Marker Functions
	bool SetStandardMarker(uint8_t beatType, uint32_t beatsPerMinute,
                const std::string& description, uint8_t tripletFeelType = noTripletFeel);
	/// Determines if the tempo marker is a standard marker
	/// @return True if the tempo marker is a standard marker, false if not
	bool IsStandardMarker() const           
		{return (GetType() == standardMarker);}
	
// Listesso Functions
        bool SetListesso(uint8_t beatType, uint8_t listessoBeatType,
                         const std::string& description = "");
	/// Determines if the tempo marker is a listesso
	/// @return True if the tempo marker is a listesso, false if not
	bool IsListesso() const                 
		{return (GetType() == listesso);}
	
// Alteration Of Pace Functions
	/// Determines if the tempo marker is an alteration of pace
	/// @return True if the tempo marker is an alteration of pace, false if not
	bool IsAlterationOfPace() const               
		{return (GetType() == alterationOfPace);}
	bool SetAlterationOfPace(bool accelerando);
	/// Determines if the tempo marker is an accelerando marker
	/// @return True if the tempo marker is an accelerando marker, false if not
	bool IsAccelerando() const
		{return ((IsAlterationOfPace()) && (GetBeatType() == 0));}
	/// Determines if the tempo marker is a ritardando marker
	/// @return True if the tempo marker is a ritardando marker, false if not
	bool IsRitardando() const
		{return ((IsAlterationOfPace()) && (GetBeatType() == 1));}
	
// Beat Type Functions
	/// Determines if a beat type is valid
	/// @param beatType Beat type to validate
	/// @return True if the beat type is valid, false if not
	static bool IsValidBeatType(uint8_t beatType)
		{return (beatType <= thirtySecondDotted);}        
	bool SetBeatType(uint8_t beatType);
	uint8_t GetBeatType() const;
					
// Listesso Beat Type Functions
	bool SetListessoBeatType(uint8_t beatType);
	uint8_t GetListessoBeatType() const;
			
// Triplet Feel Functions
	/// Determines if a triplet feel type is valid
	/// @param tripletFeelType Triplet feel type to validate
	/// @return True if the triplet feel type is valid, false if not
	static bool IsValidTripletFeelType(uint8_t tripletFeelType)
		{return (tripletFeelType <= tripletFeelSixteenthOff);}
	bool SetTripletFeelType(uint8_t tripletFeelType);
	uint8_t GetTripletFeelType() const;
	/// Determines if the tempo marker has a triplet feel effect
	/// @return True if the tempo marker has a triplet feel effect, false if not
	bool HasTripletFeel() const
		{return (GetTripletFeelType() != noTripletFeel);}
	
// Beats Per Minute Functions
	/// Determines if a beats per minute value is valid
	/// @param beatsPerMinute Beats per minute value to validate
	/// @return True if the beats per minute value is valid, false if not
	static bool IsValidBeatsPerMinute(uint32_t beatsPerMinute)
	{
		return ((beatsPerMinute >= MIN_BEATS_PER_MINUTE) &&
			((beatsPerMinute <= MAX_BEATS_PER_MINUTE)));
	}
	/// Sets the beats per minute
	/// @param beatsPerMinute Beats per minute value to set
	/// @return True if the beats per minute value was set, false if not
	bool SetBeatsPerMinute(uint32_t beatsPerMinute);
	/// Gets the beats per minute
	/// @return The beats per minute
	uint32_t GetBeatsPerMinute() const                     
		{return ((uint32_t)(m_data & beatsPerMinuteMask));}
	
// Description Functions
	/// Sets the description
	/// @param description Description to set
	/// @return True if the description was set, false if not
        bool SetDescription(const std::string& description)
	{
            CHECK_THAT(!description.empty(), false);
            m_description = description;
            return (true);
	}
	/// Gets the description
	/// @return The description
        std::string GetDescription() const
        {
            return m_description;
        }
};

#endif // POWERTABDOCUMENT_TEMPO_MARKER_H
