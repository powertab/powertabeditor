/*
  * Copyright (C) 2013 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "scoreinfo.h"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <util/enumtostring.h>

SongData::AudioReleaseInfo::AudioReleaseInfo()
    : myReleaseType(ReleaseType::Album),
      myYear(boost::gregorian::day_clock::local_day().year()),
      myIsLive(false)
{
}

SongData::AudioReleaseInfo::AudioReleaseInfo(ReleaseType type,
                                             const std::string &title, int year,
                                             bool live)
    : myReleaseType(type), myTitle(title), myYear(year), myIsLive(live)
{
}

bool SongData::AudioReleaseInfo::operator==(const AudioReleaseInfo &other) const
{
    return myReleaseType == other.myReleaseType && myTitle == other.myTitle &&
           myYear == other.myYear && myIsLive == other.myIsLive;
}

SongData::AudioReleaseInfo::ReleaseType
SongData::AudioReleaseInfo::getReleaseType() const
{
    return myReleaseType;
}

const std::string &SongData::AudioReleaseInfo::getTitle() const
{
    return myTitle;
}

int SongData::AudioReleaseInfo::getYear() const
{
    return myYear;
}

bool SongData::AudioReleaseInfo::isLive() const
{
    return myIsLive;
}

SongData::VideoReleaseInfo::VideoReleaseInfo() : myIsLive(false)
{
}

SongData::VideoReleaseInfo::VideoReleaseInfo(const std::string &title,
                                             bool live)
    : myTitle(title), myIsLive(live)
{
}

bool SongData::VideoReleaseInfo::operator==(const VideoReleaseInfo &other) const
{
    return myTitle == other.myTitle && myIsLive == other.myIsLive;
}

const std::string &SongData::VideoReleaseInfo::getTitle() const
{
    return myTitle;
}

bool SongData::VideoReleaseInfo::isLive() const
{
    return myIsLive;
}

SongData::BootlegInfo::BootlegInfo()
{
    auto greg_date = boost::gregorian::day_clock::local_day();
    myDate = Util::Date(greg_date.year(), greg_date.month(), greg_date.day());
}

SongData::BootlegInfo::BootlegInfo(const std::string &title,
                                   const Util::Date &date)
    : myTitle(title), myDate(date)
{
}

bool SongData::BootlegInfo::operator==(const BootlegInfo &other) const
{
    return myTitle == other.myTitle && myDate == other.myDate;
}

const std::string &SongData::BootlegInfo::getTitle() const
{
    return myTitle;
}

const Util::Date &SongData::BootlegInfo::getDate() const
{
    return myDate;
}

LessonData::LessonData()
    : myMusicStyle(MusicStyle::General),
      myDifficultyLevel(DifficultyLevel::Intermediate)
{
}

SongData::AuthorInfo::AuthorInfo()
{
}

SongData::AuthorInfo::AuthorInfo(const std::string &composer,
                                 const std::string &lyricist)
    : myComposer(composer), myLyricist(lyricist)
{
}

bool SongData::AuthorInfo::operator==(const AuthorInfo &other) const
{
    return myComposer == other.myComposer && myLyricist == other.myLyricist;
}

const std::string &SongData::AuthorInfo::getComposer() const
{
    return myComposer;
}

const std::string &SongData::AuthorInfo::getLyricist() const
{
    return myLyricist;
}

SongData::SongData()
    : myAudioReleaseInfo(AudioReleaseInfo()),
      myAuthorInfo(AuthorInfo())
{
}

bool SongData::operator==(const SongData &other) const
{
    return myTitle == other.myTitle &&
           myArtist == other.myArtist &&
           myAudioReleaseInfo == other.myAudioReleaseInfo &&
           myVideoReleaseInfo == other.myVideoReleaseInfo &&
           myBootlegReleaseInfo == other.myBootlegReleaseInfo &&
           myAuthorInfo == other.myAuthorInfo &&
           myArranger == other.myArranger &&
           myTranscriber == other.myTranscriber &&
           myCopyright == other.myCopyright &&
           myLyrics == other.myLyrics &&
           myPerformanceNotes == other.myPerformanceNotes;
}

void SongData::setTitle(const std::string &title)
{
    myTitle = title;
}

const std::string &SongData::getTitle() const
{
    return myTitle;
}

void SongData::setSubtitle(const std::string &subtitle)
{
    mySubtitle = subtitle;
}

const std::string &SongData::getSubtitle() const
{
    return mySubtitle;
}

void SongData::setArtist(const std::string &artist)
{
    myArtist = artist;
}

const std::string &SongData::getArtist() const
{
    return myArtist;
}

bool SongData::isAudioRelease() const
{
    return myAudioReleaseInfo.has_value();
}

const SongData::AudioReleaseInfo &SongData::getAudioReleaseInfo() const
{
    return *myAudioReleaseInfo;
}

void SongData::setAudioReleaseInfo(const SongData::AudioReleaseInfo &info)
{
    setUnreleased();
    myAudioReleaseInfo = info;
}

bool SongData::isVideoRelease() const
{
    return myVideoReleaseInfo.has_value();
}

const SongData::VideoReleaseInfo &SongData::getVideoReleaseInfo() const
{
    return *myVideoReleaseInfo;
}

void SongData::setVideoReleaseInfo(const SongData::VideoReleaseInfo &info)
{
    setUnreleased();
    myVideoReleaseInfo = info;
}

bool SongData::isBootleg() const
{
    return myBootlegReleaseInfo.has_value();
}

const SongData::BootlegInfo &SongData::getBootlegInfo() const
{
    return *myBootlegReleaseInfo;
}

void SongData::setBootlegInfo(const SongData::BootlegInfo &info)
{
    setUnreleased();
    myBootlegReleaseInfo = info;
}

bool SongData::isUnreleased() const
{
    return !isAudioRelease() && !isVideoRelease() && !isBootleg();
}

void SongData::setUnreleased()
{
    myAudioReleaseInfo.reset();
    myVideoReleaseInfo.reset();
    myBootlegReleaseInfo.reset();
}

void SongData::setAuthorInfo(const SongData::AuthorInfo &info)
{
    myAuthorInfo = info;
}

const SongData::AuthorInfo &SongData::getAuthorInfo() const
{
    return *myAuthorInfo;
}

void SongData::setTraditionalAuthor()
{
    myAuthorInfo.reset();
}

bool SongData::isTraditionalAuthor() const
{
    return !myAuthorInfo.has_value();
}

void SongData::setArranger(const std::string &arranger)
{
    myArranger = arranger;
}

const std::string &SongData::getArranger() const
{
    return myArranger;
}

void SongData::setTranscriber(const std::string &transcriber)
{
    myTranscriber = transcriber;
}

const std::string &SongData::getTranscriber() const
{
    return myTranscriber;
}

void SongData::setCopyright(const std::string &copyright)
{
    myCopyright = copyright;
}

const std::string &SongData::getCopyright() const
{
    return myCopyright;
}

void SongData::setLyrics(const std::string &lyrics)
{
    myLyrics = lyrics;
}

const std::string &SongData::getLyrics() const
{
    return myLyrics;
}

void SongData::setPerformanceNotes(const std::string &notes)
{
    myPerformanceNotes = notes;
}

const std::string &SongData::getPerformanceNotes() const
{
    return myPerformanceNotes;
}

bool LessonData::operator==(const LessonData &other) const
{
    return myTitle == other.myTitle &&
           mySubtitle == other.mySubtitle &&
           myMusicStyle == other.myMusicStyle &&
           myDifficultyLevel == other.myDifficultyLevel &&
           myAuthor == other.myAuthor &&
           myNotes == other.myNotes &&
           myCopyright == other.myCopyright;
}

void LessonData::setTitle(const std::string &title)
{
    myTitle = title;
}

const std::string &LessonData::getTitle() const
{
    return myTitle;
}

void LessonData::setSubtitle(const std::string &subtitle)
{
    mySubtitle = subtitle;
}

const std::string &LessonData::getSubtitle() const
{
    return mySubtitle;
}

void LessonData::setMusicStyle(LessonData::MusicStyle style)
{
    myMusicStyle = style;
}

LessonData::MusicStyle LessonData::getMusicStyle() const
{
    return myMusicStyle;
}

void LessonData::setDifficultyLevel(LessonData::DifficultyLevel level)
{
    myDifficultyLevel = level;
}

LessonData::DifficultyLevel LessonData::getDifficultyLevel() const
{
    return myDifficultyLevel;
}

void LessonData::setAuthor(const std::string &author)
{
    myAuthor = author;
}

const std::string &LessonData::getAuthor() const
{
    return myAuthor;
}

void LessonData::setNotes(const std::string &notes)
{
    myNotes = notes;
}

const std::string &LessonData::getNotes() const
{
    return myNotes;
}

void LessonData::setCopyright(const std::string &copyright)
{
    myCopyright = copyright;
}

const std::string &LessonData::getCopyright() const
{
    return myCopyright;
}

ScoreInfo::ScoreInfo()
	: mySongData(SongData())
{
}

bool ScoreInfo::operator==(const ScoreInfo &other) const
{
    return mySongData == other.mySongData && myLessonData == other.myLessonData;
}

ScoreInfo::ScoreType ScoreInfo::getScoreType() const
{
    return mySongData.has_value() ? ScoreType::Song : ScoreType::Lesson;
}

const SongData &ScoreInfo::getSongData() const
{
	if (!mySongData)
		throw std::logic_error("Invalid attempt to read song data");

	return *mySongData;
}

void ScoreInfo::setSongData(const SongData &data)
{
	mySongData = data;
	myLessonData.reset();
}

const LessonData &ScoreInfo::getLessonData() const
{
	if (!myLessonData)
		throw std::logic_error("Invalid attempt to read lesson data");

	return *myLessonData;
}

void ScoreInfo::setLessonData(const LessonData &data)
{
	myLessonData = data;
	mySongData.reset();
}

using ReleaseType = SongData::AudioReleaseInfo::ReleaseType;
using MusicStyle = LessonData::MusicStyle;
using DifficultyLevel = LessonData::DifficultyLevel;

UTIL_DEFINE_ENUMTOSTRING(ReleaseType, {
    { ReleaseType::Single, "Single" },
    { ReleaseType::Ep, "EP" },
    { ReleaseType::Album, "Album" },
    { ReleaseType::DoubleAlbum, "DoubleAlbum" },
    { ReleaseType::TripleAlbum, "TripleAlbum" },
    { ReleaseType::Boxset, "BoxSet" }
})

UTIL_DEFINE_ENUMTOSTRING(MusicStyle, {
    { MusicStyle::Alternative, "Alternative" },
    { MusicStyle::Bluegrass, "Bluegrass" },
    { MusicStyle::Blues, "Blues" },
    { MusicStyle::Country, "Country" },
    { MusicStyle::Fingerpick, "Fingerpick" },
    { MusicStyle::Flamenco, "Flamenco" },
    { MusicStyle::Folk, "Folk" },
    { MusicStyle::Funk, "Funk" },
    { MusicStyle::Fusion, "Fusion" },
    { MusicStyle::General, "General" },
    { MusicStyle::Jazz, "Jazz" },
    { MusicStyle::Metal, "Metal" },
    { MusicStyle::Other, "Other" },
    { MusicStyle::Pop, "Pop" },
    { MusicStyle::Progressive, "Progressive" },
    { MusicStyle::Punk, "Punk" },
    { MusicStyle::Reggae, "Reggae" },
    { MusicStyle::Rock, "Rock" },
    { MusicStyle::Swing, "Swing" },
})

UTIL_DEFINE_ENUMTOSTRING(DifficultyLevel, {
    { DifficultyLevel::Beginner, "Beginner" },
    { DifficultyLevel::Intermediate, "Intermediate" },
    { DifficultyLevel::Advanced, "Advanced" }
})
