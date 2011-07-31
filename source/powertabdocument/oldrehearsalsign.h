/////////////////////////////////////////////////////////////////////////////
// Name:            oldrehearsalsign.h
// Purpose:         Used to retrieve rehearsal signs in older file versions
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 27, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef OLDREHEARSALSIGN_H
#define OLDREHEARSALSIGN_H

class RehearsalSign;
#include "powertabobject.h"
#include <sstream>

/// Used to retrieve rehearsal signs in older file versions
class OldRehearsalSign : public PowerTabObject
{
// Constants
public:
	static const uint16_t         DEFAULT_SYSTEM;             ///< Default value for the system member variable
	static const uint8_t         DEFAULT_POSITION;           ///< Default value for the position member variable
	static const uint32_t       DEFAULT_DATA;               ///< Default value for the data member variable
	static const int8_t         DEFAULT_LETTER;             ///< Default value for the letter member variable
	static const char*        DEFAULT_DESCRIPTION;        ///< Default value for the description member variable
	
// Member Variables
public:
	uint16_t          m_system;               ///< Zero based index of the system the rehearsal sign is anchored to
	uint8_t          m_position;             ///< Zero based index of the position within the system where the rehearsal sign is anchored
	uint32_t        m_data;                 ///< Unused data
	int8_t          m_letter;               ///< The letter used to uniquely identify the rehearsal sign (i.e. A, B, F, etc. - must be a capital letter)
        std::string        m_description;          ///< A description that indicates the passage the rehearsal sign is marking (i.e. Chorus, Intro, etc.)

// Constructor/Destructor
public:
	OldRehearsalSign();
	~OldRehearsalSign();
	
// Construction Functions
	bool ConstructRehearsalSign(RehearsalSign& rehearsalSign) const;
	
// Operators
	const OldRehearsalSign& operator=(const OldRehearsalSign& oldRehearsalSign);
	bool operator==(const OldRehearsalSign& oldRehearsalSign) const;
	bool operator!=(const OldRehearsalSign& oldRehearsalSign) const;
	
// MFC Class Functions
	/// Gets the MFC class id associated with the object
	/// @return The MFC class id associated with the object
        std::string GetMFCClassId() const
	{
		std::stringstream returnValue;
		returnValue << GetMFCClassName() << "-" << GetMFCClassSchema();
		return returnValue.str();
	}
	/// Gets the MFC Class Name for the object
	/// @return The MFC Class Name
        std::string GetMFCClassName() const
		{return "CRehearsalSign";}
	/// Gets the MFC Class Schema for the object
	/// @return The MFC Class Schema
	uint16_t GetMFCClassSchema() const            
		{return ((uint16_t)1);}
	
// Serialization Functions
protected:
	bool Deserialize(PowerTabInputStream& stream, uint16_t version);
};

#endif // OLDREHEARSALSIGN_H
