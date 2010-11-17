/////////////////////////////////////////////////////////////////////////////
// Name:            powertabdocument.cpp
// Purpose:         Loads and saves Power Tab files (.ptb)
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 29, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "powertabdocument.h"

#include "powertabinputstream.h"
#include "powertabstream.h"
#include <math.h>                   // Needed for pow

// Default Constants
const int32_t   PowerTabDocument::DEFAULT_TABLATURE_STAFF_LINE_SPACING      = 9;
const uint32_t  PowerTabDocument::DEFAULT_FADE_IN                           = 0;
const uint32_t  PowerTabDocument::DEFAULT_FADE_OUT                          = 0;

// Tablature Staff Line Spacing Constants
const int32_t   PowerTabDocument::MIN_TABLATURE_STAFF_LINE_SPACING          = 6;
const int32_t   PowerTabDocument::MAX_TABLATURE_STAFF_LINE_SPACING          = 14;

// Score Constants
const uint8_t    PowerTabDocument::NUM_SCORES                                = 2;
const uint8_t    PowerTabDocument::GUITAR_SCORE                              = 0;
const uint8_t    PowerTabDocument::BASS_SCORE                                = 1;

// Guitar Constants
const uint8_t    PowerTabDocument::MAX_GUITARS                               = 7;
	
// Constructor/Destructor
/// Default Constructor
PowerTabDocument::PowerTabDocument() :
	m_tablatureStaffLineSpacing(DEFAULT_TABLATURE_STAFF_LINE_SPACING),
        m_fadeIn(DEFAULT_FADE_IN), m_fadeOut(DEFAULT_FADE_OUT)
{
    m_scoreArray.push_back(new Score);
    m_scoreArray.push_back(new Score);
}

/// Destructor
PowerTabDocument::~PowerTabDocument()
{
	//------Last Checked------//
	// - Jan 26, 2005
	DeleteContents();
}

// Save Functions
/// Serializes the document to an output stream (in v1.7 format)
/// @param stream Output stream to save to
/// @return The output stream
/*wxOutputStream& PowerTabDocument::SaveObject(wxOutputStream& stream)
{
	//------Last Checked------//
	// - Jan 26, 2005
	
	// Create our special Power Tab stream
	PowerTabOutputStream data_stream(stream);
	
	// Write the header
	m_header.Serialize(data_stream);
	if (!data_stream.CheckState())
	{
		DisplaySerializationError(GetFilename(), stream.TellO(),
			data_stream.GetLastErrorMessage());
		return (stream);
	}
	
	// Write the scores
	size_t i = 0;
	size_t count = m_scoreArray.GetCount();
	for (; i < count; i++)
	{
		m_scoreArray[i]->Serialize(data_stream);
		if (!data_stream.CheckState())
		{
			DisplaySerializationError(GetFilename(), stream.TellO(),
				data_stream.GetLastErrorMessage());
			return (stream);
		}
	}
	
	// Write the document font settings    
	uint32_t fontSettingIndex = 0;
	for (; fontSettingIndex < NUM_DOCUMENT_FONT_SETTINGS; fontSettingIndex++)
	{
		m_fontSettingArray[fontSettingIndex].Serialize(data_stream);
		if (!data_stream.CheckState())
		{
			DisplaySerializationError(GetFilename(), stream.TellO(),
				data_stream.GetLastErrorMessage());
			return (stream);
		}
	}
	
	// Write the line spacing and fade values
	data_stream << m_tablatureStaffLineSpacing << m_fadeIn << m_fadeOut;
	if (!data_stream.CheckState())
	{
		DisplaySerializationError(GetFilename(), stream.TellO(),
			data_stream.GetLastErrorMessage());
		return (stream);
	}
				   
	return (stream);
}*/

// Load Functions
/// Deserializes the document from an input stream
/// @param stream Input stream to load from
/// @return Input stream
/*wxInputStream& PowerTabDocument::LoadObject(wxInputStream& stream)
{
	//------Last Checked------//
	// - Jan 26, 2005
	
	// Create our special Power Tab stream
	PowerTabInputStream data_stream(stream);
	
	// Read the header
	m_header.Deserialize(data_stream);
	if (!data_stream.CheckState())
	{
		DeleteContents();
		DisplayDeserializationError(GetFilename(), stream.TellI(),
			data_stream.GetLastErrorMessage());
		return (stream);
	}
	
	// Get the file version
	const uint16_t version = m_header.GetVersion();
	
	// v1.0 or v1.0.2
	if ((version == PowerTabFileHeader::FILEVERSION_1_0) ||
		(version == PowerTabFileHeader::FILEVERSION_1_0_2))
	{
		DeserializeFormat1_0(data_stream, version);
	}
	// v1.5
	else if (version == PowerTabFileHeader::FILEVERSION_1_5)
		DeserializeFormat1_5(data_stream);
	// v1.7
	else
		DeserializeFormat1_7(data_stream);
	
	if (!data_stream.CheckState())
	{
		DeleteContents();
		DisplayDeserializationError(GetFilename(), stream.TellI(),
			data_stream.GetLastErrorMessage());
		return (stream);
	}
	
	return (stream);
}*/

/// Loads a power tab file
/// @param fileName Full path of the file to load
/// @return True if the file was loaded, false if not
bool PowerTabDocument::Load(string fileName)
{
	// Create our special Power Tab stream, then call the sibling function
	PowerTabInputStream data_stream(fileName);
	if (data_stream.fail())
	{
		return false;
	}
        m_fileName = fileName;
	return (Load(data_stream));
}

/// Loads a power tab file from an input stream
/// @param stream Input stream to load from
/// @return True if the file was loaded, false if not
bool PowerTabDocument::Load(PowerTabInputStream& stream)
{
	//------Last Checked------//
	// - Jan 27, 2005
	CHECK_THAT(stream.CheckState(), false);
	DeleteContents();
	
	// Read the header
	m_header.Deserialize(stream);
	CHECK_THAT(stream.CheckState(), false);
	
	//const uint16_t version = m_header.GetVersion();
	
	// v1.0 or v1.0.2
	/*if (version == PowerTabFileHeader::FILEVERSION_1_0 ||
		version == PowerTabFileHeader::FILEVERSION_1_0_2)
	{
		//DeserializeFormat1_0(stream, version);
	}
	// v1.5
	else if (version == PowerTabFileHeader::FILEVERSION_1_5)
		DeserializeFormat1_5(stream);
	// v1.7
	else*/
		DeserializeFormat1_7(stream);
		
	return (stream.CheckState());
}

/// Deserializes Version 1.7 Files from an input stream
/// @param stream Input stream to read from
/// @return True if the document was deserialized, false if not
bool PowerTabDocument::DeserializeFormat1_7(PowerTabInputStream& stream)
{
	//------Last Checked------//
	// - Jan 27, 2005
	
	// Set the version
	const uint16_t version = PowerTabFileHeader::FILEVERSION_1_7;
	
	// Read the guitar score
	Score* guitarScore = new Score;
	guitarScore->Deserialize(stream, version);
	if (!stream.CheckState())
	{
		delete guitarScore;
		return (false);
	}
	m_scoreArray.push_back(guitarScore);
		
	// Read the bass score
	Score* bassScore = new Score;
	bassScore->Deserialize(stream, version);
	if (!stream.CheckState())
	{
		delete bassScore;
		return (false);
	}
	m_scoreArray.push_back(bassScore);

	// Read the document font settings
	uint32_t fontSettingIndex = 0;
	for (; fontSettingIndex < NUM_DOCUMENT_FONT_SETTINGS; fontSettingIndex++)
	{
		FontSetting fontSetting;
		fontSetting.Deserialize(stream, version);
		CHECK_THAT(stream.CheckState(), false);
		
		m_fontSettingArray[fontSettingIndex] = fontSetting;
	}
	
	// Read the line spacing and fade values
	stream >> m_tablatureStaffLineSpacing >> m_fadeIn >> m_fadeOut;
	CHECK_THAT(stream.CheckState(), false);
		
	return (true);
}

// Overrides
bool PowerTabDocument::DeleteContents()
{
	//------Last Checked------//
	// - Jan 26, 2005
	m_header.LoadDefaults();
	DeleteScoreArrayContents();
	m_tablatureStaffLineSpacing = DEFAULT_TABLATURE_STAFF_LINE_SPACING;
	uint32_t i = 0;
	for (; i < NUM_DOCUMENT_FONT_SETTINGS; i++)
		m_fontSettingArray[i] = FontSetting();
	m_fadeIn = DEFAULT_FADE_IN;
	m_fadeOut = DEFAULT_FADE_OUT;
	return (true);
}

// Score Functions
/// Determines if the guitar score in another document can be merged into the
/// current document
/// @param document Document who's guitar score we want to merge with
/// @return True if the guitar score can be merged, false if not
bool PowerTabDocument::CanMergeGuitarScore(PowerTabDocument& document) const
{
	//------Last Checked------//
	// - Jan 9, 2005
	
	// In order to merge, we must meet three requirements:
	// 1) Line spacing on the tablature staff must be the same
	// 2) All document font settings must be the same
	// 3) We must not have more than seven guitars total after the merge

	// 1)
	if (m_tablatureStaffLineSpacing != document.m_tablatureStaffLineSpacing)
		return (false);

	// 2)
	uint32_t fontSettingIndex = 0;
	for (; fontSettingIndex < NUM_DOCUMENT_FONT_SETTINGS; fontSettingIndex++)
	{
		if (m_fontSettingArray[fontSettingIndex] !=
			document.m_fontSettingArray[fontSettingIndex])
		{
			return (false);
		}
	}
	
	// 3)
	Score* guitarScore = GetGuitarScore();
	CHECK_THAT(guitarScore != NULL, false);
	Score* bassScore = document.GetBassScore();
	CHECK_THAT(bassScore != NULL, false);
	
	if ((guitarScore->GetGuitarCount() + bassScore->GetGuitarCount()) >
		MAX_GUITARS)
	{
		return (false);
	}
	
	return (true);
}

/// Merges the guitar score of another document into the current document
/// @param document The document who's guitar score will be merged
/// @return True if the document's guitar score was merged, false if not
bool PowerTabDocument::MergeGuitarScore(PowerTabDocument& document)
{
	//------Last Checked------//
	// - Jan 26, 2005
	
	// If we can't merge, bail out
	if (!CanMergeGuitarScore(document))
		return (false);
	
	// Get a pointer to the score we're merging
	Score* fromGuitarScore = document.GetGuitarScore();
	CHECK_THAT(fromGuitarScore != NULL, false);
	
	// Get the current guitar score
        //Score* oldGuitarScore = GetGuitarScore();
        //CHECK_THAT(oldGuitarScore != NULL, false);
	
	// Make a copy of the score
        //Score* mergeGuitarScore = (Score*)fromGuitarScore->CloneObject(); TODO fix this
        //CHECK_THAT(mergeGuitarScore != NULL, false);
	
	// Update the guitar score
        //m_scoreArray[0] = mergeGuitarScore;
	
	// Delete the old guitar score
        //delete oldGuitarScore;
		
	return (true);
}

/// Determines if the bass score in another document can be merged into the
/// current document
/// @param document Document who's bass score we want to merge with
/// @return True if the bass score can be merged, false if not
bool PowerTabDocument::CanMergeBassScore(PowerTabDocument& document) const
{
	//------Last Checked------//
	// - Jan 9, 2005
	
	// In order to merge, we must meet three requirements:
	// 1) Line spacing on the tablature staff must be the same
	// 2) All document font settings must be the same
	// 3) We must not have more than seven guitars total after the merge

	// 1)
	if (m_tablatureStaffLineSpacing != document.m_tablatureStaffLineSpacing)
		return (false);

	// 2)
	uint32_t i = 0;
	for (; i < NUM_DOCUMENT_FONT_SETTINGS; i++)
	{
		if (m_fontSettingArray[i] != document.m_fontSettingArray[i])
			return (false);
	}
	
	// 3)
	Score* guitarScore = GetGuitarScore();
	CHECK_THAT(guitarScore != NULL, false);
	Score* bassScore = document.GetBassScore();
	CHECK_THAT(bassScore != NULL, false);
	
	if ((guitarScore->GetGuitarCount() + bassScore->GetGuitarCount()) >
		MAX_GUITARS)
	{
		return (false);
	}
		
	return (true);
}

/// Merges the bass score of another document into the current document
/// @param document The document who's bass score will be merged
/// @return True if the document's bass score was merged, false if not
bool PowerTabDocument::MergeBassScore(PowerTabDocument& document)
{
	//------Last Checked------//
	// - Jan 26, 2005
	
	// If we can't merge, bail out
	if (!CanMergeBassScore(document))
		return (false);
	
	// Get a pointer to the score we're merging
	Score* fromBassScore = document.GetBassScore();
	CHECK_THAT(fromBassScore != NULL, false);
	
	// Get the current bass score
        /*Score* oldBassScore = GetBassScore();
	CHECK_THAT(oldBassScore != NULL, false);
	
	// Make a copy of the score
	Score* mergeBassScore = (Score*)fromBassScore->CloneObject();
	CHECK_THAT(mergeBassScore != NULL, false);
	
	// Update the bass score
	m_scoreArray[1] = mergeBassScore;
	
	// Delete the old bass score
        delete oldBassScore;*/
	
	return (true);
}

/// Deletes the contents (and frees the memory) of the score array
void PowerTabDocument::DeleteScoreArrayContents()
{
	//------Last Checked------//
	// - Dec 16, 2004
   
	size_t i = 0;
	size_t count = m_scoreArray.size();
	for (i = 0; i < count; i++)
		delete m_scoreArray[i];
	m_scoreArray.clear();
}

