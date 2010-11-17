/////////////////////////////////////////////////////////////////////////////
// Name:            oldtimesignature.h
// Purpose:         Used to retrieve time signatures in older file versions
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 29, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __OLDTIMESIGNATURE_H__
#define __OLDTIMESIGNATURE_H__

#include "powertabobject.h"
#include <sstream>

class TimeSignature;

/// Used to retrieve time signatures in older file versions
class OldTimeSignature : public PowerTabObject
{
// Constants
public:
	static const uint16_t         DEFAULT_SYSTEM;             ///< Default value for the system member variable
	static const uint8_t         DEFAULT_POSITION;           ///< Default value for the position member variable
	static const uint32_t       DEFAULT_DATA;               ///< Default value for the data member variable

// Member Variables
public:
	uint16_t      m_system;                       ///< Zero based index of the system the time signature is anchored to
	uint8_t      m_position;                     ///< Zero based index of the position within the system where the symbol is anchored
	uint32_t    m_data;                         ///< Time signature related data
	
// Constructor/Destructor
public:
	OldTimeSignature();
	~OldTimeSignature();
	
// Construction Functions
	bool ConstructTimeSignature(TimeSignature& timeSignature) const;
	
// Operators
	const OldTimeSignature& operator=(const OldTimeSignature& oldTimeSignature);
	bool operator==(const OldTimeSignature& oldTimeSignature) const;
	bool operator!=(const OldTimeSignature& oldTimeSignature) const;
	
// MFC Class Functions
	/// Gets the MFC class id associated with the object
	/// @return The MFC class id associated with the object
	string GetMFCClassId() const              
	{
		std::stringstream returnValue;
		returnValue << GetMFCClassName() << "-" << GetMFCClassSchema();
		return returnValue.str();
	}
	/// Gets the MFC Class Name for the object
	/// @return The MFC Class Name
	string GetMFCClassName() const            
		{return "CTimeSignature";}
	/// Gets the MFC Class Schema for the object
	/// @return The MFC Class Schema
	uint16_t GetMFCClassSchema() const            
		{return ((uint16_t)1);}
		  
// Serialization Functions
protected:  
	bool Deserialize(PowerTabInputStream& stream, uint16_t version);
};

#endif
