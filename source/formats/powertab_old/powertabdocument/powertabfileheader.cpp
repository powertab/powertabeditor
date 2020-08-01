/////////////////////////////////////////////////////////////////////////////
// Name:            powertabfileheader.cpp
// Purpose:         Stores all of the information found in the header of a Power Tab file
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 4, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "powertabfileheader.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Constants (see .h for details)
const uint32_t PowerTabFileHeader::POWERTABFILE_MARKER              = 0x62617470;

const uint16_t PowerTabFileHeader::FILEVERSION_CURRENT                = PowerTabFileHeader::Version_1_7;
const uint16_t PowerTabFileHeader::NUM_FILEVERSIONS                   = 5;

const uint8_t PowerTabFileHeader::NUM_FILETYPES                      = 2;
const uint8_t PowerTabFileHeader::FILETYPE_SONG                      = 0;
const uint8_t PowerTabFileHeader::FILETYPE_LESSON                    = 1;

const uint8_t PowerTabFileHeader::CONTENTTYPE_NONE                   = 0;
const uint8_t PowerTabFileHeader::CONTENTTYPE_GUITAR                 = 0x01;
const uint8_t PowerTabFileHeader::CONTENTTYPE_BASS                   = 0x02;
const uint8_t PowerTabFileHeader::CONTENTTYPE_PERCUSSION             = 0x04;
const uint8_t PowerTabFileHeader::CONTENTTYPE_ALL                    = (CONTENTTYPE_GUITAR | CONTENTTYPE_BASS | CONTENTTYPE_PERCUSSION);

const uint8_t PowerTabFileHeader::NUM_RELEASETYPES                   = 4;
const uint8_t PowerTabFileHeader::RELEASETYPE_PUBLIC_AUDIO           = 0;
const uint8_t PowerTabFileHeader::RELEASETYPE_PUBLIC_VIDEO           = 1;
const uint8_t PowerTabFileHeader::RELEASETYPE_BOOTLEG                = 2;
const uint8_t PowerTabFileHeader::RELEASETYPE_NOTRELEASED            = 3;

const uint8_t PowerTabFileHeader::NUM_AUDIORELEASETYPES              = 6;
const uint8_t PowerTabFileHeader::AUDIORELEASETYPE_SINGLE            = 0;
const uint8_t PowerTabFileHeader::AUDIORELEASETYPE_EP                = 1;
const uint8_t PowerTabFileHeader::AUDIORELEASETYPE_ALBUM             = 2;
const uint8_t PowerTabFileHeader::AUDIORELEASETYPE_DOUBLEALBUM       = 3;
const uint8_t PowerTabFileHeader::AUDIORELEASETYPE_TRIPLEALBUM       = 4;
const uint8_t PowerTabFileHeader::AUDIORELEASETYPE_BOXSET            = 5;

const uint8_t PowerTabFileHeader::NUM_AUTHORTYPES                    = 2;
const uint8_t PowerTabFileHeader::AUTHORTYPE_AUTHORKNOWN             = 0;
const uint8_t PowerTabFileHeader::AUTHORTYPE_TRADITIONAL             = 1;

const uint16_t PowerTabFileHeader::NUM_MUSICSTYLES                    = 19;
const uint16_t PowerTabFileHeader::MUSICSTYLE_ALTERNATIVE             = 0;
const uint16_t PowerTabFileHeader::MUSICSTYLE_BLUEGRASS               = 1;
const uint16_t PowerTabFileHeader::MUSICSTYLE_BLUES                   = 2;
const uint16_t PowerTabFileHeader::MUSICSTYLE_COUNTRY                 = 3;
const uint16_t PowerTabFileHeader::MUSICSTYLE_FINGERPICK              = 4;
const uint16_t PowerTabFileHeader::MUSICSTYLE_FLAMENCO                = 5;
const uint16_t PowerTabFileHeader::MUSICSTYLE_FOLK                    = 6;
const uint16_t PowerTabFileHeader::MUSICSTYLE_FUNK                    = 7;
const uint16_t PowerTabFileHeader::MUSICSTYLE_FUSION                  = 8;
const uint16_t PowerTabFileHeader::MUSICSTYLE_GENERAL                 = 9;
const uint16_t PowerTabFileHeader::MUSICSTYLE_JAZZ                    = 10;
const uint16_t PowerTabFileHeader::MUSICSTYLE_METAL                   = 11;
const uint16_t PowerTabFileHeader::MUSICSTYLE_OTHER                   = 12;
const uint16_t PowerTabFileHeader::MUSICSTYLE_POP                     = 13;
const uint16_t PowerTabFileHeader::MUSICSTYLE_PROGRESSIVE             = 14;
const uint16_t PowerTabFileHeader::MUSICSTYLE_PUNK                    = 15;
const uint16_t PowerTabFileHeader::MUSICSTYLE_REGGAE                  = 16;
const uint16_t PowerTabFileHeader::MUSICSTYLE_ROCK                    = 17;
const uint16_t PowerTabFileHeader::MUSICSTYLE_SWING                   = 18;

const uint8_t PowerTabFileHeader::NUM_LESSONLEVELS                   = 3;
const uint8_t PowerTabFileHeader::LESSONLEVEL_BEGINNER               = 0;
const uint8_t PowerTabFileHeader::LESSONLEVEL_INTERMEDIATE           = 1;
const uint8_t PowerTabFileHeader::LESSONLEVEL_ADVANCED               = 2;

// Compatibility Constants (used by v1.0-v1.5 formats)
const uint8_t PowerTabFileHeader::NUM_RELEASEDONS                    = 11;
const uint8_t PowerTabFileHeader::RO_SINGLE                          = 0;
const uint8_t PowerTabFileHeader::RO_EP                              = 1;
const uint8_t PowerTabFileHeader::RO_LP                              = 2;
const uint8_t PowerTabFileHeader::RO_DOUBLELP                        = 3;
const uint8_t PowerTabFileHeader::RO_TRIPLELP                        = 4;
const uint8_t PowerTabFileHeader::RO_BOXSET                          = 5;
const uint8_t PowerTabFileHeader::RO_BOOTLEG                         = 6;
const uint8_t PowerTabFileHeader::RO_DEMO                            = 7;
const uint8_t PowerTabFileHeader::RO_SOUNDTRACK                      = 8;
const uint8_t PowerTabFileHeader::RO_VIDEO                           = 9;
const uint8_t PowerTabFileHeader::RO_NONE                            = 10;
		
/// Default Constructor
PowerTabFileHeader::PowerTabFileHeader()
{
	LoadDefaults();
}

/// Copy Constructor
PowerTabFileHeader::PowerTabFileHeader(const PowerTabFileHeader& header)
{
	LoadDefaults();
	*this = header;
}

// Operators
/// Assignment Operator
const PowerTabFileHeader& PowerTabFileHeader::operator=(
	const PowerTabFileHeader& header)
{
    m_version = header.m_version;
    m_fileType = header.m_fileType;

    m_songData.contentType = header.m_songData.contentType;
    m_songData.title = header.m_songData.title;
    m_songData.artist = header.m_songData.artist;
    m_songData.releaseType = header.m_songData.releaseType;

    m_songData.audioData.type = header.m_songData.audioData.type;
    m_songData.audioData.title = header.m_songData.audioData.title;
    m_songData.audioData.year = header.m_songData.audioData.year;
    m_songData.audioData.live = header.m_songData.audioData.live;

    m_songData.videoData.title = header.m_songData.videoData.title;
    m_songData.videoData.live = header.m_songData.videoData.live;

    m_songData.bootlegData.title = header.m_songData.bootlegData.title;
    m_songData.bootlegData.month = header.m_songData.bootlegData.month;
    m_songData.bootlegData.day = header.m_songData.bootlegData.day;
    m_songData.bootlegData.year = header.m_songData.bootlegData.year;

    m_songData.authorType = header.m_songData.authorType;

    m_songData.authorData.composer = header.m_songData.authorData.composer;
    m_songData.authorData.lyricist = header.m_songData.authorData.lyricist;

    m_songData.arranger = header.m_songData.arranger;

    m_songData.guitarScoreTranscriber =
            header.m_songData.guitarScoreTranscriber;
    m_songData.bassScoreTranscriber =
            header.m_songData.bassScoreTranscriber;

    m_songData.copyright = header.m_songData.copyright;

    m_songData.lyrics = header.m_songData.lyrics;

    m_songData.guitarScoreNotes = header.m_songData.guitarScoreNotes;
    m_songData.bassScoreNotes = header.m_songData.bassScoreNotes;

    m_lessonData.title = header.m_lessonData.title;
    m_lessonData.subtitle = header.m_lessonData.subtitle;
    m_lessonData.musicStyle = header.m_lessonData.musicStyle;
    m_lessonData.level = header.m_lessonData.level;
    m_lessonData.author = header.m_lessonData.author;
    m_lessonData.notes = header.m_lessonData.notes;
    m_lessonData.copyright = header.m_lessonData.copyright;

    return *this;
}

/// Equality Operator
bool PowerTabFileHeader::operator==(const PowerTabFileHeader& header) const
{
	return (
		(m_version == header.m_version) &&
		(m_fileType == header.m_fileType) &&
		
		(m_songData.contentType == header.m_songData.contentType) &&
		(m_songData.title == header.m_songData.title) &&
		(m_songData.artist == header.m_songData.artist) &&
		(m_songData.releaseType == header.m_songData.releaseType) &&
		
		(m_songData.audioData.type == header.m_songData.audioData.type) &&
		(m_songData.audioData.title == header.m_songData.audioData.title) &&
		(m_songData.audioData.year == header.m_songData.audioData.year) &&
		(m_songData.audioData.live == header.m_songData.audioData.live) &&
		
		(m_songData.videoData.title == header.m_songData.videoData.title) &&
		(m_songData.videoData.live == header.m_songData.videoData.live) &&
		
		(m_songData.bootlegData.title == header.m_songData.bootlegData.title) &&
		(m_songData.bootlegData.month == header.m_songData.bootlegData.month) &&
		(m_songData.bootlegData.day == header.m_songData.bootlegData.day) &&
		(m_songData.bootlegData.year == header.m_songData.bootlegData.year) &&
		
		(m_songData.authorType == header.m_songData.authorType) &&
		
		(m_songData.authorData.composer == header.m_songData.authorData.composer) &&
		(m_songData.authorData.lyricist == header.m_songData.authorData.lyricist) &&
		
		(m_songData.arranger == header.m_songData.arranger) &&
		
		(m_songData.guitarScoreTranscriber == header.m_songData.guitarScoreTranscriber) &&
		(m_songData.bassScoreTranscriber == header.m_songData.bassScoreTranscriber) &&
		
		(m_songData.copyright == header.m_songData.copyright) &&
		
		(m_songData.lyrics == header.m_songData.lyrics) &&
		
		(m_songData.guitarScoreNotes == header.m_songData.guitarScoreNotes) &&
		(m_songData.bassScoreNotes == header.m_songData.bassScoreNotes) &&
		
		(m_lessonData.title == header.m_lessonData.title) &&
		(m_lessonData.subtitle == header.m_lessonData.subtitle) &&
		(m_lessonData.musicStyle == header.m_lessonData.musicStyle) &&
		(m_lessonData.level == header.m_lessonData.level) &&
		(m_lessonData.author == header.m_lessonData.author) &&
		(m_lessonData.notes == header.m_lessonData.notes) &&
		(m_lessonData.copyright == header.m_lessonData.copyright)
	);
}

/// Inequality Operator
bool PowerTabFileHeader::operator!=(const PowerTabFileHeader& header) const
{
    return !operator==(header);
}

// Serialization Functions
/// Saves a PowerTabFileHeader object to a Power Tab output stream
/// @param stream Power Tab output stream to save to
/// @return True if the object was serialized, false if not
bool PowerTabFileHeader::Serialize(PowerTabOutputStream& stream) const
{
	stream << POWERTABFILE_MARKER << FILEVERSION_CURRENT << m_fileType;
	PTB_CHECK_THAT(stream.CheckState(), false);

	// Song
	if (IsSong())
	{
		stream << m_songData.contentType;
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.title);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.artist);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream << m_songData.releaseType;
		PTB_CHECK_THAT(stream.CheckState(), false);

		if (m_songData.releaseType == RELEASETYPE_PUBLIC_AUDIO)
		{
			stream << m_songData.audioData.type;
			PTB_CHECK_THAT(stream.CheckState(), false);
			
			stream.WriteMFCString(m_songData.audioData.title);
			PTB_CHECK_THAT(stream.CheckState(), false);
			
			stream << m_songData.audioData.year << m_songData.audioData.live;
			PTB_CHECK_THAT(stream.CheckState(), false);
		}
		else if (m_songData.releaseType == RELEASETYPE_PUBLIC_VIDEO)
		{
			stream.WriteMFCString(m_songData.videoData.title);
			PTB_CHECK_THAT(stream.CheckState(), false);
			
			stream << m_songData.videoData.live;
			PTB_CHECK_THAT(stream.CheckState(), false);
		}
		else if (m_songData.releaseType == RELEASETYPE_BOOTLEG)
		{
			stream.WriteMFCString(m_songData.bootlegData.title);
			PTB_CHECK_THAT(stream.CheckState(), false);
			
			stream << m_songData.bootlegData.month <<
				m_songData.bootlegData.day << m_songData.bootlegData.year;
			PTB_CHECK_THAT(stream.CheckState(), false);
		}

		stream << m_songData.authorType;
		PTB_CHECK_THAT(stream.CheckState(), false);

		if (m_songData.authorType == AUTHORTYPE_AUTHORKNOWN)
		{
			stream.WriteMFCString(m_songData.authorData.composer);
			PTB_CHECK_THAT(stream.CheckState(), false);
			
			stream.WriteMFCString(m_songData.authorData.lyricist);
			PTB_CHECK_THAT(stream.CheckState(), false);
		}
	
		stream.WriteMFCString(m_songData.arranger);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.guitarScoreTranscriber);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.bassScoreTranscriber);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.copyright);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.lyrics);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.guitarScoreNotes);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_songData.bassScoreNotes);
		PTB_CHECK_THAT(stream.CheckState(), false);
	}
	// Lesson
	else if (IsLesson())
	{
		stream.WriteMFCString(m_lessonData.title);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_lessonData.subtitle);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream << m_lessonData.musicStyle << m_lessonData.level;
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_lessonData.author);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_lessonData.notes);
		PTB_CHECK_THAT(stream.CheckState(), false);
		
		stream.WriteMFCString(m_lessonData.copyright);
		PTB_CHECK_THAT(stream.CheckState(), false);
	}
		
	return (stream.CheckState());
}

/// Loads a PowerTabFileHeader object from a Power Tab input stream
/// @param stream Power Tab input stream to load from
/// @return True if the object was deserialized, false if not
bool PowerTabFileHeader::Deserialize(PowerTabInputStream& stream)
{
	// Read the special Power Tab file marker
	uint32_t marker = 0;
	stream >> marker;
	
	if (!IsValidPowerTabFileMarker(marker))
	{
        return false;
	}
	
	// Read the file version
	stream >> m_version;
	
	if (!IsValidFileVersion(m_version))
	{
        return false;
	}
	
	// Based on the file version, deserialize
	bool returnValue = false;
	
	// Version 1.0 and 1.0.2
    if (m_version == Version_1_0 || m_version == Version_1_0_2)
		returnValue = DeserializeVersion1_0(stream);
	// Version 1.5
    else if (m_version == Version_1_5)
		returnValue = DeserializeVersion1_5(stream);
	// Version 1.7 and up
	else
		returnValue = DeserializeVersion1_7(stream);
	
    return returnValue;
}

/// Loads a v1.7 PowerTabFileHeader object from a Power Tab input stream
/// @param stream Power Tab input stream to load from
/// @return True if the object was deserialized, false if not
bool PowerTabFileHeader::DeserializeVersion1_7(PowerTabInputStream& stream)
{
	stream >> m_fileType;
	
	if (!IsValidFileType(m_fileType))
	{
        return false;
	}
	
	if (m_fileType == FILETYPE_SONG)
	{
		stream >> m_songData.contentType;
		stream.ReadMFCString(m_songData.title);
		stream.ReadMFCString(m_songData.artist);
		stream >> m_songData.releaseType;

		// CASE: Public audio release
		if (m_songData.releaseType == RELEASETYPE_PUBLIC_AUDIO)
		{
			stream >> m_songData.audioData.type;
			stream.ReadMFCString(m_songData.audioData.title);
			stream >> m_songData.audioData.year >> m_songData.audioData.live;
		}
		// CASE: Public video release
		else if (m_songData.releaseType == RELEASETYPE_PUBLIC_VIDEO)
		{
			stream.ReadMFCString(m_songData.videoData.title);
			stream >> m_songData.videoData.live;
		}
		// CASE: Bootleg release
		else if (m_songData.releaseType == RELEASETYPE_BOOTLEG)
		{
			stream.ReadMFCString(m_songData.bootlegData.title);
			stream >> m_songData.bootlegData.month >>
				m_songData.bootlegData.day >> m_songData.bootlegData.year;
		}

		stream >> m_songData.authorType;

		// CASE: Author known
		if (m_songData.authorType == AUTHORTYPE_AUTHORKNOWN)
		{
			stream.ReadMFCString(m_songData.authorData.composer);
			stream.ReadMFCString(m_songData.authorData.lyricist);
		}
		
		stream.ReadMFCString(m_songData.arranger);
		stream.ReadMFCString(m_songData.guitarScoreTranscriber);
		stream.ReadMFCString(m_songData.bassScoreTranscriber);
		stream.ReadMFCString(m_songData.copyright);
		stream.ReadMFCString(m_songData.lyrics);
		stream.ReadMFCString(m_songData.guitarScoreNotes);
		stream.ReadMFCString(m_songData.bassScoreNotes);
	}
	// Lesson
	else if (m_fileType == FILETYPE_LESSON)
	{
		stream.ReadMFCString(m_lessonData.title);
		stream.ReadMFCString(m_lessonData.subtitle);
		stream >> m_lessonData.musicStyle >> m_lessonData.level;
		stream.ReadMFCString(m_lessonData.author);
		stream.ReadMFCString(m_lessonData.notes);
		stream.ReadMFCString(m_lessonData.copyright);
	}
		
    return true;
}

/// Loads a v1.5 PowerTabFileHeader object from a Power Tab input stream
/// @param stream Power Tab input stream to load from
/// @return True if the object was deserialized, false if not
bool PowerTabFileHeader::DeserializeVersion1_5(PowerTabInputStream& stream)
{
	uint8_t releasedOn = 0, live = 0;
	uint16_t year = 0;
    std::string releaseTitle = "";

	stream.ReadMFCString(m_songData.title);
	stream.ReadMFCString(m_songData.artist);
	stream >> releasedOn;
	stream.ReadMFCString(releaseTitle);
	stream >> live;
	stream.ReadMFCString(m_songData.authorData.composer);
	stream.ReadMFCString(m_songData.authorData.lyricist);
	stream.ReadMFCString(m_songData.arranger);
	stream >> year >> m_songData.authorType;

	// Clear the composer and lyricist if the song is traditional
	if (m_songData.authorType == AUTHORTYPE_TRADITIONAL)
	{
		m_songData.authorData.composer.clear();
		m_songData.authorData.lyricist.clear();
	}

	stream.ReadMFCString(m_songData.copyright);
	stream.ReadMFCString(m_songData.lyrics);
	
	// Soundtrack becomes LP
	if (releasedOn == RO_SOUNDTRACK)
		releasedOn = RO_LP;
		
	// Bootleg becomes bootleg
	if (releasedOn == RO_BOOTLEG)
	{
		m_songData.releaseType = RELEASETYPE_BOOTLEG;
		m_songData.bootlegData.title = releaseTitle;        
		m_songData.bootlegData.day = 1;
		m_songData.bootlegData.month = 1;
		m_songData.bootlegData.year = year;
	}
	// Demo or none become not-released
	else if (releasedOn == RO_DEMO || releasedOn == RO_NONE)
		m_songData.releaseType = RELEASETYPE_NOTRELEASED;
	// Video becomes video
	else if (releasedOn == RO_VIDEO)
	{
		m_songData.releaseType = RELEASETYPE_PUBLIC_VIDEO;
		m_songData.videoData.title = releaseTitle;
		m_songData.videoData.live = live;
	}
	// Single, EP, LP, Double LP, Triple LP, Boxset, Soundtrack become audio
	// releases
	else
	{
		m_songData.releaseType = RELEASETYPE_PUBLIC_AUDIO;
		m_songData.audioData.title = releaseTitle;
		m_songData.audioData.type = releasedOn;
		m_songData.audioData.live = live;
		m_songData.audioData.year = year;
	}
	
    return true;
}

/// Loads a v1.0 or v1.0.2 format PowerTabFileHeader object from a Power Tab
/// input stream
/// @param stream Power Tab input stream to load from
/// @return True if the object was deserialized, false if not
bool PowerTabFileHeader::DeserializeVersion1_0(PowerTabInputStream& stream)
{
    uint8_t releasedOn = 0, live = 0;
	uint16_t year = 0;
    std::string releaseTitle;

	stream.ReadMFCString(m_songData.title);
	stream.ReadMFCString(m_songData.artist);
	stream >> releasedOn;
	stream.ReadMFCString(releaseTitle);
	stream >> live;
	stream.ReadMFCString(m_songData.authorData.composer);
	stream.ReadMFCString(m_songData.authorData.lyricist);
	stream.ReadMFCString(m_songData.arranger);
	stream.ReadMFCString(m_songData.guitarScoreTranscriber);
	stream >> year >> m_songData.authorType;
	
	// Clear the composer and lyricist if the song is traditional
	if (m_songData.authorType == AUTHORTYPE_TRADITIONAL)
	{
		m_songData.authorData.composer.clear();
		m_songData.authorData.lyricist.clear();
	}
	
	stream.ReadMFCString(m_songData.copyright); 
	stream.ReadMFCString(m_songData.lyrics);
	stream.ReadMFCString(m_songData.guitarScoreNotes);

	// Bootleg becomes bootleg
	if (releasedOn == RO_BOOTLEG)
	{
		m_songData.releaseType = RELEASETYPE_BOOTLEG;
		m_songData.bootlegData.title = releaseTitle;        
		m_songData.bootlegData.day = 1;
		m_songData.bootlegData.month = 1;
		m_songData.bootlegData.year = year;
	}
	// Demo becomes not-released
	else if (releasedOn == RO_DEMO)
		m_songData.releaseType = RELEASETYPE_NOTRELEASED;
	// Single, EP, LP, Double LP, Triple LP, Boxset, Soundtrack become audio
	// releases
	else
	{
		m_songData.releaseType = RELEASETYPE_PUBLIC_AUDIO;
		m_songData.audioData.title = releaseTitle;
		m_songData.audioData.type = releasedOn;
		m_songData.audioData.live = live;
		m_songData.audioData.year = year;
	}
	
    return true;
}

/// Gets the release title for a song (audio title or video title or bootleg
/// title)
/// @return The release title for the song
std::string PowerTabFileHeader::GetSongReleaseTitle() const
{
    std::string returnValue = "";

    uint8_t releaseType = GetSongReleaseType();

    // Video release
    if (releaseType == RELEASETYPE_PUBLIC_VIDEO)
        returnValue = GetSongVideoReleaseTitle();
    // Bootleg
    else if (releaseType == RELEASETYPE_BOOTLEG)
        returnValue = GetSongBootlegTitle();
    // Audio release
    else
        returnValue = GetSongAudioReleaseTitle();

    return (returnValue);
}

/// Sets the song bootleg date.
/// @param date Date to set.
/// @return True if the bootleg date was set, false if not
bool PowerTabFileHeader::SetSongBootlegDate(Util::Date date)
{
    m_songData.bootlegData.month = date.month();
    m_songData.bootlegData.day = date.day();
    m_songData.bootlegData.year = date.year();
	
    return true;
}

/// Gets the song bootleg date
/// @return The song bootleg date
Util::Date PowerTabFileHeader::GetSongBootlegDate() const
{
    return Util::Date(m_songData.bootlegData.year, m_songData.bootlegData.month,
                      m_songData.bootlegData.day);
}

// Operations
/// Loads the default settings for the header
void PowerTabFileHeader::LoadDefaults()
{
    m_version = FILEVERSION_CURRENT;
    m_fileType = FILETYPE_SONG;

    // Load default song data
    m_songData.contentType = 0;
    m_songData.title.clear();
    m_songData.artist.clear();
    m_songData.releaseType = RELEASETYPE_PUBLIC_AUDIO;

    m_songData.audioData.type = AUDIORELEASETYPE_ALBUM;
    m_songData.audioData.title.clear();
    m_songData.audioData.year = -1;
    m_songData.audioData.live = 0;

    m_songData.videoData.title.clear();
    m_songData.videoData.live = 0;

    m_songData.bootlegData.title.clear();
    m_songData.bootlegData.month = -1;
    m_songData.bootlegData.day = -1;
    m_songData.bootlegData.year = -1;

    m_songData.authorType = AUTHORTYPE_AUTHORKNOWN;

    m_songData.authorData.composer.clear();
    m_songData.authorData.lyricist.clear();

    m_songData.arranger.clear();

    m_songData.guitarScoreTranscriber.clear();
    m_songData.bassScoreTranscriber.clear();

    m_songData.copyright.clear();

    m_songData.lyrics.clear();

    m_songData.guitarScoreNotes.clear();
    m_songData.bassScoreNotes.clear();

    // Load default lesson data
    m_lessonData.title.clear();
    m_lessonData.subtitle.clear();
    m_lessonData.musicStyle = MUSICSTYLE_GENERAL;
    m_lessonData.level = LESSONLEVEL_INTERMEDIATE;
    m_lessonData.author.clear();
    m_lessonData.notes.clear();
    m_lessonData.copyright.clear();
}

}
