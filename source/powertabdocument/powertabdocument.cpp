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

using std::string;

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
    DeleteContents();
}

// Save Functions
/// Serializes the document to an output stream
/// @param stream Output stream to save to
/// @return The output stream
bool PowerTabDocument::Save(const std::string &fileName)
{
    // Create our special Power Tab stream
    PowerTabOutputStream stream(fileName);

    // Write the header
    m_header.Serialize(stream);
    CHECK_THAT(stream.CheckState(), false);

    // Write the scores
    for (size_t i = 0; i < m_scoreArray.size(); i++)
    {
        m_scoreArray[i]->Serialize(stream);
        CHECK_THAT(stream.CheckState(), false);
    }

    // Write the document font settings
    uint32_t fontSettingIndex = 0;
    for (; fontSettingIndex < NUM_DOCUMENT_FONT_SETTINGS; fontSettingIndex++)
    {
        m_fontSettingArray[fontSettingIndex].Serialize(stream);
        CHECK_THAT(stream.CheckState(), false);
    }

    // Write the line spacing and fade values
    stream << m_tablatureStaffLineSpacing << m_fadeIn << m_fadeOut;

    return stream.CheckState();
}

/// Loads a power tab file
/// @param fileName Full path of the file to load
/// @return True if the file was loaded, false if not
bool PowerTabDocument::Load(const string& fileName)
{
    PowerTabInputStream stream(fileName);

    if (stream.fail())
        return false;

    DeleteContents();

    // read the header
    m_header.Deserialize(stream);
    if (!stream.CheckState())
    {
        DeleteContents();
        return false;
    }

    // read the rest of the document
    Deserialize(stream);
    if (!stream.CheckState())
    {
        DeleteContents();
        return false;
    }

    m_fileName = fileName;
    return true;
}

/// Deserializes a file from an input stream
/// @param stream Input stream to read from
/// @return True if the document was deserialized, false if not
bool PowerTabDocument::Deserialize(PowerTabInputStream& stream)
{
    // Set the version
    const uint16_t version = m_header.GetVersion();

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

    return true;
}

void PowerTabDocument::DeleteContents()
{
    m_header.LoadDefaults();
    DeleteScoreArrayContents();
    m_tablatureStaffLineSpacing = DEFAULT_TABLATURE_STAFF_LINE_SPACING;
    uint32_t i = 0;
    for (; i < NUM_DOCUMENT_FONT_SETTINGS; i++)
        m_fontSettingArray[i] = FontSetting();
    m_fadeIn = DEFAULT_FADE_IN;
    m_fadeOut = DEFAULT_FADE_OUT;
}

/// Deletes the contents (and frees the memory) of the score array
void PowerTabDocument::DeleteScoreArrayContents()
{
    size_t i = 0;
    size_t count = m_scoreArray.size();
    for (i = 0; i < count; i++)
        delete m_scoreArray[i];
    m_scoreArray.clear();
}

