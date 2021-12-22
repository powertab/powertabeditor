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

#include "score.h"
#include <fstream>

namespace PowerTabDocument {

// Default Constants
const int32_t   Document::DEFAULT_TABLATURE_STAFF_LINE_SPACING      = 9;
const uint32_t  Document::DEFAULT_FADE_IN                           = 0;
const uint32_t  Document::DEFAULT_FADE_OUT                          = 0;

// Tablature Staff Line Spacing Constants
const int32_t   Document::MIN_TABLATURE_STAFF_LINE_SPACING          = 6;
const int32_t   Document::MAX_TABLATURE_STAFF_LINE_SPACING          = 14;

// Guitar Constants
const uint8_t    Document::MAX_GUITARS                               = 7;

// Constructor/Destructor
/// Default Constructor
Document::Document() :
    m_tablatureStaffLineSpacing(DEFAULT_TABLATURE_STAFF_LINE_SPACING),
    m_fadeIn(DEFAULT_FADE_IN), m_fadeOut(DEFAULT_FADE_OUT)
{
    m_scoreArray.push_back(new Score("Guitar Score"));
    m_scoreArray.push_back(new Score("Bass Score"));
}

/// Destructor
Document::~Document()
{
    DeleteContents();
}

Document::PathType Document::GetFileName() const
{
    return m_fileName;
}

void Document::SetFileName(const PathType& fileName)
{
    m_fileName = fileName;
}

PowerTabFileHeader& Document::GetHeader()
{
    return m_header;
}

const PowerTabFileHeader& Document::GetHeader() const
{
    return m_header;
}

void Document::SetHeader(const PowerTabFileHeader& header)
{
    m_header = header;
}

/// Sets the font setting used by chord names
void Document::SetChordNameFontSetting(const FontSetting& fontSetting)
{
    m_fontSettings[FONT_SETTING_CHORD_NAME] = fontSetting;
}

/// Gets the font setting used by chord names
FontSetting& Document::GetChordNameFontSetting()
{
    return m_fontSettings[FONT_SETTING_CHORD_NAME];
}

/// Gets the font setting used by chord names
const FontSetting& Document::GetChordNameFontSetting() const
{
    return m_fontSettings[FONT_SETTING_CHORD_NAME];
}

/// Sets the font setting used by tablature numbers on the tablature staff
void Document::SetTabulatureNumbersFontSetting(const FontSetting& fontSetting)
{
    m_fontSettings[FONT_SETTING_TABLATURE_NUMBERS] = fontSetting;
}

/// Gets the font setting used by tablature numbers on the tablature staff
FontSetting& Document::GetTabulatureNumbersFontSetting()
{
    return m_fontSettings[FONT_SETTING_TABLATURE_NUMBERS];
}

/// Gets the font setting used by tablature numbers on the tablature staff
const FontSetting& Document::GetTabulatureNumbersFontSetting() const
{
    return m_fontSettings[FONT_SETTING_TABLATURE_NUMBERS];
}

/// Sets the amount of fade in at the start of the song or lesson
/// @param fadeIn Amount of fade in to set, in MIDI units (see generalmidi.h)
void Document::SetFadeIn(uint32_t fadeIn)
{
    m_fadeIn = fadeIn;
}

/// Gets the amount of fade in at the start of the song or lesson
/// @return The amount of fade in at the start of the song or lesson, in MIDI units (see generalmidi.h)
uint32_t Document::GetFadeIn() const
{
    return m_fadeIn;
}

/// Determines if a tablature staff line spacing value is valid
bool Document::IsValidTablatureStaffLineSpacing(int32_t tablatureStaffLineSpacing)
{
    return (tablatureStaffLineSpacing >= MIN_TABLATURE_STAFF_LINE_SPACING &&
            tablatureStaffLineSpacing <= MAX_TABLATURE_STAFF_LINE_SPACING);
}

/// Sets the amount of spacing between tablature staff lines
bool Document::SetTablatureStaffLineSpacing(uint32_t tablatureStaffLineSpacing)
{
    PTB_CHECK_THAT(IsValidTablatureStaffLineSpacing(tablatureStaffLineSpacing), false);
    m_tablatureStaffLineSpacing = tablatureStaffLineSpacing;
    return true;
}

/// Gets the amount of spacing between tablature staff lines
int32_t Document::GetTablatureStaffLineSpacing() const
{
    return m_tablatureStaffLineSpacing;
}

/// Sets the amount of fade out at the end of the song or lesson
/// @param fadeOut Amount of fade out to set, in MIDI units (see generalmidi.h)
void Document::SetFadeOut(uint32_t fadeOut)
{
    m_fadeOut = fadeOut;
}

/// Gets the amount of fade out at the end of the song or lesson
/// @return The amount of fade out at the end of the song or lesson, in MIDI units (see generalmidi.h)
uint32_t Document::GetFadeOut() const
{
    return m_fadeOut;
}

/// Returns the number of scores (guitar, bass, etc)
size_t Document::GetNumberOfScores() const
{
    return m_scoreArray.size();
}

/// Gets a pointer to the score at index
/// @param index Which score to get
/// @return A pointer to thescore
Score* Document::GetScore(size_t index) const
{
    PTB_CHECK_THAT(index < m_scoreArray.size(), nullptr);
    return m_scoreArray[index];
}

// Save Functions
/// Serializes the document to an output stream
/// @param stream Output stream to save to
/// @return The output stream
bool Document::Save(const PathType& fileName) const
{
    std::ofstream fileStream(fileName,
                             std::ofstream::out | std::ofstream::binary);
    PowerTabOutputStream stream(fileStream);

    // Write the header
    m_header.Serialize(stream);
    PTB_CHECK_THAT(stream.CheckState(), false);

    if (m_header.GetVersion() <= PowerTabFileHeader::Version_1_7)
    {
        // Write the scores
        for (auto &score : m_scoreArray)
        {
            score->Serialize(stream);
            PTB_CHECK_THAT(stream.CheckState(), false);
        }
    }
    else // just write the guitar score for version 2.0
    {
        m_scoreArray[0]->Serialize(stream);
    }

    // Write the document font settings
    uint32_t fontSettingIndex = 0;
    for (; fontSettingIndex < NUM_FONT_SETTINGS; fontSettingIndex++)
    {
        m_fontSettings[fontSettingIndex].Serialize(stream);
        PTB_CHECK_THAT(stream.CheckState(), false);
    }

    // Write the line spacing and fade values
    stream << m_tablatureStaffLineSpacing << m_fadeIn << m_fadeOut;

    return stream.CheckState();
}

/// Loads a power tab file.
/// @param fileName Full path of the file to load.
/// @throw std::ifstream::failure
void Document::Load(const std::filesystem::path& fileName)
{
    std::ifstream fileStream(fileName,
                             std::ifstream::in | std::ifstream::binary);
    PowerTabInputStream stream(fileStream);

    DeleteContents();

    // read the header
    if (!m_header.Deserialize(stream))
    {
        throw std::runtime_error("Invalid header");
    }

    // read the rest of the document
    Deserialize(stream);

#if 0
    m_header.SetVersion(PowerTabFileHeader::FILEVERSION_CURRENT);
#endif
    m_fileName = fileName;
}

/// Deserializes a file from an input stream
/// @param stream Input stream to read from
/// @return True if the document was deserialized, false if not
bool Document::Deserialize(PowerTabInputStream& stream)
{
    // Set the version
    const uint16_t version = m_header.GetVersion();

    m_scoreArray.push_back(new Score("Guitar Score"));
    m_scoreArray.push_back(new Score("Bass Score"));
    m_scoreArray[0]->Deserialize(stream, version);
    m_scoreArray[1]->Deserialize(stream, version);

    // Read the document font settings
    for (size_t fontSettingIndex = 0; fontSettingIndex < NUM_FONT_SETTINGS;
         fontSettingIndex++)
    {
        FontSetting fontSetting;
        fontSetting.Deserialize(stream, version);

        m_fontSettings[fontSettingIndex] = fontSetting;
    }

    // Read the line spacing and fade values
    stream >> m_tablatureStaffLineSpacing >> m_fadeIn >> m_fadeOut;
    return true;
}

void Document::DeleteContents()
{
    m_header.LoadDefaults();
    DeleteScoreArrayContents();
    m_tablatureStaffLineSpacing = DEFAULT_TABLATURE_STAFF_LINE_SPACING;

    m_fontSettings.fill(FontSetting());

    m_fadeIn = DEFAULT_FADE_IN;
    m_fadeOut = DEFAULT_FADE_OUT;
}

/// Deletes the contents (and frees the memory) of the score array
void Document::DeleteScoreArrayContents()
{
    size_t i = 0;
    size_t count = m_scoreArray.size();
    for (i = 0; i < count; i++)
        delete m_scoreArray[i];
    m_scoreArray.clear();
}

}
