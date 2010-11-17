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
		
	return (stream.CheckState());
}
