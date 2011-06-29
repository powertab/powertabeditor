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
#include "powertaboutputstream.h"

// compatibility classes
#include "oldrehearsalsign.h"
#include "oldtimesignature.h"

#include <fstream>

#include "score.h"

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

std::string PowerTabDocument::GetFileName() const
{
    return m_fileName;
}

void PowerTabDocument::SetFileName(const std::string& fileName)
{
    m_fileName = fileName;
}

PowerTabFileHeader& PowerTabDocument::GetHeader()
{
    return m_header;
}

const PowerTabFileHeader& PowerTabDocument::GetHeader() const
{
    return m_header;
}

/// Sets the font setting used by chord names
void PowerTabDocument::SetChordNameFontSetting(const FontSetting& fontSetting)
{
    m_fontSettings[FONT_SETTING_CHORD_NAME] = fontSetting;
}

/// Gets the font setting used by chord names
FontSetting& PowerTabDocument::GetChordNameFontSetting()
{
    return m_fontSettings[FONT_SETTING_CHORD_NAME];
}

/// Gets the font setting used by chord names
const FontSetting& PowerTabDocument::GetChordNameFontSetting() const
{
    return m_fontSettings[FONT_SETTING_CHORD_NAME];
}

/// Sets the font setting used by tablature numbers on the tablature staff
void PowerTabDocument::SetTabulatureNumbersFontSetting(const FontSetting& fontSetting)
{
    m_fontSettings[FONT_SETTING_TABLATURE_NUMBERS] = fontSetting;
}

/// Gets the font setting used by tablature numbers on the tablature staff
FontSetting& PowerTabDocument::GetTabulatureNumbersFontSetting()
{
    return m_fontSettings[FONT_SETTING_TABLATURE_NUMBERS];
}

/// Gets the font setting used by tablature numbers on the tablature staff
const FontSetting& PowerTabDocument::GetTabulatureNumbersFontSetting() const
{
    return m_fontSettings[FONT_SETTING_TABLATURE_NUMBERS];
}

/// Sets the amount of fade in at the start of the song or lesson
/// @param fadeIn Amount of fade in to set, in MIDI units (see generalmidi.h)
void PowerTabDocument::SetFadeIn(uint32_t fadeIn)
{
    m_fadeIn = fadeIn;
}

/// Gets the amount of fade in at the start of the song or lesson
/// @return The amount of fade in at the start of the song or lesson, in MIDI units (see generalmidi.h)
uint32_t PowerTabDocument::GetFadeIn() const
{
    return m_fadeIn;
}

/// Determines if a tablature staff line spacing value is valid
bool PowerTabDocument::IsValidTablatureStaffLineSpacing(int32_t tablatureStaffLineSpacing)
{
    return (tablatureStaffLineSpacing >= MIN_TABLATURE_STAFF_LINE_SPACING &&
            tablatureStaffLineSpacing <= MAX_TABLATURE_STAFF_LINE_SPACING);
}

/// Sets the amount of spacing between tablature staff lines
bool PowerTabDocument::SetTablatureStaffLineSpacing(uint32_t tablatureStaffLineSpacing)
{
    CHECK_THAT(IsValidTablatureStaffLineSpacing(tablatureStaffLineSpacing), false);
    m_tablatureStaffLineSpacing = tablatureStaffLineSpacing;
    return true;
}

/// Gets the amount of spacing between tablature staff lines
int32_t PowerTabDocument::GetTablatureStaffLineSpacing() const
{
    return m_tablatureStaffLineSpacing;
}

/// Sets the amount of fade out at the end of the song or lesson
/// @param fadeOut Amount of fade out to set, in MIDI units (see generalmidi.h)
void PowerTabDocument::SetFadeOut(uint32_t fadeOut)
{
    m_fadeOut = fadeOut;
}

/// Gets the amount of fade out at the end of the song or lesson
/// @return The amount of fade out at the end of the song or lesson, in MIDI units (see generalmidi.h)
uint32_t PowerTabDocument::GetFadeOut() const
{
    return m_fadeOut;
}

/// Returns the guitar score
Score* PowerTabDocument::GetGuitarScore() const
{
    CHECK_THAT(!m_scoreArray.empty(), NULL);
    return m_scoreArray[GUITAR_SCORE];
}

/// Gets a pointer to the bass score
/// @return A pointer to the bass score
Score* PowerTabDocument::GetBassScore() const
{
    CHECK_THAT(m_scoreArray.size() >= 2, NULL);
    return m_scoreArray[BASS_SCORE];
}

// Save Functions
/// Serializes the document to an output stream
/// @param stream Output stream to save to
/// @return The output stream
bool PowerTabDocument::Save(const string& fileName) const
{
    std::ofstream fileStream(fileName.c_str(), std::ofstream::out | std::ofstream::binary);
    PowerTabOutputStream stream(fileStream);

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
    for (; fontSettingIndex < NUM_FONT_SETTINGS; fontSettingIndex++)
    {
        m_fontSettings[fontSettingIndex].Serialize(stream);
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
    try
    {
        std::ifstream fileStream(fileName.c_str(), std::ifstream::in | std::ifstream::binary);
        PowerTabInputStream stream(fileStream);

        DeleteContents();

        // read the header
        if (!m_header.Deserialize(stream))
        {
            return false;
        }

        // read the rest of the document
        Deserialize(stream);

        m_header.SetVersion(PowerTabFileHeader::FILEVERSION_CURRENT);
        m_fileName = fileName;
    }
    catch (std::ifstream::failure)
    {
        return false;
    }

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
    m_scoreArray.push_back(new Score);
    m_scoreArray[0]->Deserialize(stream, version);

    // Read the bass score
    m_scoreArray.push_back(new Score);
    m_scoreArray[1]->Deserialize(stream, version);

    m_scoreArray[0]->MergeScore(*m_scoreArray[1]);

    // Read the document font settings
    for (size_t fontSettingIndex = 0; fontSettingIndex < NUM_FONT_SETTINGS; fontSettingIndex++)
    {
        FontSetting fontSetting;
        fontSetting.Deserialize(stream, version);

        m_fontSettings[fontSettingIndex] = fontSetting;
    }

    // Read the line spacing and fade values
    stream >> m_tablatureStaffLineSpacing >> m_fadeIn >> m_fadeOut;
    return true;
}

void PowerTabDocument::DeleteContents()
{
    m_header.LoadDefaults();
    DeleteScoreArrayContents();
    m_tablatureStaffLineSpacing = DEFAULT_TABLATURE_STAFF_LINE_SPACING;

    m_fontSettings.fill(FontSetting());

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

/// Initializes the document to an empty score (only necessary for creating new documents, not opening existing ones)
void PowerTabDocument::Init()
{
    GetGuitarScore()->Init();
}
