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

#include "clickableitem.h"
#include "layoutinfo.h"
#include "simpletextitem.h"

#include <score/score.h>
#include <score/scoreinfo.h>
#include <QColor>
#include <QCoreApplication>
#include <QDate>

static constexpr double VERTICAL_SPACING = 10.0;
static const double AUTHOR_INFO_WIDTH = LayoutInfo::STAFF_WIDTH * 0.4;
static constexpr int TITLE_SIZE = 36;
static constexpr int SUBTITLE_SIZE = 20;
static constexpr int AUTHOR_SIZE = 14;

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
                            int font_size, const QColor &color, const QString &text)
{
    font.setPixelSize(font_size);
    auto text_item = new SimpleTextItem(text, font, TextAlignment::Top, QPen(color));

    // Center horizontally.
    text_item->setX(LayoutInfo::centerItem(0.0, LayoutInfo::STAFF_WIDTH,
                                           text_item->boundingRect().width()));
    text_item->setY(getNextY(group));

    group.addToGroup(text_item);
}

static void renderReleaseInfo(QGraphicsItemGroup &group, const QFont &font,
                              const QColor &color, const SongData &song_data)
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

        release_info = QStringLiteral("(From the %1 %2 \"%3\")");
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
        release_info = QStringLiteral("(From the Video \"%1\")").arg(
            QString::fromStdString(video_release.getTitle()));
    }
    else if (song_data.isBootleg())
    {
        const SongData::BootlegInfo &bootleg = song_data.getBootlegInfo();
        if (bootleg.getTitle().empty())
            return;

        QDate date(bootleg.getDate().year(), bootleg.getDate().month(),
                   bootleg.getDate().day());

        release_info = QStringLiteral("(From the %1 Bootleg \"%2\")").arg(
            date.toString(QStringLiteral("MMM d yyyy")),
            QString::fromStdString(bootleg.getTitle()));
    }
    else
        return;

    addCenteredText(group, font, 15, color,release_info);
}

static void addAuthorText(QGraphicsItemGroup &group, QFont font,
                          const QColor &color, const QString &text, const double y,
                          bool right_align = false)
{
    font.setPixelSize(AUTHOR_SIZE);

    auto text_item = new QGraphicsTextItem(text);
    text_item->setFont(font);
    text_item->setDefaultTextColor(color);

    const double text_width =
        std::min(AUTHOR_INFO_WIDTH, text_item->boundingRect().width());
    text_item->setTextWidth(text_width);
    if (right_align)
    {
        text_item->setX(LayoutInfo::STAFF_WIDTH - text_width);
        text_item->setHtml(
            QStringLiteral("<div align=\"right\">%1</div>").arg(text));
    }

    text_item->setY(y);

    group.addToGroup(text_item);
}

static void renderAuthorInfo(QGraphicsItemGroup &group, const QFont &font,
                              const QColor &color, const SongData &song_data)
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
            author_lines.append(
                QStringLiteral("Words and Music by %1").arg(composer));
        }
        else
        {
            if (!composer.isEmpty())
            {
                author_lines.append(
                    QStringLiteral("Music by %1").arg(composer));
            }

            if (!lyricist.isEmpty())
            {
                author_lines.append(
                    QStringLiteral("Words by %1").arg(lyricist));
            }
        }
    }

    if (!song_data.getArranger().empty())
    {
        QString arranger = QString::fromStdString(song_data.getArranger());
        author_lines.append(QStringLiteral("Arranged by %1").arg(arranger));
    }

    if (!author_lines.isEmpty())
    {
        QString author_text = author_lines.join('\n');
        addAuthorText(group, font, color, author_text, y, /* right_align */ true);
    }

    // Transcriber info.
    if (!song_data.getTranscriber().empty())
    {
        QString transcriber_text =
            QStringLiteral("Transcribed by %1")
                .arg(QString::fromStdString(song_data.getTranscriber()));

        addAuthorText(group, font, color, transcriber_text, y,
                      /* right_align */ false);
    }
}

static void renderSongInfo(QGraphicsItemGroup &group, const QFont &font,
                            const QColor &color, const SongData &song_data)
{
    if (!song_data.getTitle().empty())
    {
        addCenteredText(group, font, TITLE_SIZE, color,
                        QString::fromStdString(song_data.getTitle()));
    }

    if (!song_data.getSubtitle().empty())
    {
        addCenteredText(group, font, SUBTITLE_SIZE, color,
                        QString::fromStdString(song_data.getSubtitle()));
    }

    if (!song_data.getArtist().empty())
    {
        QString artist_info =
            QStringLiteral("As recorded by %1")
                .arg(QString::fromStdString(song_data.getArtist()));

        addCenteredText(group, font, SUBTITLE_SIZE, color, artist_info);
    }

    renderReleaseInfo(group, font, color, song_data);

    renderAuthorInfo(group, font, color, song_data);
}

static void renderLessonInfo(QGraphicsItemGroup &group, const QFont &font,
                           const QColor &color, const LessonData &lesson_data)
{
    if (!lesson_data.getTitle().empty())
    {
        addCenteredText(group, font, TITLE_SIZE, color,
                        QString::fromStdString(lesson_data.getTitle()));
    }

    if (!lesson_data.getSubtitle().empty())
    {
        addCenteredText(group, font, SUBTITLE_SIZE, color, 
                        QString::fromStdString(lesson_data.getSubtitle()));
    }

    const double y = getNextY(group);

    if (!lesson_data.getAuthor().empty())
    {
        QString author_text =
            QStringLiteral("Written by %1")
                .arg(QString::fromStdString(lesson_data.getAuthor()));

        addAuthorText(group, font, color ,author_text, y, /* right_align */ true);
    }

    const int level = static_cast<int>(lesson_data.getDifficultyLevel());
    QString level_text = QStringLiteral("Level: %1").arg(theDifficulties[level]);
    addAuthorText(group, font, color,level_text, y, /* right_align */ false);
}

QGraphicsItem *
ScoreInfoRenderer::render(const Score &score, const QColor &color,
                          const ScoreClickEvent &click_event)
{
    const ScoreInfo &score_info = score.getScoreInfo();
    QFont font(QStringLiteral("Liberation Serif"));

    ConstScoreLocation location(score);
    auto group = new ClickableGroup(
        QCoreApplication::translate("ScoreArea",
                                    "Double-click to edit score information."),
        click_event, location, ScoreItem::ScoreInfo);

    switch (score_info.getScoreType())
    {
        case ScoreInfo::ScoreType::Song:
            renderSongInfo(*group, font, color, score_info.getSongData());
            break;
        case ScoreInfo::ScoreType::Lesson:
            renderLessonInfo(*group, font, color, score_info.getLessonData());
            break;
    }

    return group;
}
