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
#include <boost/variant/get.hpp>

namespace Score {

SongData::AudioData::AudioData()
    : myReleaseType(Album),
      myYear(boost::gregorian::day_clock::local_day().year()),
      myIsLive(false)
{
}

bool SongData::AudioData::operator==(const AudioData &other) const
{
    return myReleaseType == other.myReleaseType &&
           myTitle == other.myTitle &&
           myYear == other.myYear &&
           myIsLive == other.myIsLive;
}

SongData::VideoData::VideoData()
    : myIsLive(false)
{
}

bool SongData::VideoData::operator==(const VideoData &other) const
{
    return myTitle == other.myTitle &&
           myIsLive == other.myIsLive;
}

SongData::BootlegData::BootlegData()
    : myDate(boost::gregorian::day_clock::local_day())
{
}

bool SongData::BootlegData::operator==(const BootlegData &other) const
{
    return myTitle == other.myTitle &&
           myDate == other.myDate;
}

LessonData::LessonData()
    : myMusicStyle(General),
      myDifficultyLevel(Intermediate)
{
}

bool SongData::AuthorData::operator==(const AuthorData &other) const
{
    return myComposer == other.myComposer &&
           myLyricist == other.myLyricist;
}

SongData::SongData()
    : myReleaseInfo(AudioData()),
      myAuthorInfo(AuthorData())
{
}

bool SongData::operator==(const SongData &other) const
{
    return myTitle == other.myTitle &&
           myArtist == other.myArtist &&
           myReleaseInfo == other.myReleaseInfo &&
           myAuthorInfo == other.myAuthorInfo &&
           myArranger == other.myArranger &&
           myTranscriber == other.myTranscriber &&
           myCopyright == other.myCopyright &&
           myLyrics == other.myLyrics &&
           myPerformanceNotes == other.myPerformanceNotes;
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

ScoreInfo::ScoreInfo()
    : myData(SongData())
{
}

bool ScoreInfo::operator==(const ScoreInfo &other) const
{
    return myData == other.myData;
}

ScoreInfo::ScoreType ScoreInfo::getScoreType() const
{
    return static_cast<ScoreType>(myData.which());
}

const SongData &ScoreInfo::getSongData() const
{
    return boost::get<SongData>(myData);
}

void ScoreInfo::setSongData(const SongData &data)
{
    myData = data;
}

const LessonData &ScoreInfo::getLessonData() const
{
    return boost::get<LessonData>(myData);
}

void ScoreInfo::setLessonData(const LessonData &data)
{
    myData = data;
}

}
