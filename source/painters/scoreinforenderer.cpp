/*
  * Copyright (C) 2016 Cameron White
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

#include "scoreinforenderer.h"

#include "layoutinfo.h"
#include "simpletextitem.h"

#include <score/scoreinfo.h>
#include <QDate>

static const double VERTICAL_SPACING = 10.0;
static const double AUTHOR_INFO_WIDTH = LayoutInfo::STAFF_WIDTH * 0.4;
static const int TITLE_SIZE = 36;
static const int SUBTITLE_SIZE = 20;
static const int AUTHOR_SIZE = 14;

static const QString theAudioReleaseTypes[] = {
    "Single", "EP", "Album", "Double Album", "Triple Album", "Boxset"
};

static const QString theDifficulties[] = {
    "Beginner", "Intermediate", "Advanced"
};

static double getNextY(const QGraphicsItemGroup &group)
{
    double bottom = group.boundingRect().bottom();
    if (!group.childItems().empty())
        bottom += VERTICAL_SPACING;

    return bottom;
}

static void addCenteredText(QGraphicsItemGroup &group, QFont font,
                            int font_size, const QString &text)
{
    font.setPointSize(font_size);
    auto text_item = new SimpleTextItem(text, font);

    // Center horizontally.
    text_item->setX(LayoutInfo::centerItem(0.0, LayoutInfo::STAFF_WIDTH,
                                           text_item->boundingRect().width()));
    text_item->setY(getNextY(group));

    group.addToGroup(text_item);
}

static void renderReleaseInfo(QGraphicsItemGroup &group, const QFont &font,
                              const SongData &song_data)
{
    QString release_info;

    if (song_data.isAudioRelease())
    {
        const SongData::AudioReleaseInfo &audio_release =
            song_data.getAudioReleaseInfo();
        if (audio_release.getTitle().empty())
            return;

        const int release_type =
            static_cast<int>(audio_release.getReleaseType());

        release_info = QString("(From the %1 %2 \"%3\")");
        release_info =
            release_info.arg(QString::number(audio_release.getYear()),
                             theAudioReleaseTypes[release_type],
                             QString::fromStdString(audio_release.getTitle()));
    }
    else if (song_data.isVideoRelease())
    {
        const SongData::VideoReleaseInfo &video_release =
            song_data.getVideoReleaseInfo();
        if (video_release.getTitle().empty())
            return;

        // TODO - handle live video / audio releases?
        release_info = QString("(From the Video \"%1\")").arg(
            QString::fromStdString(video_release.getTitle()));
    }
    else if (song_data.isBootleg())
    {
        const SongData::BootlegInfo &bootleg = song_data.getBootlegInfo();
        if (bootleg.getTitle().empty())
            return;

        QDate date(bootleg.getDate().year(), bootleg.getDate().month(),
                   bootleg.getDate().day());

        release_info = QString("(From the %1 Bootleg \"%2\")").arg(
            date.toString("MMM d yyyy"),
            QString::fromStdString(bootleg.getTitle()));
    }
    else
        return;

    addCenteredText(group, font, 15, release_info);
}

static void addAuthorText(QGraphicsItemGroup &group, QFont font,
                          const QString &text, const double y,
                          bool right_align = false)
{
    font.setPointSize(AUTHOR_SIZE);

    auto text_item = new QGraphicsTextItem(text);
    text_item->setFont(font);

    const double text_width =
        std::min(AUTHOR_INFO_WIDTH, text_item->boundingRect().width());
    text_item->setTextWidth(text_width);
    if (right_align)
    {
        text_item->setX(LayoutInfo::STAFF_WIDTH - text_width);
        text_item->setHtml(QString("<div align=\"right\">%1</div>").arg(text));
    }

    text_item->setY(y);

    group.addToGroup(text_item);
}

static void renderAuthorInfo(QGraphicsItemGroup &group, QFont font,
                             const SongData &song_data)
{
    const double y = getNextY(group);
    QStringList author_lines;

    if (!song_data.isTraditionalAuthor())
    {
        auto &author_info = song_data.getAuthorInfo();
        QString composer = QString::fromStdString(author_info.getComposer());
        QString lyricist = QString::fromStdString(author_info.getLyricist());

        if (!composer.isEmpty() && !lyricist.isEmpty() && composer == lyricist)
        {
            author_lines.append(QString("Words and Music by %1").arg(composer));
        }
        else
        {
            if (!composer.isEmpty())
                author_lines.append(QString("Music by %1").arg(composer));

            if (!lyricist.isEmpty())
                author_lines.append(QString("Words by %1").arg(lyricist));
        }
    }

    if (!song_data.getArranger().empty())
    {
        QString arranger = QString::fromStdString(song_data.getArranger());
        author_lines.append(QString("Arranged by %1").arg(arranger));
    }

    if (!author_lines.isEmpty())
    {
        QString author_text = author_lines.join('\n');
        addAuthorText(group, font, author_text, y, /* right_align */ true);
    }

    // Transcriber info.
    if (!song_data.getTranscriber().empty())
    {
        QString transcriber_text("Transcribed by %1");
        transcriber_text = transcriber_text.arg(
            QString::fromStdString(song_data.getTranscriber()));

        addAuthorText(group, font, transcriber_text, y,
                      /* right_align */ false);
    }
}

static void renderSongInfo(QGraphicsItemGroup &group, const QFont &font,
                           const SongData &song_data)
{
    if (!song_data.getTitle().empty())
    {
        addCenteredText(group, font, TITLE_SIZE,
                        QString::fromStdString(song_data.getTitle()));
    }

    if (!song_data.getArtist().empty())
    {
        QString artist_info("As recorded by %1");
        artist_info =
            artist_info.arg(QString::fromStdString(song_data.getArtist()));

        addCenteredText(group, font, SUBTITLE_SIZE, artist_info);
    }

    renderReleaseInfo(group, font, song_data);

    renderAuthorInfo(group, font, song_data);
}

static void renderLessonInfo(QGraphicsItemGroup &group, const QFont &font,
                           const LessonData &lesson_data)
{
    if (!lesson_data.getTitle().empty())
    {
        addCenteredText(group, font, TITLE_SIZE,
                        QString::fromStdString(lesson_data.getTitle()));
    }

    if (!lesson_data.getSubtitle().empty())
    {
        addCenteredText(group, font, SUBTITLE_SIZE,
                        QString::fromStdString(lesson_data.getSubtitle()));
    }

    const double y = getNextY(group);

    if (!lesson_data.getAuthor().empty())
    {
        QString author_text("Written by %1");
        author_text = author_text.arg(
            QString::fromStdString(lesson_data.getAuthor()));

        addAuthorText(group, font, author_text, y, /* right_align */ true);
    }

    const int level = static_cast<int>(lesson_data.getDifficultyLevel());
    QString level_text = QString("Level: %1").arg(theDifficulties[level]);
    addAuthorText(group, font, level_text, y, /* right_align */ false);
}

QGraphicsItem *
ScoreInfoRenderer::render(const ScoreInfo &score_info)
{
    QFont font("Liberation Serif");

    auto group = new QGraphicsItemGroup();

    switch (score_info.getScoreType())
    {
        case ScoreInfo::ScoreType::Song:
            renderSongInfo(*group, font, score_info.getSongData());
            break;
        case ScoreInfo::ScoreType::Lesson:
            renderLessonInfo(*group, font, score_info.getLessonData());
            break;
    }

    return group;
}
