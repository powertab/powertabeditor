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
#include <boost/optional/optional.hpp>
#include "fileversion.h"
#include <string>

class SongData
{
public:
    class AudioData
    {
    public:
        enum AudioReleaseType
        {
            Single,
            Ep,
            Album,
            DoubleAlbum,
            TripleAlbum,
            Boxset
        };

        AudioData();
        AudioData(AudioReleaseType type, const std::string &title,
                  int year, bool live);
        bool operator==(const AudioData &other) const;

		template <class Archive>
        void serialize(Archive &ar, const FileVersion /*version*/)
		{
			ar("release_type", myReleaseType);
			ar("title", myTitle);
			ar("year", myYear);
			ar("live", myIsLive);
		}

        AudioReleaseType getReleaseType() const;
        const std::string &getTitle();
        int getYear() const;
        bool isLive() const;

    private:
        AudioReleaseType myReleaseType;
        std::string myTitle;
        int myYear;
        bool myIsLive;
    };

    class VideoData
    {
    public:
        VideoData();
        VideoData(const std::string &title, bool live);
        bool operator==(const VideoData &other) const;

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

    class BootlegData
    {
    public:
        BootlegData();
        BootlegData(const std::string &title, const boost::gregorian::date &date);
        bool operator==(const BootlegData &other) const;

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

    class AuthorData
    {
    public:
        AuthorData();
        AuthorData(const std::string &composer, const std::string &lyricist);
        bool operator==(const AuthorData &other) const;

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
    void setAudioReleaseInfo(const AudioData &info);
    bool isVideoRelease() const;
    void setVideoReleaseInfo(const VideoData &info);
    bool isBootleg() const;
    void setBootlegInfo(const BootlegData &info);
    bool isUnreleased() const;
    void setUnreleased();

    void setAuthorInfo(const AuthorData &info);
    const AuthorData &getAuthorInfo() const;
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
	boost::optional<AudioData> myAudioReleaseInfo;
	boost::optional<VideoData> myVideoReleaseInfo;
	boost::optional<BootlegData> myBootlegReleaseInfo;
    boost::optional<AuthorData> myAuthorInfo;
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
    enum ScoreType
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
