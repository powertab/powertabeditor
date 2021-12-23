/////////////////////////////////////////////////////////////////////////////
// Name:            powertabdocument.h
// Purpose:         Loads and saves Power Tab files (.ptb)
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 3, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef POWER_TAB_DOCUMENT_H
#define POWER_TAB_DOCUMENT_H

#include "powertabfileheader.h"
#include "fontsetting.h"

#include <array>
#include <filesystem>
#include <vector>

namespace PowerTabDocument {

class Guitar;
class Score;

/// Loads and saves Power Tab files (.ptb)
class Document
{
public:   
    // Default Constants
    static const int32_t    DEFAULT_TABLATURE_STAFF_LINE_SPACING;   ///< Default value for the tablature staff line spacing member variable
    static const uint32_t   DEFAULT_FADE_IN;                        ///< Default value for the fade in member variable
    static const uint32_t   DEFAULT_FADE_OUT;                       ///< Default value for the fade out member variable

    // Tablature Staff Line Spacing Constants
    static const int32_t    MIN_TABLATURE_STAFF_LINE_SPACING;       ///< Minimum allowed value for the tablature staff line spacing member variable
    static const int32_t    MAX_TABLATURE_STAFF_LINE_SPACING;       ///< Maximum allowed value for the tablature staff line spacing member variable

    // Guitar Constants
    static const uint8_t     MAX_GUITARS;                           ///< Maximum number of guitars allowed per document

    enum
    {
        FONT_SETTING_CHORD_NAME,           ///< Font used to draw the chord name (chord text)
        FONT_SETTING_TABLATURE_NUMBERS,    ///< Font used to draw the fret numbers on the tablature staff
        FONT_SETTING_LYRICS,               ///< Unused
        NUM_FONT_SETTINGS                  ///< Number of document wide font settings
    };

    using PathType = std::filesystem::path;

    // Member Variables
private:
    PowerTabFileHeader  m_header;                                   ///< The one and only header (contains file information)
    std::vector<Score*> m_scoreArray;                               ///< List of scores (zeroth element = guitar score, first element = bass score)

    std::array<FontSetting, NUM_FONT_SETTINGS> m_fontSettings; ///< List of global font settings
    int32_t             m_tablatureStaffLineSpacing;                    ///< Amount of space used between lines on the tablature staff
    uint32_t            m_fadeIn;                                       ///< Amount of fade in at the start of the song (in MIDI units - see generalmidi.h)
    uint32_t            m_fadeOut;                                      ///< Amount of fade out at the end of the song (in MIDI units - see generalmidi.h)
    PathType            m_fileName;                         ///< Name of the currently open file

public:
    Document();
    ~Document();

    bool Save(const PathType& fileName) const;
    void Load(const PathType& fileName);

    bool Deserialize(PowerTabInputStream& stream);

    void DeleteContents();

    PathType GetFileName() const;
    void SetFileName(const PathType& fileName);

    PowerTabFileHeader& GetHeader();
    const PowerTabFileHeader& GetHeader() const;
    void SetHeader(const PowerTabFileHeader& header);

    // Score Functions
    size_t GetNumberOfScores() const;
    Score* GetScore(size_t index) const;
    void DeleteScoreArrayContents();

    // Font Setting Functions
    void SetChordNameFontSetting(const FontSetting& fontSetting);
    const FontSetting& GetChordNameFontSetting() const;
    FontSetting& GetChordNameFontSetting();

    void SetTabulatureNumbersFontSetting(const FontSetting& fontSetting);
    const FontSetting& GetTabulatureNumbersFontSetting() const;
    FontSetting& GetTabulatureNumbersFontSetting();

    // Tablature Staff Line Spacing Functions
    static bool IsValidTablatureStaffLineSpacing(int32_t tablatureStaffLineSpacing);
    bool SetTablatureStaffLineSpacing(uint32_t tablatureStaffLineSpacing);
    int32_t GetTablatureStaffLineSpacing() const;

    // Fade In Functions
    void SetFadeIn(uint32_t fadeIn);
    uint32_t GetFadeIn() const;

    // Fade Out Functions
    void SetFadeOut(uint32_t fadeOut);
    uint32_t GetFadeOut() const;
};

}

#endif // POWER_TAB_DOCUMENT_H
