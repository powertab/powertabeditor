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

#ifndef SCORE_SCOREINFO_H
#define SCORE_SCOREINFO_H

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/variant.hpp>
#include <string>

namespace Score {

class SongData
{
public:
    struct AudioData
    {
        AudioData();
        bool operator==(const AudioData &other) const;

        enum AudioReleaseType
        {
            Single,
            Ep,
            Album,
            DoubleAlbum,
            TripleAlbum,
            Boxset
        };

        AudioReleaseType myReleaseType;
        std::string myTitle;
        int myYear;
        bool myIsLive;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
        {
            ar & myReleaseType & myTitle & myYear & myIsLive;
        }
    };

    struct VideoData
    {
        VideoData();
        bool operator==(const VideoData &other) const;

        std::string myTitle;
        bool myIsLive;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
        {
            ar & myTitle & myIsLive;
        }
    };

    struct BootlegData
    {
        BootlegData();
        bool operator==(const BootlegData &other) const;

        std::string myTitle;
        boost::gregorian::date myDate;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
        {
            ar & myTitle & myDate;
        }
    };

    struct AuthorData
    {
        bool operator==(const AuthorData &other) const;

        std::string myComposer;
        std::string myLyricist;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
        {
            ar & myComposer & myLyricist;
        }
    };

    SongData();
    bool operator==(const SongData &other) const;

    std::string myTitle;
    std::string myArtist;
    boost::variant<boost::blank, AudioData, VideoData, BootlegData> myReleaseInfo;
    boost::optional<AuthorData> myAuthorInfo;
    std::string myArranger;
    std::string myTranscriber;
    std::string myCopyright;
    std::string myLyrics;
    std::string myPerformanceNotes;
    // TODO - add performance notes and transcriber credits for each view?

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myTitle & myArtist & myReleaseInfo & myAuthorInfo & myArranger &
             myTranscriber & myCopyright & myLyrics & myPerformanceNotes;
    }
};

class LessonData
{
public:
    enum MusicStyle
    {
        Alternative,
        Bluegrass,
        Blues,
        Country,
        Fingerpick,
        Flamenco,
        Folk,
        Funk,
        Fusion,
        General,
        Jazz,
        Metal,
        Other,
        Pop,
        Progressive,
        Punk,
        Reggae,
        Rock,
        Swing
    };

    enum DifficultyLevel
    {
        Beginner,
        Intermediate,
        Advanced
    };

    LessonData();
    bool operator==(const LessonData &other) const;

    std::string myTitle;
    std::string mySubtitle;
    MusicStyle myMusicStyle;
    DifficultyLevel myDifficultyLevel;
    std::string myAuthor;
    std::string myNotes;
    std::string myCopyright;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myTitle & mySubtitle & myMusicStyle & myDifficultyLevel &
             myAuthor & myNotes & myCopyright;
    }
};

class ScoreInfo
{
public:
    enum ScoreType
    {
        Song,
        Lesson
    };

    ScoreInfo();
    bool operator==(const ScoreInfo &other) const;

    /// Returns the type of score (song or lesson).
    ScoreType getScoreType() const;

    /// Returns the song data (if the score type is a song).
    const SongData &getSongData() const;
    /// Set the song data (and change the score type to song).
    void setSongData(const SongData &data);

    /// Returns the lesson data (if the score type is a lesson).
    const LessonData &getLessonData() const;
    /// Set the lesson data (and change the lesson type to song).
    void setLessonData(const LessonData &data);

private:
    boost::variant<SongData, LessonData> myData;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myData;
    }
};

}

#endif
