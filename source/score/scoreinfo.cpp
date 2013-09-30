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

#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

SongData::AudioData::AudioData()
    : myReleaseType(Album),
      myYear(boost::gregorian::day_clock::local_day().year()),
      myIsLive(false)
{
}

SongData::AudioData::AudioData(SongData::AudioData::AudioReleaseType type,
                               const std::string &title, int year, bool live)
    : myReleaseType(type),
      myTitle(title),
      myYear(year),
      myIsLive(live)
{
}

bool SongData::AudioData::operator==(const AudioData &other) const
{
    return myReleaseType == other.myReleaseType &&
           myTitle == other.myTitle &&
           myYear == other.myYear &&
            myIsLive == other.myIsLive;
}

SongData::AudioData::AudioReleaseType SongData::AudioData::getReleaseType() const
{
    return myReleaseType;
}

const std::string &SongData::AudioData::getTitle()
{
    return myTitle;
}

int SongData::AudioData::getYear() const
{
    return myYear;
}

bool SongData::AudioData::isLive() const
{
    return myIsLive;
}

SongData::VideoData::VideoData()
    : myIsLive(false)
{
}

SongData::VideoData::VideoData(const std::string &title, bool live)
    : myTitle(title),
      myIsLive(live)
{
}

bool SongData::VideoData::operator==(const VideoData &other) const
{
    return myTitle == other.myTitle &&
            myIsLive == other.myIsLive;
}

const std::string &SongData::VideoData::getTitle() const
{
    return myTitle;
}

bool SongData::VideoData::isLive() const
{
    return myIsLive;
}

SongData::BootlegData::BootlegData()
    : myDate(boost::gregorian::day_clock::local_day())
{
}

SongData::BootlegData::BootlegData(const std::string &title,
                                   const boost::gregorian::date &date)
    : myTitle(title),
      myDate(date)
{
}

bool SongData::BootlegData::operator==(const BootlegData &other) const
{
    return myTitle == other.myTitle &&
            myDate == other.myDate;
}

const std::string &SongData::BootlegData::getTitle() const
{
    return myTitle;
}

const boost::gregorian::date &SongData::BootlegData::getDate() const
{
    return myDate;
}

LessonData::LessonData()
    : myMusicStyle(General),
      myDifficultyLevel(Intermediate)
{
}

SongData::AuthorData::AuthorData()
{
}

SongData::AuthorData::AuthorData(const std::string &composer,
                                 const std::string &lyricist)
    : myComposer(composer),
      myLyricist(lyricist)
{
}

bool SongData::AuthorData::operator==(const AuthorData &other) const
{
    return myComposer == other.myComposer &&
            myLyricist == other.myLyricist;
}

const std::string &SongData::AuthorData::getComposer() const
{
    return myComposer;
}

const std::string &SongData::AuthorData::getLyricist() const
{
    return myLyricist;
}

SongData::SongData()
	: myAudioReleaseInfo(AudioData()),
      myAuthorInfo(AuthorData())
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
	return myAudioReleaseInfo.is_initialized();
}

void SongData::setAudioReleaseInfo(const SongData::AudioData &info)
{
	setUnreleased();
	myAudioReleaseInfo.reset(info);
}

bool SongData::isVideoRelease() const
{
	return myVideoReleaseInfo.is_initialized();
}

void SongData::setVideoReleaseInfo(const SongData::VideoData &info)
{
    setUnreleased();
	myVideoReleaseInfo.reset(info);
}

bool SongData::isBootleg() const
{
	return myBootlegReleaseInfo.is_initialized();
}

void SongData::setBootlegInfo(const SongData::BootlegData &info)
{
	setUnreleased();
	myBootlegReleaseInfo.reset(info);
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

void SongData::setAuthorInfo(const SongData::AuthorData &info)
{
    myAuthorInfo = info;
}

const SongData::AuthorData &SongData::getAuthorInfo() const
{
    return myAuthorInfo.get();
}

void SongData::setTraditionalAuthor()
{
    myAuthorInfo.reset();
}

bool SongData::isTraditionalAuthor() const
{
    return myAuthorInfo;
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
	return mySongData.is_initialized() ? Song : Lesson;
}

const SongData &ScoreInfo::getSongData() const
{
	if (!mySongData.is_initialized())
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
	if (!myLessonData.is_initialized())
		throw std::logic_error("Invalid attempt to read lesson data");

	return *myLessonData;
}

void ScoreInfo::setLessonData(const LessonData &data)
{
	myLessonData = data;
	mySongData.reset();
}

