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
#include "fileversion.h"
#include <string>

class SongData
{
public:
    class AudioReleaseInfo
    {
    public:
        enum class ReleaseType
        {
            Single,
            Ep,
            Album,
            DoubleAlbum,
            TripleAlbum,
            Boxset
        };

        AudioReleaseInfo();
        AudioReleaseInfo(ReleaseType type, const std::string &title, int year,
                         bool live);
        bool operator==(const AudioReleaseInfo &other) const;

        template <class Archive>
        void serialize(Archive &ar, const FileVersion /*version*/)
        {
            ar("release_type", myReleaseType);
            ar("title", myTitle);
            ar("year", myYear);
            ar("live", myIsLive);
        }

        ReleaseType getReleaseType() const;
        const std::string &getTitle() const;
        int getYear() const;
        bool isLive() const;

    private:
        ReleaseType myReleaseType;
        std::string myTitle;
        int myYear;
        bool myIsLive;
    };

    class VideoReleaseInfo
    {
    public:
        VideoReleaseInfo();
        VideoReleaseInfo(const std::string &title, bool live);
        bool operator==(const VideoReleaseInfo &other) const;

        template <class Archive>
        void serialize(Archive &ar, const FileVersion /*version*/)
        {
            ar("title", myTitle);
            ar("live", myIsLive);
        }

        const std::string &getTitle() const;
        bool isLive() const;

    private:
        std::string myTitle;
        bool myIsLive;
    };

    class BootlegInfo
    {
    public:
        BootlegInfo();
        BootlegInfo(const std::string &title,
                    const boost::gregorian::date &date);
        bool operator==(const BootlegInfo &other) const;

        template <class Archive>
        void serialize(Archive &ar, const FileVersion /*version*/)
        {
            ar("title", myTitle);
            ar("date", myDate);
        }

        const std::string &getTitle() const;
        const boost::gregorian::date &getDate() const;

    private:
        std::string myTitle;
        boost::gregorian::date myDate;
    };

    class AuthorInfo
    {
    public:
        AuthorInfo();
        AuthorInfo(const std::string &composer, const std::string &lyricist);
        bool operator==(const AuthorInfo &other) const;

        template <class Archive>
        void serialize(Archive &ar, const FileVersion /*version*/)
        {
            ar("composer", myComposer);
            ar("lyricist", myLyricist);
        }

        const std::string &getComposer() const;
        const std::string &getLyricist() const;

    private:
        std::string myComposer;
        std::string myLyricist;
    };

    SongData();
    bool operator==(const SongData &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    void setTitle(const std::string &title);
    const std::string &getTitle() const;

    void setArtist(const std::string &artist);
    const std::string &getArtist() const;

    bool isAudioRelease() const;
    const AudioReleaseInfo &getAudioReleaseInfo() const;
    void setAudioReleaseInfo(const AudioReleaseInfo &info);

    bool isVideoRelease() const;
    const VideoReleaseInfo &getVideoReleaseInfo() const;
    void setVideoReleaseInfo(const VideoReleaseInfo &info);

    bool isBootleg() const;
    const BootlegInfo &getBootlegInfo() const;
    void setBootlegInfo(const BootlegInfo &info);

    bool isUnreleased() const;
    void setUnreleased();

    void setAuthorInfo(const AuthorInfo &info);
    const AuthorInfo &getAuthorInfo() const;
    void setTraditionalAuthor();
    bool isTraditionalAuthor() const;

    void setArranger(const std::string &arranger);
    const std::string &getArranger() const;

    void setTranscriber(const std::string &transcriber);
    const std::string &getTranscriber() const;

    void setCopyright(const std::string &copyright);
    const std::string &getCopyright() const;

    void setLyrics(const std::string &lyrics);
    const std::string &getLyrics() const;

    void setPerformanceNotes(const std::string &notes);
    const std::string &getPerformanceNotes() const;

private:
    std::string myTitle;
    std::string myArtist;
    boost::optional<AudioReleaseInfo> myAudioReleaseInfo;
    boost::optional<VideoReleaseInfo> myVideoReleaseInfo;
    boost::optional<BootlegInfo> myBootlegReleaseInfo;
    boost::optional<AuthorInfo> myAuthorInfo;
    std::string myArranger;
    std::string myTranscriber;
    std::string myCopyright;
    std::string myLyrics;
    std::string myPerformanceNotes;
    // TODO - add performance notes and transcriber credits for each view?
};

template <class Archive>
void SongData::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("title", myTitle);
    ar("artist", myArtist);
    ar("audio_release_info", myAudioReleaseInfo);
    ar("video_release_info", myVideoReleaseInfo);
    ar("bootleg_relaese_info", myBootlegReleaseInfo);
    ar("author_info", myAuthorInfo);
    ar("arranger", myArranger);
    ar("transcriber", myTranscriber);
    ar("copyright", myCopyright);
    ar("lyrics", myLyrics);
    ar("performance_notes", myPerformanceNotes);
}

class LessonData
{
public:
    enum class MusicStyle
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

    enum class DifficultyLevel
    {
        Beginner,
        Intermediate,
        Advanced
    };

    LessonData();

    bool operator==(const LessonData &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    void setTitle(const std::string &title);
    const std::string &getTitle() const;

    void setSubtitle(const std::string &subtitle);
    const std::string &getSubtitle() const;

    void setMusicStyle(MusicStyle style);
    MusicStyle getMusicStyle() const;

    void setDifficultyLevel(DifficultyLevel level);
    DifficultyLevel getDifficultyLevel() const;

    void setAuthor(const std::string &author);
    const std::string &getAuthor() const;

    void setNotes(const std::string &notes);
    const std::string &getNotes() const;

    void setCopyright(const std::string &copyright);
    const std::string &getCopyright() const;

private:
    std::string myTitle;
    std::string mySubtitle;
    MusicStyle myMusicStyle;
    DifficultyLevel myDifficultyLevel;
    std::string myAuthor;
    std::string myNotes;
    std::string myCopyright;
};

template <class Archive>
void LessonData::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("title", myTitle);
    ar("subtitle", mySubtitle);
    ar("style", myMusicStyle);
    ar("difficulty", myDifficultyLevel);
    ar("author", myAuthor);
    ar("notes", myNotes);
    ar("copyright", myCopyright);
}

class ScoreInfo
{
public:
    enum class ScoreType
    {
        Song,
        Lesson
    };

    ScoreInfo();

    bool operator==(const ScoreInfo &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

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
    boost::optional<SongData> mySongData;
    boost::optional<LessonData> myLessonData;
};

template <class Archive>
void ScoreInfo::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("song_data", mySongData);
    ar("lesson_data", myLessonData);
}

#endif
