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

#ifndef __POWERTABDOC_H__
#define __POWERTABDOC_H__

#include "powertabfileheader.h"
#include "score.h"
#include "fontsetting.h"

// compatibility classes
#include "oldrehearsalsign.h"
#include "oldtimesignature.h"

#define NUM_DOCUMENT_FONT_SETTINGS                  3                           ///< Number of document wide font settings
#define DOCUMENT_FONT_SETTING_CHORD_NAME            0                           ///< Font used to draw the chord name (chord text)
#define DOCUMENT_FONT_SETTING_TABLATURE_NUMBERS     1                           ///< Font used to draw the fret numbers on the tablature staff
#define DOCUMENT_FONT_LYRICS                        2                           ///< Unused

/// Loads and saves Power Tab files (.ptb)
class PowerTabDocument
{
// Constants
public:   
	// Default Constants
	static const int32_t    DEFAULT_TABLATURE_STAFF_LINE_SPACING;               ///< Default value for the tablature staff line spacing member variable
	static const uint32_t   DEFAULT_FADE_IN;                                    ///< Default value for the fade in member variable    
	static const uint32_t   DEFAULT_FADE_OUT;                                   ///< Default value for the fade out member variable
	
	// Tablature Staff Line Spacing Constants
	static const int32_t    MIN_TABLATURE_STAFF_LINE_SPACING;                   ///< Minimum allowed value for the tablature staff line spacing member variable
	static const int32_t    MAX_TABLATURE_STAFF_LINE_SPACING;                   ///< Maximum allowed value for the tablature staff line spacing member variable

	// Score Constants
	static const uint8_t     NUM_SCORES;                                         ///< Number of scores per document (guitar + bass)
	static const uint8_t     GUITAR_SCORE;                                       ///< Index of the guitar score
	static const uint8_t     BASS_SCORE;                                         ///< Index of the bass score
	
	// Guitar Constants
	static const uint8_t     MAX_GUITARS;                                        ///< Maximum number of guitars allowed per document
	
// Member Variables
protected:
	PowerTabFileHeader  m_header;                                               ///< The one and only header (contains file information)
        std::vector<Score*>          m_scoreArray;                                           ///< List of scores (zeroth element = guitar score, first element = bass score)
	
	FontSetting         m_fontSettingArray[NUM_DOCUMENT_FONT_SETTINGS];         ///< List of global font settings
	int32_t             m_tablatureStaffLineSpacing;                            ///< Amount of space used between lines on the tablature staff
	uint32_t            m_fadeIn;                                               ///< Amount of fade in at the start of the song (in MIDI units - see generalmidi.h)
	uint32_t            m_fadeOut;                                              ///< Amount of fade out at the end of the song (in MIDI units - see generalmidi.h)
        string              m_fileName; ///< Name of the currently open file

// Constructor/Destructor
public:
	PowerTabDocument();
	~PowerTabDocument();

        // Equality operator (based on file name)
        bool operator==(const PowerTabDocument& doc)
        {
            return m_fileName == doc.m_fileName;
        }
   
// Save Functions
	//wxOutputStream& SaveObject(wxOutputStream& stream);
	
// Load Functions
	//TODO - fix these
	//wxInputStream& LoadObject(wxInputStream& stream);
        bool Load(string fileName);
	bool Load(PowerTabInputStream& stream);
	bool DeserializeFormat1_7(PowerTabInputStream& stream);
	//bool DeserializeFormat1_5(PowerTabInputStream& stream);
	//bool DeserializeFormat1_0(PowerTabInputStream& stream, uint16_t version);
	
// Overrides
	virtual bool DeleteContents();

        string GetFileName()
        {
            return m_fileName;
        }
        bool SetFileName(string fileName)
        {
            m_fileName = fileName;
            return true;
        }

// Header Functions
	/// Gets a reference to the header
	/// @return A reference to the header
	PowerTabFileHeader& GetHeaderRef()      
		{return (m_header);}
	/// Gets a constant reference to the header
	/// @return A constant reference to the header
	const PowerTabFileHeader& GetHeaderConstRef() const
		{return (m_header);}
		
// Score Functions
	/// Gets a pointer to the guitar score
	/// @return A pointer to the guitar score
	Score* GetGuitarScore() const           
		{CHECK_THAT(!m_scoreArray.empty(), NULL); return (m_scoreArray[0]);}
	/// Gets a pointer to the bass score
	/// @return A pointer to the bass score
	Score* GetBassScore() const
		{CHECK_THAT(m_scoreArray.size() >= 2, NULL); return (m_scoreArray[1]);}
	bool CanMergeGuitarScore(PowerTabDocument& document) const;
	bool MergeGuitarScore(PowerTabDocument& document);
	bool CanMergeBassScore(PowerTabDocument& document) const;    
	bool MergeBassScore(PowerTabDocument& document);
	void DeleteScoreArrayContents();

// Font Setting Functions
	/// Sets the font setting used by chord names
	/// @param fontSetting Font setting to set
	void SetChordNameFontSetting(const FontSetting& fontSetting)
		{m_fontSettingArray[DOCUMENT_FONT_SETTING_CHORD_NAME] = fontSetting;}
	/// Gets the font setting used by chord names
	/// @return The font setting used by chord names
	FontSetting GetChordNameFontSetting() const
		{return (m_fontSettingArray[DOCUMENT_FONT_SETTING_CHORD_NAME]);}
	/// Gets a reference to the font setting used by chord names
	/// @return A reference to the font setting used by chord names
	FontSetting& GetChordNameFontSettingRef()
		{return (m_fontSettingArray[DOCUMENT_FONT_SETTING_CHORD_NAME]);}
	/// Gets a constant reference to the font setting used by chord names
	/// @return A constant reference to the font setting used by chord names
	const FontSetting& GetChordNameFontSettingConstRef() const
		{return (m_fontSettingArray[DOCUMENT_FONT_SETTING_CHORD_NAME]);}

	/// Sets the font setting used by tablature numbers on the tablature staff
	/// @param fontSetting Font setting to set
	void SetTablatureNumbersFontSetting(const FontSetting& fontSetting)
		{m_fontSettingArray[DOCUMENT_FONT_SETTING_TABLATURE_NUMBERS] = fontSetting;}
	/// Gets the font setting used by tablature numbers on the tablature staff
	/// @return The font setting used by tablature numbers on the tablature
	/// staff
	FontSetting GetTablatureNumbersFontSetting() const
		{return (m_fontSettingArray[DOCUMENT_FONT_SETTING_TABLATURE_NUMBERS]);}
	/// Gets a reference to the font setting used by tablature numbers on the
	/// tablature staff
	/// @return A reference to the font setting used by tablature numbers on the
	/// tablature staff
	FontSetting& GetTablatureNumbersFontSettingRef()
		{return (m_fontSettingArray[DOCUMENT_FONT_SETTING_TABLATURE_NUMBERS]);}
	/// Gets a constant reference to the font setting used by tablature numbers
	/// on the tablature staff
	/// @return A constant reference to the font setting used by tablature
	/// numbers on the tablature staff
	const FontSetting& GetTablatureNumbersFontSettingConstRef() const
		{return (m_fontSettingArray[DOCUMENT_FONT_SETTING_TABLATURE_NUMBERS]);}
		
// Tablature Staff Line Spacing Functions
	/// Determines if a tablature staff line spacing value is valid
	/// @param tablatureStaffLineSpacing Tablature staff line spacing value to
	/// validate
	/// @return True if the tablature staff line spacing is valid, false if not
	static bool IsValidTablatureStaffLineSpacing(
		int32_t tablatureStaffLineSpacing)
	{
		return ((tablatureStaffLineSpacing >= MIN_TABLATURE_STAFF_LINE_SPACING) &&
			(tablatureStaffLineSpacing <= MAX_TABLATURE_STAFF_LINE_SPACING));
	}
	/// Sets the amount of spacing between tablature staff lines
	/// @param tablatureStaffLineSpacing Amount of spacing to set
	bool SetTablatureStaffLineSpacing(uint32_t tablatureStaffLineSpacing)
	{
		CHECK_THAT(IsValidTablatureStaffLineSpacing(tablatureStaffLineSpacing), false);
		m_tablatureStaffLineSpacing = tablatureStaffLineSpacing;
		return (true);
	}
	/// Gets the amount of spacing between tablature staff lines
	/// @return The amount of spacing between tablature staff lines
	int32_t GetTablatureStaffLineSpacing() const
		{return (m_tablatureStaffLineSpacing);}
	
// Fade In Functions
	/// Sets the amount of fade in at the start of the song or lesson
	/// @param fadeIn Amount of fade in to set, in MIDI units
	/// (see generalmidi.h)
	void SetFadeIn(uint32_t fadeIn)         
		{m_fadeIn = fadeIn;}
	/// Gets the amount of fade in at the start of the song or lesson
	/// @return The amount of fade in at the start of the song or lesson, in
	/// MIDI units (see generalmidi.h)
	uint32_t GetFadeIn() const
		{return (m_fadeIn);}
	
// Fade Out Functions
	/// Sets the amount of fade out at the end of the song or lesson
	/// @param fadeOut Amount of fade out to set, in MIDI units
	/// (see generalmidi.h)
	void SetFadeOut(uint32_t fadeOut)       
		{m_fadeOut = fadeOut;}
	/// Gets the amount of fade out at the end of the song or lesson
	/// @return The amount of fade out at the end of the song or lesson, in MIDI
	/// units (see generalmidi.h)
	uint32_t GetFadeOut() const             
		{return (m_fadeOut);}
	
// Error Functions
	/*void DisplaySerializationError(const string& fileName, off_t offset,
		const string& errorMessage);
	void DisplayDeserializationError(const string& fileName, off_t offset,
		const string& errorMessage);*/
};

#endif
