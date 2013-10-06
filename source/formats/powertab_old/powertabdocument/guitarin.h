/////////////////////////////////////////////////////////////////////////////
// Name:            guitarin.h
// Purpose:         Stores and renders guitar in symbols
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 16, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef GUITARIN_H
#define GUITARIN_H

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders guitar in symbols
class GuitarIn : public PowerTabObject
{
// Constants
public:
	// Default constants
	static const uint16_t         DEFAULT_SYSTEM;                     ///< Default value for the system member variable
	static const uint8_t         DEFAULT_STAFF;                      ///< Default value for the staff member variable
	static const uint8_t         DEFAULT_POSITION;                   ///< Default value for the position member variable
	static const uint16_t         DEFAULT_DATA;                       ///< Default value for the data member variable
	
	// System Constants
	static const uint32_t       MIN_SYSTEM;                         ///< Minimum allowed value for the system member variable
	static const uint32_t       MAX_SYSTEM;                         ///< Maximum allowed value for the system member variable

	// Staff Constants
	static const uint32_t       MIN_STAFF;                          ///< Minimum allowed value for the staff member variable
	static const uint32_t       MAX_STAFF;                          ///< Maximum allowed value for the staff member variable

	// Position Constants
	static const uint32_t       MIN_POSITION;                       ///< Minimum allowed value for the position member variable
	static const uint32_t       MAX_POSITION;                       ///< Maximum allowed value for the position member variable
	
// Member Variables
private:
	uint16_t      m_system;       ///< Zero-based index of the system where the guitar in is anchored
	uint8_t      m_staff;        ///< Zero-based index of the staff within the system where the guitar in is anchored
	uint8_t      m_position;     ///< Zero-based index of the position within the system where the guitar in is anchored
	uint16_t      m_data;         ///< Bit map representing the guitar number of the active guitars (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
								///< High byte = staff guitars, low byte = rhythm slash guitars

// Constructor/Destructor
public:
	GuitarIn();
	GuitarIn(uint32_t system, uint32_t staff, uint32_t position,
		uint8_t staffGuitars, uint8_t rhythmSlashGuitars);
	GuitarIn(const GuitarIn& guitarIn);
	~GuitarIn();
	
// Operators
	const GuitarIn& operator=(const GuitarIn& guitarIn);
	bool operator==(const GuitarIn& guitarIn) const;
	bool operator!=(const GuitarIn& guitarIn) const;

// Serialize Functions
        bool Serialize(PowerTabOutputStream &stream) const override;
        bool Deserialize(PowerTabInputStream &stream,
                         uint16_t version) override;

// MFC Class Functions
public:
    std::string GetMFCClassName() const override
                {return "CGuitarIn";}
                uint16_t GetMFCClassSchema() const override
                {return ((uint16_t)1);}
	
// System Functions
	/// Determines whether a system is valid
	/// @param system System to validate
	/// @return True if the system is valid, false if not
	static bool IsValidSystem(uint32_t system)
		{return ((system >= MIN_SYSTEM) && (system <= MAX_SYSTEM));}
	/// Sets the system within the system where the guitar in is anchored
	/// @param system Zero-based index of the system where the guitar in is
	/// anchored
	/// @return True if the system was set, false if not
	bool SetSystem(uint32_t system)
	{
		PTB_CHECK_THAT(IsValidSystem(system), false);
		m_system = (uint16_t)system;
		return (true);
	}
	/// Gets the system within the system where the guitar in is anchored
	/// @return The system within the system where the guitar in is anchored
	uint32_t GetSystem() const                           
		{return (m_system);}
		
// Staff Functions
	/// Determines whether a staff is valid
	/// @param staff Staff to validate
	/// @return True if the staff is valid, false if not
	static bool IsValidStaff(uint32_t staff)
		{return ((staff >= MIN_STAFF) && (staff <= MAX_STAFF));}
	/// Sets the staff within the system where the guitar in is anchored
	/// @param staff Zero-based index of the staff within the system where the
	/// guitar in is anchored
	/// @return True if the staff was set, false if not
	bool SetStaff(uint32_t staff)
	{
		PTB_CHECK_THAT(IsValidStaff(staff), false);
		m_staff = (uint8_t)staff;
		return (true);
	}
	/// Gets the staff within the system where the guitar in is anchored
	/// @return The staff within the system where the guitar in is anchored
	uint32_t GetStaff() const                           
		{return (m_staff);}
		
// Position Functions
	/// Determines whether a position is valid
	/// @param position Position to validate
	/// @return True if the position is valid, false if not
	static bool IsValidPosition(uint32_t position)
		{return ((position >= MIN_POSITION) && (position <= MAX_POSITION));}
	/// Sets the position within the system where the guitar in is anchored
	/// @param position Zero-based index of the position within the system where
	/// the guitar in is anchored
	/// @return True if the position was set, false if not
	bool SetPosition(uint32_t position)
	{
		PTB_CHECK_THAT(IsValidPosition(position), false);
		m_position = (uint8_t)position;
		return (true);
	}
	/// Gets the position within the system where the guitar in is anchored
	/// @return The position within the system where the guitar in is anchored
	uint32_t GetPosition() const                           
		{return (m_position);}

// Staff Guitars Functions
	/// Sets the guitars active on the staff
	/// @param guitars A bit map representing the guitars to set
	/// (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
	/// @return True if the guitars were set, false if not
	bool SetStaffGuitars(uint8_t guitars)
		{return (SetGuitars(false, guitars));}
	/// Gets the staff guitars
	/// @return A bit map representing the active staff guitars
	/// (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
	uint8_t GetStaffGuitars() const
		{return (GetGuitars(false));}
	/// Determines if the staff guitars are set
	/// @return True if the staff guitars are set, false if not
	bool HasStaffGuitarsSet() const
		{return (HasGuitarsSet(false));}

// Rhythm Slash Functions
	/// Sets the guitars active for rhythm slashes
	/// @param guitars A bit map representing the guitars to set
	/// (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
	/// @return True if the guitars were set, false if not
	bool SetRhythmSlashGuitars(uint8_t guitars)
		{return (SetGuitars(true, guitars));}
	/// Gets the rhythm slash guitars
	/// @return A bit map representing the active rhythm slash guitars
	/// (bit 1 = guitar 0, bit 2 = guitar 1, etc.)
	uint8_t GetRhythmSlashGuitars() const
		{return (GetGuitars(true));}
	/// Determines if the rhythm slash guitars are set
	/// @return True if the rhythm slash guitars are set, false if not
	bool HasRhythmSlashGuitarsSet() const
		{return (HasGuitarsSet(true));}
private:
	bool SetGuitars(bool rhythmSlashes, uint8_t guitars);
	uint8_t GetGuitars(bool rhythmSlashes) const;
	bool HasGuitarsSet(bool rhythmSlashes) const;

// Operations
public:
        std::string GetText(bool rhythmSlashes) const;

};

}

#endif // GUITARIN_H
