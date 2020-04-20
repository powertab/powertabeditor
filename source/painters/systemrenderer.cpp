/*
  * Copyright (C) 2011 Cameron White
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

#include "systemrenderer.h"

#include <app/pubsub/clickpubsub.h>
#include <app/scorearea.h>
#include <app/viewoptions.h>
#include <boost/algorithm/clamp.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <painters/antialiasedpathitem.h>
#include <painters/barlinepainter.h>
#include <painters/clickablegroup.h>
#include <painters/keysignaturepainter.h>
#include <painters/layoutinfo.h>
#include <painters/simpletextitem.h>
#include <painters/staffpainter.h>
#include <painters/stdnotationnote.h>
#include <painters/timesignaturepainter.h>
#include <painters/verticallayout.h>
#include <QBrush>
#include <QDebug>
#include <QGraphicsItem>
#include <QPen>
#include <score/score.h>
#include <score/scorelocation.h>
#include <score/system.h>
#include <score/utils.h>
#include <score/voiceutils.h>

void SystemRenderer::centerHorizontally(QGraphicsItem &item, double xmin,
                                        double xmax)
{
    double itemWidth = item.boundingRect().width();
    double centredX = xmin + ((xmax - (xmin + itemWidth)) / 2);
    item.setX(centredX);
}

void SystemRenderer::centerSymbolVertically(QGraphicsItem &item, double y)
{
    item.setY(y + 0.5 * (LayoutInfo::SYSTEM_SYMBOL_SPACING -
                         item.boundingRect().height()));
}

SystemRenderer::SystemRenderer(const ScoreArea *score_area, const Score &score,
                               const ViewOptions &view_options)
    : myScoreArea(score_area),
      myScore(score),
      myViewOptions(view_options),
      myParentSystem(nullptr),
      myParentStaff(nullptr),
      myMusicNotationFont(MusicFont::getFont(MusicFont::DEFAULT_FONT_SIZE)),
      myMusicFontMetrics(myMusicNotationFont),
      myPlainTextFont("Liberation Sans"),
      mySymbolTextFont("Liberation Sans"),
      myRehearsalSignFont("Helvetica")
{
    myPlainTextFont.setPixelSize(10);
    myPlainTextFont.setStyleStrategy(QFont::PreferAntialias);
    mySymbolTextFont.setPixelSize(9);
    myRehearsalSignFont.setPixelSize(12);
}

QGraphicsItem *SystemRenderer::operator()(const System &system,
                                          int systemIndex)
{
    // Draw the bounding rectangle for the system.
    myParentSystem = new QGraphicsRectItem();
    myParentSystem->setPen(QPen(QBrush(QColor(0, 0, 0, 127)), 0.5));

    const ViewFilter *filter =
        myViewOptions.getFilter()
            ? &myScore.getViewFilters()[*myViewOptions.getFilter()]
            : nullptr;

    // Draw each staff.
    double height = 0;
    int i = 0;
    for (const Staff &staff : system.getStaves())
    {
        if (filter && !filter->accept(myScore, systemIndex, i))
        {
            ++i;
            continue;
        }

        const bool isFirstStaff = (height == 0);
        LayoutConstPtr layout = std::make_shared<LayoutInfo>(
            myScore, system, systemIndex, staff, i);

        if (isFirstStaff)
        {
            drawSystemSymbols(system, *layout);
            height += layout->getSystemSymbolSpacing();
        }

        myParentStaff = new StaffPainter(layout,
                                         ScoreLocation(myScore, systemIndex, i),
                                         myScoreArea->getClickPubSub());
        myParentStaff->setPos(0, height);
        myParentStaff->setParentItem(myParentSystem);
        height += layout->getStaffHeight();

        if (isFirstStaff)
            drawBarNumber(systemIndex, *layout);

        // Draw the clefs.
        const double CLEF_OFFSET =
            (staff.getClefType() == Staff::TrebleClef) ? -6 : -21;
        auto pubsub = myScoreArea->getClickPubSub();
        const ScoreLocation location(myScore, systemIndex, i);
        auto clef = new SimpleTextItem(staff.getClefType() == Staff::TrebleClef
                                           ? QChar(MusicFont::TrebleClef)
                                           : QChar(MusicFont::BassClef),
                                       myMusicNotationFont);
        auto group = new ClickableGroup(
            QObject::tr("Click to change clef type."), [=]() {
            pubsub->publish(ClickType::Clef, location);
        });
        group->addToGroup(clef);
        group->setPos(LayoutInfo::CLEF_PADDING,
                      layout->getTopStdNotationLine() + CLEF_OFFSET);
        group->setParentItem(myParentStaff);

        drawTabClef(LayoutInfo::CLEF_PADDING, *layout, location);

        drawBarlines(system, systemIndex, layout, location.getStaffIndex());
        drawTabNotes(staff, layout);
        drawLegato(staff, *layout);
        drawSlides(staff, *layout);

        drawSymbolsAboveStdNotationStaff(*layout);
        drawSymbolsBelowStdNotationStaff(*layout);
        drawSymbolsAboveTabStaff(staff, *layout);
        drawSymbolsBelowTabStaff(*layout);

        drawPlayerChanges(system, i, *layout);
        drawStdNotation(system, staff, *layout);

        ++i;
    }

    myParentSystem->setRect(0, 0, LayoutInfo::STAFF_WIDTH, height);
    return myParentSystem;
}

void SystemRenderer::drawTabClef(double x, const LayoutInfo &layout,
                                 const ScoreLocation &location)
{
    // Determine the size of the clef symbol based on the number of strings and
    // the line spacing.
    const int pixel_size =
        (layout.getStringCount() - 1) * layout.getTabLineSpacing() * 0.6;
    QFont font = MusicFont::getFont(pixel_size);

    auto clef = new SimpleTextItem(QChar(MusicFont::TabClef), font);

    auto pubsub = myScoreArea->getClickPubSub();
    auto group = new ClickableGroup(
        QObject::tr("Click to edit the number of strings."), [=]() {
        pubsub->publish(ClickType::TabClef, location);
    });
    group->addToGroup(clef);

    // Position the clef symbol.
    group->setPos(x, layout.getTopTabLine() - pixel_size / 2.1);
    group->setParentItem(myParentStaff);
}

void SystemRenderer::drawBarNumber(int systemIndex, const LayoutInfo &layout)
{
    int number = 1;
    for (int i = 0; i < systemIndex; ++i)
    {
        const System &system = myScore.getSystems()[i];
        number += static_cast<int>(system.getBarlines().size()) - 1;
    }

    auto text = new SimpleTextItem(QString::number(number), myPlainTextFont);
    text->setPos(-text->boundingRect().width() - LayoutInfo::BAR_NUMBER_PADDING,
                 layout.getTopStdNotationLine());
    text->setParentItem(myParentStaff);
}

void SystemRenderer::drawBarlines(const System &system, int systemIndex,
                                  const LayoutConstPtr &layout,
                                  int staffIndex)
{
    for (const Barline &barline : system.getBarlines())
    {
        const ScoreLocation location(myScore, systemIndex, staffIndex,
                                     barline.getPosition());

        const KeySignature &keySig = barline.getKeySignature();
        const TimeSignature &timeSig = barline.getTimeSignature();

        BarlinePainter *barlinePainter = new BarlinePainter(layout, barline,
                location, myScoreArea->getClickPubSub());

        double x = layout->getPositionX(barline.getPosition());
        double keySigX = x + barlinePainter->boundingRect().width() - 1;
        double timeSigX = x + barlinePainter->boundingRect().width() +
                layout->getWidth(keySig);
        double rehearsalSignX = x + 0.5 * layout->getPositionSpacing();

        if (barline == system.getBarlines().front()) // Start bar of system.
        {
            // For normal bars, display a line at the far left edge.
            if (barline.getBarType() == Barline::SingleBar)
            {
                x = 0 - barlinePainter->boundingRect().width() / 2 - 0.5;
                rehearsalSignX = 0;
            }
            else
            {
                // Otherwise, display the bar after the clef, etc, and to the
                // left of the first note.
                x = layout->getFirstPositionX() - layout->getPositionSpacing();
                rehearsalSignX = x + 0.5 * layout->getPositionSpacing();
            }

            keySigX = LayoutInfo::CLEF_WIDTH;
            timeSigX = LayoutInfo::CLEF_WIDTH + layout->getWidth(keySig);
        }
        else if (barline == system.getBarlines().back()) // End bar.
        {
            x = LayoutInfo::STAFF_WIDTH -
                    barlinePainter->boundingRect().width() / 2 - 1;

            if (barline.getBarType() == Barline::RepeatEnd)
                x -= 6; // bit of a positioning hack.
            else if (barline.getBarType() == Barline::SingleBar)
                x += 1 ;
            else if (barline.getBarType() == Barline::FreeTimeBar)
                x += 2;
        }

        barlinePainter->setPos(x, 0);
        barlinePainter->setParentItem(myParentStaff);

        if (keySig.isVisible())
        {
            KeySignaturePainter *keySigPainter = new KeySignaturePainter(
                        layout, keySig, location,
                        myScoreArea->getClickPubSub());

            keySigPainter->setPos(keySigX, layout->getTopStdNotationLine());
            keySigPainter->setParentItem(myParentStaff);
        }

        if (timeSig.isVisible())
        {
            TimeSignaturePainter *timeSigPainter = new TimeSignaturePainter(
                        layout, timeSig, location,
                        myScoreArea->getClickPubSub());

            timeSigPainter->setPos(timeSigX, layout->getTopStdNotationLine());
            timeSigPainter->setParentItem(myParentStaff);
        }

        if (barline.hasRehearsalSign() && staffIndex == 0)
        {
            const RehearsalSign &sign = barline.getRehearsalSign();
            const int RECTANGLE_OFFSET = 4;

            auto signLetters = new SimpleTextItem(
                QString::fromStdString(sign.getLetters()), myRehearsalSignFont);
            signLetters->setX(rehearsalSignX + RECTANGLE_OFFSET);
            centerSymbolVertically(*signLetters, 0);

            QFontMetricsF metrics(myRehearsalSignFont);
            const Barline *nextBar = system.getNextBarline(barline.getPosition());
            Q_ASSERT(nextBar);
            const double signTextX =
                signLetters->x() + signLetters->boundingRect().width() + 7;
            // If the description is too wide, cut it off with an ellipsis.
            QString shortenedSignText = metrics.elidedText(
                QString::fromStdString(sign.getDescription()), Qt::ElideRight,
                layout->getPositionX(nextBar->getPosition()) - signTextX -
                    RECTANGLE_OFFSET);

            auto signText =
                new SimpleTextItem(shortenedSignText, myRehearsalSignFont);
            signText->setX(signTextX);
            centerSymbolVertically(*signText, 0);
            // The tooltip should contain the full description.
            signText->setToolTip(QString::fromStdString(sign.getDescription()));

            // Draw rectangle around rehearsal sign letters.
            QRectF boundingRect = signLetters->boundingRect();
            boundingRect.setWidth(boundingRect.width() + 7);
            auto rect = new QGraphicsRectItem(boundingRect);
            rect->setX(rehearsalSignX);
            centerSymbolVertically(*rect, 0);

            rect->setParentItem(myParentSystem);
            signText->setParentItem(myParentSystem);
            signLetters->setParentItem(myParentSystem);
        }
    }
}

void SystemRenderer::drawTabNotes(const Staff &staff,
                                  const LayoutConstPtr &layout)
{
    for (const Voice &voice : staff.getVoices())
    {
        for (const Position &pos : voice.getPositions())
        {
            if (pos.isRest() || pos.getNotes().empty())
                continue;

            const double location = layout->getPositionX(pos.getPosition());

            for (const Note &note : pos.getNotes())
            {
                const QString text = QString::fromStdString(
                            boost::lexical_cast<std::string>(note));

                auto tabNote = new SimpleTextItem(
                    text, myPlainTextFont,
                    QPen(note.hasProperty(Note::Tied) ? Qt::lightGray
                                                      : Qt::black),
                    QBrush(QColor(255, 255, 255)));

                centerHorizontally(*tabNote, location,
                                   location + layout->getPositionSpacing());
                tabNote->setY(layout->getTabLine(note.getString() + 1) -
                              0.6 * myPlainTextFont.pixelSize());
                tabNote->setParentItem(myParentStaff);
            }

            // Draw arpeggios if necessary.
            if (pos.hasProperty(Position::ArpeggioDown) ||
                pos.hasProperty(Position::ArpeggioUp))
            {
                drawArpeggio(pos, location, *layout);
            }
        }
    }
}

void SystemRenderer::drawArpeggio(const Position &position, double x,
                                  const LayoutInfo& layout)
{
    // Get the highest and lowest strings used at this position, and
    // convert the string indices to positions on the staff.
    const double top = layout.getTabLine(
                position.getNotes().front().getString() + 1);
    const double bottom = layout.getTabLine(
                position.getNotes().back().getString() + 1);
    const double height = bottom - top;

    // Take a vibrato segment, spanning the distance from top to bottom note,
    // and then rotate it by 90 degrees.
    const QChar arpeggioSymbol = MusicFont::Vibrato;
    const double symbolWidth = myMusicFontMetrics.width(arpeggioSymbol);
    const int numSymbols = height / symbolWidth;

    auto arpeggio = new SimpleTextItem(QString(numSymbols, arpeggioSymbol),
                                       myMusicNotationFont);
    arpeggio->setPos(x + arpeggio->boundingRect().height() / 2.0 - 3.0, top);
    arpeggio->setRotation(90);
    arpeggio->setParentItem(myParentStaff);

    // Draw the end of the arpeggio.
    const QChar arpeggioEnd = position.hasProperty(Position::ArpeggioUp) ?
                MusicFont::ArpeggioUp : MusicFont::ArpeggioDown;

    auto endPoint = new SimpleTextItem(arpeggioEnd, myMusicNotationFont);
    const double y = position.hasProperty(Position::ArpeggioUp) ? top : bottom;
    endPoint->setPos(x, y - 1.45 * myMusicNotationFont.pixelSize());
    endPoint->setParentItem(myParentStaff);
}

void SystemRenderer::drawSystemSymbols(const System &system,
                                       const LayoutInfo& layout)
{
    double height = 0;

    // Allocate space for any rehearsal signs - they are drawn at the same
    // time as barlines.
    for (const Barline &barline : system.getBarlines())
    {
        if (barline.hasRehearsalSign())
        {
            height += LayoutInfo::SYSTEM_SYMBOL_SPACING;
            drawDividerLine(height);
            break;
        }
    }

    if (!system.getAlternateEndings().empty())
    {
        drawAlternateEndings(system, layout, height);
        height += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(height);
    }

    if (!system.getTempoMarkers().empty())
    {
        drawTempoMarkers(system, layout, height);
        height += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(height);
    }

    if (!system.getDirections().empty())
    {
        height += drawDirections(system, layout, height);
        drawDividerLine(height);
    }

    if (!system.getChords().empty())
    {
        drawChordText(system, layout, height);
        height += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(height);
    }

    if (!system.getTextItems().empty())
    {
        drawTextItems(system, layout, height);
        height += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(height);
    }
}

void SystemRenderer::drawDividerLine(double y)
{
    auto line = new QGraphicsLineItem();
    line->setLine(0, y, LayoutInfo::STAFF_WIDTH, y);
    line->setOpacity(0.5);
    line->setPen(QPen(Qt::black, 0.5, Qt::DashLine));

    line->setParentItem(myParentSystem);
}

void SystemRenderer::drawAlternateEndings(const System &system,
                                          const LayoutInfo &layout,
                                          double height)
{
    const double TOP_LINE_OFFSET = 2;
    const double TEXT_PADDING = 5;

    for (const AlternateEnding &ending : system.getAlternateEndings())
    {
        const double location = layout.getPositionX(ending.getPosition()) +
                                0.5 * layout.getPositionSpacing();

        // Draw the vertical line.
        auto vertLine = new QGraphicsLineItem();
        vertLine->setLine(0, TOP_LINE_OFFSET, 0,
                          LayoutInfo::SYSTEM_SYMBOL_SPACING - TOP_LINE_OFFSET);
        vertLine->setPos(location, height);
        vertLine->setParentItem(myParentSystem);

        // Draw the text indicating the repeat numbers.
        auto text = new SimpleTextItem(
            QString::fromStdString(boost::lexical_cast<std::string>(ending)),
            myPlainTextFont);
        text->setPos(location + TEXT_PADDING, height + TEXT_PADDING / 2.0);
        text->setParentItem(myParentSystem);

        // The horizontal line either stretches to the next repeat end bar
        // in the system, or just to the next bar.
        double endX = 0;
        for (const Barline &barline : system.getBarlines())
        {
            // Look for the next repeat end bar.
            if (barline.getPosition() > ending.getPosition() &&
                barline.getBarType() == Barline::RepeatEnd)
            {
                endX = layout.getPositionX(barline.getPosition());
            }
        }

        // Otherwise, if there is no repeat bar just go to the next barline.
        if (endX == 0)
        {
            endX = layout.getPositionX(system.getNextBarline(
                                    ending.getPosition())->getPosition());
        }

        // Draw to the center of the barline's position.
        endX += 0.5 * layout.getPositionSpacing();

        // Ensure that the line doesn't extend past the edge of the system.
        endX = boost::algorithm::clamp(endX, 0.0, LayoutInfo::STAFF_WIDTH);

        auto horizLine = new QGraphicsLineItem();
        horizLine->setLine(0, TOP_LINE_OFFSET, endX - location, TOP_LINE_OFFSET);
        horizLine->setPos(location, height);
        horizLine->setParentItem(myParentSystem);
    }
}

static QString getBeatTypeImage(TempoMarker::BeatType type)
{
    QString file;

    switch (type)
    {
        case TempoMarker::Half:
            file = ":images/half_note";
            break;
        case TempoMarker::HalfDotted:
            file = ":images/half_note_dotted";
            break;
        case TempoMarker::Quarter:
            file = ":images/quarter_note";
            break;
        case TempoMarker::QuarterDotted:
            file = ":images/dotted_note";
            break;
        case TempoMarker::Eighth:
            file = ":images/8th_note";
            break;
        case TempoMarker::EighthDotted:
            file = ":images/8th_note_dotted";
            break;
        case TempoMarker::Sixteenth:
            file = ":images/16th_note";
            break;
        case TempoMarker::SixteenthDotted:
            file = ":images/16th_note_dotted";
            break;
        case TempoMarker::ThirtySecond:
            file = ":images/32nd_note";
            break;
        case TempoMarker::ThirtySecondDotted:
            file = ":images/32nd_note_dotted";
            break;
    }

    return file;
}

static QString getTripletFeelImage(const TempoMarker &tempo)
{
    switch (tempo.getTripletFeel())
    {
        case TempoMarker::TripletFeelEighth:
            return QStringLiteral(":/images/triplet_feel_eighth.png");
        case TempoMarker::TripletFeelEighthOff:
            return QStringLiteral(":/images/triplet_feel_eighth_off.png");
        case TempoMarker::TripletFeelSixteenth:
            return QStringLiteral(":/images/triplet_feel_sixteenth.png");
        case TempoMarker::TripletFeelSixteenthOff:
            return QStringLiteral(":/images/triplet_feel_sixteenth_off.png");
        default:
            Q_ASSERT(false);
            return "";
    }
}

void SystemRenderer::drawTempoMarkers(const System &system,
                                      const LayoutInfo &layout,
                                      double height)
{
    for (const TempoMarker &tempo : system.getTempoMarkers())
    {
        if (tempo.getMarkerType() == TempoMarker::NotShown)
            continue;

        const double x = layout.getPositionX(tempo.getPosition());

#if 0
        auto group = new ClickableGroup([]() {
            // TODO - allow editing a tempo marker by clicking on it.
        });
#else
        auto group = new QGraphicsItemGroup();
#endif

        QFont font = myPlainTextFont;
        if (tempo.getMarkerType() == TempoMarker::AlterationOfPace)
            font.setItalic(true);
        else
            font.setBold(true);

        QString text;
        if (tempo.getMarkerType() == TempoMarker::AlterationOfPace)
        {
            text = (tempo.getAlterationOfPace() == TempoMarker::Accelerando)
                       ? "accel."
                       : "rit.";
        }
        else
        {
            text += QString::fromStdString(tempo.getDescription()) + " ";

            const QString imageSpacing(3, ' ');
            const double NOTE_HEIGHT = 16;

            // Add the beat type image.
            QFontMetricsF fm(font);
            QPixmap image(getBeatTypeImage(tempo.getBeatType()));
            auto pixmap = new QGraphicsPixmapItem(image.scaled(
                fm.width(imageSpacing), NOTE_HEIGHT,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            pixmap->setX(fm.width(text));
            centerSymbolVertically(*pixmap, height);
            group->addToGroup(pixmap);

            text += imageSpacing;
            text += " = ";

            if (tempo.getMarkerType() == TempoMarker::ListessoMarker)
            {
                // Add the second beat type image.
                QPixmap image(getBeatTypeImage(tempo.getListessoBeatType()));
                auto pixmap = new QGraphicsPixmapItem(image.scaled(
                    fm.width(imageSpacing), NOTE_HEIGHT,
                    Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                pixmap->setX(fm.width(text));
                centerSymbolVertically(*pixmap, height);
                group->addToGroup(pixmap);

                text += imageSpacing;
            }
            else
                text += QString::number(tempo.getBeatsPerMinute());

            // Add the triplet feel image if necessary.
            if (tempo.getTripletFeel() != TempoMarker::NoTripletFeel)
            {
                text += " ( ";

                const QString imageSpacing(12, ' ');
                QPixmap image(getTripletFeelImage(tempo));
                pixmap = new QGraphicsPixmapItem(image.scaled(
                    fm.width(imageSpacing), 21,
                    Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                pixmap->setX(fm.width(text));
                centerSymbolVertically(*pixmap, height);
                group->addToGroup(pixmap);

                text += imageSpacing + " )";
            }
        }

        auto textItem = new SimpleTextItem(text, font);
        centerSymbolVertically(*textItem, height);
        group->addToGroup(textItem);

        group->setX(x);
        group->setParentItem(myParentSystem);
    }
}

void SystemRenderer::drawChordText(const System &system,
                                   const LayoutInfo &layout, double height)
{
    for (const ChordText &chord : system.getChords())
    {
        const double x = layout.getPositionX(chord.getPosition());
        const std::string text =
            boost::lexical_cast<std::string>(chord.getChordName());

        auto textItem =
            new SimpleTextItem(QString::fromStdString(text), myPlainTextFont);
        textItem->setX(x);
        centerSymbolVertically(*textItem, height);
        textItem->setParentItem(myParentSystem);
    }
}

void SystemRenderer::drawTextItems(const System &system,
                                   const LayoutInfo &layout, double height)
{
    for (const TextItem &text : system.getTextItems())
    {
        const QString &contents = QString::fromStdString(text.getContents());

        // Note: the SimpleTextItem class is not used here since multi-line
        // support is needed.
        auto text_item = new QGraphicsSimpleTextItem();
        text_item->setFont(myPlainTextFont);
        text_item->setText(contents);

        text_item->setX(layout.getPositionX(text.getPosition()));
        centerSymbolVertically(*text_item, height);
        text_item->setParentItem(myParentSystem);
    }
}

#if 0
void SystemRenderer::drawRhythmSlashes()
{
    const uint32_t y = system->GetExtraSpacing();

    std::vector<RhythmSlashPainter*> slashPainters;

    // generate the rhythm slashes
    for (size_t i = 0; i < system->GetRhythmSlashCount(); i++)
    {
        shared_ptr<const RhythmSlash> rhythmSlash = system->GetRhythmSlash(i);
        const uint32_t x = system->GetPositionX(rhythmSlash->GetPosition());

        RhythmSlashPainter* painter = new RhythmSlashPainter(rhythmSlash);
        painter->setPos(x, y);
        slashPainters.push_back(painter);

        // draw ties
        if (rhythmSlash->IsTied() && i != 0)
        {
            const RhythmSlashPainter* prevSlash = slashPainters.at(i - 1);

            const double xDiff = x - prevSlash->x();

            QPainterPath path;
            path.moveTo(xDiff, System::RHYTHM_SLASH_SPACING / 4.0);
            path.arcTo(0, 0, xDiff, System::RHYTHM_SLASH_SPACING / 2.0, 0, 180);

            QGraphicsPathItem* arc = new QGraphicsPathItem(path);
            arc->setPos(prevSlash->x() + RhythmSlashPainter::STEM_OFFSET,
                        y + RhythmSlashPainter::NOTE_HEAD_OFFSET - arc->boundingRect().height());
            arc->setParentItem(parentSystem);
        }
    }

    // add each rhythm slash to the scene
    for (RhythmSlashPainter* painter, slashPainters)
    {
        painter->setParentItem(parentSystem);
    }
}

#endif

void SystemRenderer::drawLegato(const Staff &staff, const LayoutInfo &layout)
{
    for (const Voice &voice : staff.getVoices())
    {
        std::map<int, int> arcs;

        for (const Position &pos : voice.getPositions())
        {
            const int position = pos.getPosition();

            for (const Note &note : pos.getNotes())
            {
                const int string = note.getString();

                if (note.hasProperty(Note::HammerOnOrPullOff) ||
                    note.hasProperty(Note::LegatoSlide))
                {
                    // Set the start position of an arc, if necessary.
                    if (arcs.find(string) == arcs.end())
                        arcs[string] = position;
                }
                // If an arc was already started and the current note is not
                // a hammeron / pulloff, end the arc.
                else if (arcs.find(string) != arcs.end())
                {
                    const int startPos = arcs.find(string)->second;

                    const double left = layout.getPositionX(startPos);
                    const double width = layout.getPositionX(position) - left;
                    double height = 7.5;
                    double y = layout.getTabLine(string) - 2;

                    if (string >= layout.getStringCount() / 2)
                    {
                        // For notes on the bottom half of the staff, flip the
                        // arc and place it below the notes.
                        y += 2 * layout.getTabLineSpacing() + height / 2.5;
                        height = -height;
                    }

                    QPainterPath path;
                    path.moveTo(width, height / 2);
                    path.arcTo(0, 0, width, height, 0, 180);

                    auto arc = new AntialiasedPathItem(path);
                    arc->setPos(left + layout.getPositionSpacing() / 2, y);
                    arc->setParentItem(myParentStaff);

                    arcs.erase(arcs.find(string));
                }

                // Draw hammerons/pulloffs to nowhere.
                if (note.hasProperty(Note::HammerOnFromNowhere) ||
                    note.hasProperty(Note::PullOffToNowhere))
                {
                    const double height = 10;
                    const double width = 6;

                    QPainterPath path;
                    path.moveTo(width, height / 2);
                    path.arcTo(0, 0, width, height, 0, 180);

                    auto arc = new AntialiasedPathItem(path);
                    arc->setPos(layout.getPositionX(position) + 2,
                                layout.getTabLine(string) - 2);
                    arc->setParentItem(myParentStaff);

                    if (arcs.find(string) != arcs.end())
                        arcs.erase(arcs.find(string));
                }
            }
        }
    }
}

void SystemRenderer::drawPlayerChanges(const System &system, int staffIndex,
                                       const LayoutInfo &layout)
{
    for (const PlayerChange &change : system.getPlayerChanges())
    {
        const std::vector<ActivePlayer> activePlayers =
                change.getActivePlayers(staffIndex);

        QString description;
        if (!activePlayers.empty())
        {
            for (size_t i = 0, n = activePlayers.size(); i < n; ++i)
            {
                if (i != 0)
                    description += ", ";

                const int player_index = activePlayers[i].getPlayerNumber();
                description += QString::fromStdString(
                    myScore.getPlayers()[player_index].getDescription());
            }
        }
        else
            description = "(No Players)";

        auto text = new SimpleTextItem(description, myPlainTextFont);
        text->setPos(layout.getPositionX(change.getPosition()),
                     layout.getBottomStdNotationLine() +
                     LayoutInfo::STAFF_BORDER_SPACING +
                     layout.getStdNotationStaffBelowSpacing());
        text->setParentItem(myParentStaff);
    }
}

void SystemRenderer::drawSlides(const Staff &staff, const LayoutInfo &layout)
{
    for (const Voice &voice : staff.getVoices())
    {
        for (int string = 0; string < staff.getStringCount(); ++string)
        {
            for (const Position &pos : voice.getPositions())
            {
                const Note *note = Utils::findByString(pos, string);
                if (!note)
                    continue;

                // Draw any slides out of the note.
                if (note->hasProperty(Note::ShiftSlide) ||
                    note->hasProperty(Note::LegatoSlide) ||
                    note->hasProperty(Note::SlideOutOfDownwards) ||
                    note->hasProperty(Note::SlideOutOfUpwards))
                {
                    // Figure out if we're sliding up or down.
                    bool slideUp = note->hasProperty(Note::SlideOutOfUpwards);

                    if (note->hasProperty(Note::ShiftSlide) ||
                        note->hasProperty(Note::LegatoSlide))
                    {
                        const Note *nextNote = VoiceUtils::getNextNote(
                            voice, pos.getPosition(), string);
                        slideUp = nextNote && nextNote->getFretNumber() >
                                note->getFretNumber();
                    }

                    const Position *nextPos =
                        VoiceUtils::getNextPosition(voice, pos.getPosition());

                    drawSlide(layout, string, slideUp, pos.getPosition(),
                              nextPos ? nextPos->getPosition() :
                                        pos.getPosition() + 1);
                }

                // Draw any slides going into the note.
                if (note->hasProperty(Note::SlideIntoFromAbove) ||
                    note->hasProperty(Note::SlideIntoFromBelow))
                {
                    const int position = pos.getPosition();
                    drawSlide(layout, string,
                              note->hasProperty(Note::SlideIntoFromBelow),
                              position - 1, position);
                }
            }
        }
    }
}

void SystemRenderer::drawSlide(const LayoutInfo &layout, int string,
                               bool slideUp, int position1, int position2) const
{
    Q_ASSERT(position1 <= position2);

    double left = layout.getPositionX(std::max(position1, 0));
    // Adjust slides at the first position of a system.
    if (position1 < 0)
        left -= layout.getPositionSpacing();

    const int numPositions = layout.getNumPositions();
    double right = layout.getPositionX(std::min(position2, numPositions));
    if (position2 > numPositions)
        right += layout.getPositionSpacing();

    const double width = right - left;

    double height = 5;
    double y = (layout.getTabLine(string + 1) + layout.getTabLine(string)) / 2;

    if (slideUp) // If we're sliding up, flip the line.
    {
        height = -height;
        y += layout.getTabLineSpacing() + 1;
    }

    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(width - layout.getPositionSpacing() / 2, height);

    auto slide = new AntialiasedPathItem(path);
    slide->setPos(left + layout.getPositionSpacing() / 1.5 + 1,
                  y + height / 2);
    slide->setParentItem(myParentStaff);
}

void SystemRenderer::drawSymbolsBelowTabStaff(const LayoutInfo &layout)
{
    for (const SymbolGroup &symbolGroup : layout.getTabStaffBelowSymbols())
    {
        QGraphicsItem *renderedSymbol = nullptr;

        switch (symbolGroup.getSymbolType())
        {
        case SymbolGroup::PickStrokeUp:
            renderedSymbol = createPickStroke(QChar(MusicFont::PickStrokeUp));
            break;
        case SymbolGroup::PickStrokeDown:
            renderedSymbol = createPickStroke(QChar(MusicFont::PickStrokeDown));
            break;
        case SymbolGroup::Tap:
            renderedSymbol = createPlainTextSymbol("T", QFont::StyleNormal);
            break;
        case SymbolGroup::Hammeron:
            renderedSymbol = createPlainTextSymbol("H", QFont::StyleNormal);
            break;
        case SymbolGroup::Pulloff:
            renderedSymbol = createPlainTextSymbol("P", QFont::StyleNormal);
            break;
        case SymbolGroup::Slide:
            renderedSymbol = createPlainTextSymbol("sl.", QFont::StyleItalic);
            break;
        case SymbolGroup::ArtificialHarmonic:
        {
            const Position *pos = ScoreUtils::findByPosition(
                symbolGroup.getVoice().getPositions(),
                symbolGroup.getLeftPosition());
            Q_ASSERT(pos);
            renderedSymbol = createArtificialHarmonicText(*pos);
            break;
        }
        default:
            Q_ASSERT(false);
            break;
        }

        double x = layout.getPositionX(symbolGroup.getLeftPosition());
        // Center symbols that apply to this note and the next adjacent note.
        if (symbolGroup.getSymbolType() == SymbolGroup::Hammeron ||
            symbolGroup.getSymbolType() == SymbolGroup::Pulloff ||
            symbolGroup.getSymbolType() == SymbolGroup::Slide)
        {
            x += 0.5 * (layout.getPositionX(symbolGroup.getRightPosition()) - x);
        }

        centerHorizontally(*renderedSymbol, x, x + symbolGroup.getWidth());
        renderedSymbol->setY(layout.getBottomTabLine() +
                             symbolGroup.getHeight() *
                                 LayoutInfo::TAB_SYMBOL_SPACING);

        renderedSymbol->setParentItem(myParentStaff);
    }
}

QGraphicsItem *SystemRenderer::createPickStroke(const QString &text)
{
    auto textItem = new SimpleTextItem(text, myMusicNotationFont);
    textItem->setPos(2, 2 - myMusicFontMetrics.ascent());

    // Sticking the text in a QGraphicsItemGroup allows us to offset the
    // position of the text from its default location
    auto group = new QGraphicsItemGroup();
    group->addToGroup(textItem);
    return group;
}

QGraphicsItem *SystemRenderer::createPlainTextSymbol(const QString &text,
                                                     QFont::Style style)
{
    myPlainTextFont.setStyle(style);

    auto textItem = new SimpleTextItem(text, myPlainTextFont);
    textItem->setPos(0, -8);

    auto group = new QGraphicsItemGroup();
    group->addToGroup(textItem);

    myPlainTextFont.setStyle(QFont::StyleNormal);

    return group;
}

/// Creates the text portion of an artificial harmonic, which displays the note.
QGraphicsItem* SystemRenderer::createArtificialHarmonicText(
        const Position &position)
{
    // Find the note that has the harmonic.
    auto it = boost::range::find_if(position.getNotes(), [] (const Note &note) {
        return note.hasArtificialHarmonic();
    });

    Q_ASSERT(it != position.getNotes().end());

    // Build the text representation of the harmonic.
    const ArtificialHarmonic &harmonic = it->getArtificialHarmonic();
    ChordName name;
    name.setTonicKey(harmonic.getKey());
    name.setTonicVariation(harmonic.getVariation());
    name.setBassKey(harmonic.getKey());
    name.setBassVariation(harmonic.getVariation());

    return createPlainTextSymbol(
                QString::fromStdString(boost::lexical_cast<std::string>(name)),
                QFont::StyleNormal);
}

void SystemRenderer::drawSymbolsAboveTabStaff(const Staff &staff,
                                              const LayoutInfo& layout)
{
    for (const SymbolGroup &symbolGroup : layout.getTabStaffAboveSymbols())
    {
        QGraphicsItem *renderedSymbol = nullptr;
        const double width = symbolGroup.getWidth();

        switch(symbolGroup.getSymbolType())
        {
        case SymbolGroup::Bend:
            renderedSymbol = createBendGroup(symbolGroup, layout);
            break;
        case SymbolGroup::LetRing:
            renderedSymbol = createConnectedSymbolGroup("let ring",
                                                        QFont::StyleItalic,
                                                        width, layout);
            break;
        case SymbolGroup::Vibrato:
            renderedSymbol = drawContinuousFontSymbols(MusicFont::Vibrato,
                                                       width);
            break;
        case SymbolGroup::WideVibrato:
            renderedSymbol = drawContinuousFontSymbols(MusicFont::WideVibrato,
                                                       width);
            break;
        case SymbolGroup::PalmMuting:
            renderedSymbol = createConnectedSymbolGroup("P.M.",
                                                        QFont::StyleNormal,
                                                        width, layout);
            break;
        case SymbolGroup::TremoloPicking:
            renderedSymbol = createTremoloPicking(layout);
            break;
        case SymbolGroup::Trill:
            renderedSymbol = createTrill(layout);
            break;
        case SymbolGroup::NaturalHarmonic:
            renderedSymbol = createConnectedSymbolGroup("N.H.",
                                                        QFont::StyleNormal,
                                                        width, layout);
            break;
        case SymbolGroup::Dynamic:
        {
            const Dynamic *dynamic = ScoreUtils::findByPosition(
                staff.getDynamics(), symbolGroup.getLeftPosition());
            Q_ASSERT(dynamic);

            renderedSymbol = createDynamic(*dynamic);
            break;
        }
        case SymbolGroup::ArtificialHarmonic:
            renderedSymbol = createConnectedSymbolGroup("A.H.",
                                                        QFont::StyleNormal,
                                                        width, layout);
            break;
#if 0
        case Layout::SymbolVolumeSwell:
        {
            // figure out the direction of the volume swell
            uint8_t startVolume = 0, endVolume = 0, duration = 0;
            staff->GetPosition(0, symbolGroup.leftPosIndex)->GetVolumeSwell(startVolume, endVolume, duration);

            renderedSymbol = createVolumeSwell(width, staffInfo,
                                               (startVolume <= endVolume) ? VolumeIncreasing : VolumeDecreasing);
            break;
        }
        case Layout::SymbolTremoloBar:
            renderedSymbol = new TremoloBarPainter(staff->GetPosition(0, symbolGroup.leftPosIndex),
                                                   width);
            break;        
#endif

        default:
            Q_ASSERT(false);
            break;
        }

        // Bends are positioned differently, since they overlap with the
        // standard notation staff.
        if (symbolGroup.getSymbolType() != SymbolGroup::Bend)
        {
            renderedSymbol->setPos(
                layout.getPositionX(symbolGroup.getLeftPosition()),
                layout.getTopTabLine() - LayoutInfo::STAFF_BORDER_SPACING -
                    symbolGroup.getHeight() * LayoutInfo::TAB_SYMBOL_SPACING);
        }

        renderedSymbol->setParentItem(myParentStaff);
    }
}

void SystemRenderer::drawSymbolsAboveStdNotationStaff(const LayoutInfo& layout)
{
    for (const SymbolGroup &symbolGroup : layout.getStdNotationStaffAboveSymbols())
    {
        QGraphicsItem *renderedSymbol = nullptr;

        switch (symbolGroup.getSymbolType())
        {
        case SymbolGroup::Octave8va:
            renderedSymbol = createConnectedSymbolGroup("8va",
                                                        QFont::StyleItalic,
                                                        symbolGroup.getWidth(),
                                                        layout);
            break;
        case SymbolGroup::Octave15ma:
            renderedSymbol = createConnectedSymbolGroup("15ma",
                                                        QFont::StyleItalic,
                                                        symbolGroup.getWidth(),
                                                        layout);
            break;
        default:
            // All symbol types should have been dealt with by now.
            Q_ASSERT(false);
            break;
        }

        renderedSymbol->setPos(layout.getPositionX(symbolGroup.getLeftPosition()), 0);
        renderedSymbol->setParentItem(myParentStaff);
    }
}

void SystemRenderer::drawSymbolsBelowStdNotationStaff(const LayoutInfo &layout)
{
    for (const SymbolGroup &symbolGroup :
         layout.getStdNotationStaffBelowSymbols())
    {
        QGraphicsItem *renderedSymbol = nullptr;

        switch (symbolGroup.getSymbolType())
        {
        case SymbolGroup::Octave8vb:
            renderedSymbol = createConnectedSymbolGroup("8vb",
                                                        QFont::StyleItalic,
                                                        symbolGroup.getWidth(),
                                                        layout);
            break;
        case SymbolGroup::Octave15mb:
            renderedSymbol = createConnectedSymbolGroup("15mb",
                                                        QFont::StyleItalic,
                                                        symbolGroup.getWidth(),
                                                        layout);
            break;
        default:
            // All symbol types should have been dealt with by now.
            Q_ASSERT(false);
            break;
        }

        renderedSymbol->setPos(layout.getPositionX(symbolGroup.getLeftPosition()),
                               layout.getBottomStdNotationLine() +
                               layout.getStdNotationStaffBelowSpacing());
        renderedSymbol->setParentItem(myParentStaff);
    }
}

QGraphicsItem *SystemRenderer::createConnectedSymbolGroup(const QString &text,
                                                          QFont::Style style,
                                                          double width,
                                                          const LayoutInfo &layout)
{
    mySymbolTextFont.setStyle(style);

    // Render the description (i.e. "let ring").
    auto description = new SimpleTextItem(text, mySymbolTextFont);

    auto group = new QGraphicsItemGroup();
    group->addToGroup(description);

    // Draw dashed line across the remaining positions in the group.
    if (width > layout.getPositionSpacing())
    {
        const double rightEdge = width - 0.5 * layout.getPositionSpacing();
        const double leftEdge = description->boundingRect().right();
        const double y = LayoutInfo::TAB_SYMBOL_SPACING / 2.0;

        createDashedLine(group, leftEdge, rightEdge, y);
    }

    return group;
}

void SystemRenderer::createDashedLine(QGraphicsItemGroup *group, double left,
                                      double right, double y)
{
    auto line = new QGraphicsLineItem(left, y, right, y);
    line->setPen(QPen(Qt::black, 1, Qt::DashLine));
    group->addToGroup(line);

    // Draw a vertical line at the end of the dotted lines.
    auto lineEnd = new QGraphicsLineItem(
        line->boundingRect().right(), y, line->boundingRect().right(),
        y + 0.5 * LayoutInfo::TAB_SYMBOL_SPACING);
    group->addToGroup(lineEnd);
}

#if 0

/// Creates a volume swell QGraphicsItem of the specified type
QGraphicsItem* SystemRenderer::createVolumeSwell(uint8_t width, const StaffData& currentStaffInfo,
                                                 VolumeSwellType type)
{
    double leftX = currentStaffInfo.positionWidth / 2.0;
    double rightX = width;

    if (type == VolumeDecreasing) // switch directions for decreasing volume swells
    {
        std::swap(leftX, rightX);
    }

    // in this case, the width of the symbol rectangle is the # of positions that the volume swell spans
    QPainterPath path;
    path.moveTo(rightX, 0);
    path.lineTo(leftX, Staff::TAB_SYMBOL_HEIGHT / 2.0);
    path.lineTo(rightX, Staff::TAB_SYMBOL_HEIGHT);

    QGraphicsPathItem* swell = new QGraphicsPathItem(path);
    return swell;
}

#endif
QGraphicsItem *SystemRenderer::drawContinuousFontSymbols(QChar symbol,
                                                         int width)
{
    QFont font = MusicFont::getFont(25);

    const double symbolWidth = QFontMetricsF(font).width(symbol);
    const int numSymbols = width / symbolWidth;
    auto text = new SimpleTextItem(QString(numSymbols, symbol), font);
    text->setPos(0, -25);

    // A bit of a hack for getting around the height offset caused by the
    // music font.
    auto group = new QGraphicsItemGroup();
    group->addToGroup(text);
    return group;
}

QGraphicsItem *SystemRenderer::createTremoloPicking(const LayoutInfo& layout)
{
    const double offset = LayoutInfo::TAB_SYMBOL_SPACING / 3;

    auto group = new QGraphicsItemGroup();

    for (int i = 0; i < 3; i++)
    {
        auto line = new SimpleTextItem(QChar(MusicFont::TremoloPicking),
                                       myMusicNotationFont);
        centerHorizontally(*line, 0, layout.getPositionSpacing() * 1.25);
        line->setY(-myMusicFontMetrics.ascent() - 7 + i * offset);
        group->addToGroup(line);
    }

    return group;
}

QGraphicsItem *SystemRenderer::createTrill(const LayoutInfo& layout)
{
    QFont font(MusicFont::getFont(21));

    auto text = new SimpleTextItem(QChar(MusicFont::Trill), font);
    centerHorizontally(*text, 0, layout.getPositionSpacing());
    text->setY(-18);

    auto group = new QGraphicsItemGroup();
    group->addToGroup(text);
    return group;
}

QGraphicsItem *SystemRenderer::createDynamic(const Dynamic &dynamic)
{
    QString text = "fff";
    Dynamic::VolumeLevel volume = dynamic.getVolume();

    if (volume == Dynamic::Off)
        text = "off";
    else if (volume <= Dynamic::ppp)
        text = "ppp";
    else if (volume <= Dynamic::pp)
        text = "pp";
    else if (volume <= Dynamic::p)
        text = "p";
    else if (volume <= Dynamic::mp)
        text = "mp";
    else if (volume <= Dynamic::mf)
        text = "mf";
    else if (volume <= Dynamic::f)
        text = "f";
    else if (volume <= Dynamic::ff)
        text = "ff";

    auto textItem = new SimpleTextItem(text, myMusicNotationFont);
    textItem->setPos(0, -myMusicFontMetrics.ascent() + 10);

    // Sticking the text in a QGraphicsItemGroup allows us to offset the
    // position of the text from its default location.
    auto group = new QGraphicsItemGroup();
    group->addToGroup(textItem);
    return group;
}

void SystemRenderer::drawStdNotation(const System &system, const Staff &staff,
                                     const LayoutInfo &layout)
{
    // Draw rests.
    for (const Voice &voice : staff.getVoices())
    {
        for (const Position &pos : voice.getPositions())
        {
            const double x = layout.getPositionX(pos.getPosition());

            const Barline *prevBar = system.getPreviousBarline(pos.getPosition());
            if (!prevBar)
                prevBar = &system.getBarlines().front();

            if (pos.hasMultiBarRest())
            {
                drawMultiBarRest(system, *prevBar, layout,
                                 pos.getMultiBarRestCount());
            }
            else if (pos.isRest())
            {
                drawRest(pos, x, layout);
            }
        }
    }

    const std::vector<StdNotationNote> &notes = layout.getStdNotationNotes();

    std::map<int, double> minNoteLocations;
    std::map<int, double> maxNoteLocations;
    std::map<int, double> noteHeadWidths;
    std::map<int, double> noteHeadCenters;

    QFont default_font(MusicFont::getFont(MusicFont::DEFAULT_FONT_SIZE));
    QFont grace_font(MusicFont::getFont(MusicFont::GRACE_NOTE_SIZE));
    QFontMetricsF default_fm(default_font);
    QFontMetricsF grace_fm(grace_font);

    for (const StdNotationNote &note : notes)
    {
        const QFont *font = note.isGraceNote() ? &grace_font : &default_font;
        const QFontMetricsF *fm = note.isGraceNote() ? &grace_fm : &default_fm;

        const QChar note_head_char = note.getNoteHeadSymbol();
        const double note_head_width = fm->width(note_head_char);

        const QString accidental_text = note.getAccidentalText();
        const double accidental_width = fm->width(accidental_text);

        const double x = layout.getPositionX(note.getPosition()) +
                0.5 * (layout.getPositionSpacing() - note_head_width) -
                accidental_width;
        const double y =
            note.getY() + layout.getTopStdNotationLine() - fm->ascent();
        const QString note_text = accidental_text + note_head_char;

        QGraphicsItemGroup *group = nullptr;
        auto text_item = new SimpleTextItem(note_text, *font);

        if (note.isDotted() || note.isDoubleDotted())
        {
            group = new QGraphicsItemGroup();
            const double dotX = fm->width(note_text) + 2;

            const QChar dot(MusicFont::Dot);
            auto dotText = new SimpleTextItem(dot, *font);
            dotText->setPos(dotX, 0);
            group->addToGroup(dotText);

            if (note.isDoubleDotted())
            {
                auto dotText2 = new SimpleTextItem(dot, *font);
                dotText2->setPos(dotX + 4, 0);
                group->addToGroup(dotText2);
            }
        }
        
        if (note.getNote()->hasLeftHandFingering())
        {
            if (!group)
            {
                group = new QGraphicsItemGroup();
            }
            
            
            const auto fingering = note.getNote()->getLeftHandFingering();
            const auto number = fingering.getFingerNumber();
            auto numberText = new SimpleTextItem(QString::number(number), myPlainTextFont);
            
            double numberX;
            double numberY;
            switch (fingering.getDisplayPosition())
            {
            case LeftHandFingering::DisplayPosition::Left:
                numberX = -note_head_width - 1;
                numberY = 25;
                break;
            case LeftHandFingering::DisplayPosition::AboveLeft:
                numberX = -note_head_width + 1;
                numberY = 18;
                break;
            case LeftHandFingering::DisplayPosition::Above:
                numberX = 0;
                numberY = 17;
                break;
            case LeftHandFingering::DisplayPosition::AboveRight:
                numberX = note_head_width + 1;
                numberY = 18;
                break;
            case LeftHandFingering::DisplayPosition::Right:
                numberX = note_head_width + 3;
                numberY = 25;
                break;
            case LeftHandFingering::DisplayPosition::BelowRight:
                numberX = note_head_width + 1;
                numberY = 32;
                break;
            case LeftHandFingering::DisplayPosition::Below:
                numberX = 0;
                numberY = 33;
                break;
            case LeftHandFingering::DisplayPosition::BelowLeft:
                numberX = -note_head_width + 1;
                numberY = 32;
                break;
            }
            
            numberText->setPos(numberX, numberY);
            group->addToGroup(numberText);
            
        }

        if (group)
        {
            group->addToGroup(text_item);
            group->setPos(x, y);
            group->setParentItem(myParentStaff);
        }
        else
        {
            text_item->setPos(x, y);
            text_item->setParentItem(myParentStaff);
        }

        const int position = note.getPosition();
        minNoteLocations[position] = std::min(minNoteLocations[position],
                                              note.getY());
        maxNoteLocations[position] = std::max(maxNoteLocations[position],
                                              note.getY());
        noteHeadWidths[position] = note_head_width;
        noteHeadCenters[position] = x;
    }

    drawLedgerLines(layout, minNoteLocations, maxNoteLocations, noteHeadWidths);

    for (int v = 0; v < Staff::NUM_VOICES; ++v)
    {
        const std::vector<BeamGroup> &beamGroups = layout.getBeamGroups(v);
        const std::vector<NoteStem> &stems = layout.getNoteStems(v);

        for (const BeamGroup &group : beamGroups)
        {
            group.drawStems(myParentStaff, stems, myMusicNotationFont,
                            myMusicFontMetrics, layout);
        }

        const Voice &voice = staff.getVoices()[v];
        drawIrregularGroups(voice, stems);
        drawTies(voice, notes, stems, layout);
    }
}

void SystemRenderer::drawTies(const Voice &voice,
                              const std::vector<StdNotationNote> &notes,
                              const std::vector<NoteStem> &stems,
                              const LayoutInfo &layout)
{
    const double height = 7;

    for (const StdNotationNote &note : notes)
    {
        if (&note.getVoice() != &voice || !note.getTie())
            continue;

        const int pos = note.getPosition();
        const NoteStem &stem = stems.at(
            ScoreUtils::findIndexByPosition(voice.getPositions(), pos));
        const int prevPos = *note.getTie();

        // Draw to the centre of each position.
        const double x = layout.getPositionX(note.getPosition()) +
                         0.5 * layout.getPositionSpacing();
        const double prevX =
            layout.getPositionX(prevPos) + 0.5 * layout.getPositionSpacing();
        const double width = x - prevX;

        // Adjust the orientation depending on the stem direction.
        QPainterPath path;
        path.moveTo(width, height / 2);
        path.arcTo(0, 0, width, height, 0,
                   stem.getStemType() == NoteStem::StemDown ? 180 : -180);

        const double y = note.getY() + layout.getTopStdNotationLine() +
                         (stem.getStemType() == NoteStem::StemDown
                              ? -1.25 * LayoutInfo::STD_NOTATION_LINE_SPACING
                              : 0.25 * LayoutInfo::STD_NOTATION_LINE_SPACING);

        auto arc = new AntialiasedPathItem(path);
        arc->setPos(prevX, y);
        arc->setParentItem(myParentStaff);
    }
}

void SystemRenderer::drawIrregularGroups(const Voice &voice,
                                         const std::vector<NoteStem> &stems)
{
    VerticalLayout topLayout;
    VerticalLayout bottomLayout;

    for (const IrregularGrouping &group : voice.getIrregularGroupings())
    {
        const int index = ScoreUtils::findIndexByPosition(voice.getPositions(),
                                                          group.getPosition());
        Q_ASSERT(index >= 0);

        NoteStem::StemType direction = stems.at(index).getStemType();

        auto begin = stems.begin() + index;
        auto end = begin + group.getLength();
        auto last = end - 1;

        double y1 = 0;
        double y2 = 0;

        if (direction == NoteStem::StemUp)
        {
            NoteStem highestStem = NoteStem::findHighestStem(begin, end);

            y1 = highestStem.getTop() - LayoutInfo::IRREGULAR_GROUP_BEAM_SPACING;
            y2 = highestStem.getTop() -
                 topLayout.addBox(begin->getPositionIndex(),
                                  last->getPositionIndex(), 1) *
                     LayoutInfo::IRREGULAR_GROUP_HEIGHT;
        }
        else
        {
            NoteStem lowestStem = NoteStem::findLowestStem(begin, end);

            y1 = lowestStem.getBottom() + LayoutInfo::IRREGULAR_GROUP_BEAM_SPACING;
            y2 = lowestStem.getBottom() +
                 bottomLayout.addBox(begin->getPositionIndex(),
                                     last->getPositionIndex(), 1) *
                     LayoutInfo::IRREGULAR_GROUP_HEIGHT;
        }

        const double leftX = begin->getX();
        const double rightX = last->getX();

        // Draw the value of the irregular grouping.
        const QString text = QString::fromStdString(
                    boost::lexical_cast<std::string>(group));

        QFont font = myMusicNotationFont;
        font.setItalic(true);
        font.setPixelSize(18);

        QFontMetricsF fm(font);
        const double textWidth = fm.width(text);
        const double centreX = leftX + (rightX - (leftX + textWidth)) / 2.0;

        auto textItem = new SimpleTextItem(text, font);
        textItem->setPos(centreX, y2 - font.pixelSize());
        textItem->setParentItem(myParentStaff);

        const double lineWidth =
            std::max(0.0, 0.5 * (rightX - leftX - textWidth - 10));

        // Draw the two horizontal line segments across the group, and the two
        // vertical lines on either end.
        QGraphicsLineItem *horizLine1 = new QGraphicsLineItem();
        horizLine1->setLine(leftX, y2, leftX + lineWidth, y2);
        horizLine1->setParentItem(myParentStaff);

        QGraphicsLineItem *horizLine2 = new QGraphicsLineItem();
        horizLine2->setLine(rightX - lineWidth, y2, rightX, y2);
        horizLine2->setParentItem(myParentStaff);

        QGraphicsLineItem *vertLine1 = new QGraphicsLineItem();
        vertLine1->setLine(leftX, y1, leftX, y2);
        vertLine1->setParentItem(myParentStaff);

        QGraphicsLineItem *vertLine2 = new QGraphicsLineItem();
        vertLine2->setLine(rightX, y1, rightX, y2);
        vertLine2->setParentItem(myParentStaff);
    }
}

void SystemRenderer::drawMultiBarRest(const System &system,
                                      const Barline &leftBar,
                                      const LayoutInfo &layout,
                                      int measureCount)
{
    const Barline *rightBar = system.getNextBarline(leftBar.getPosition());
    Q_ASSERT(rightBar);

    const double leftX = (leftBar.getPosition() == 0) ?
                layout.getPositionX(leftBar.getPosition()) :
                layout.getPositionX(leftBar.getPosition() + 1);

    const double rightX = boost::algorithm::clamp(
                layout.getPositionX(rightBar->getPosition()), 0.0,
                LayoutInfo::STAFF_WIDTH - layout.getPositionSpacing() / 2.0);

    // Draw the measure count.
    auto measureCountText =
        new SimpleTextItem(QString::number(measureCount), myMusicNotationFont);

    centerHorizontally(*measureCountText, leftX, rightX);
    measureCountText->setY(layout.getTopStdNotationLine() -
                           myMusicFontMetrics.ascent());
    measureCountText->setParentItem(myParentStaff);

    // Draw symbol across std. notation staff.
    auto vertLineLeft =
        new QGraphicsLineItem(leftX, layout.getStdNotationLine(2), leftX,
                              layout.getStdNotationLine(4));
    vertLineLeft->setParentItem(myParentStaff);

    auto vertLineRight =
        new QGraphicsLineItem(rightX, layout.getStdNotationLine(2), rightX,
                              layout.getStdNotationLine(4));
    vertLineRight->setParentItem(myParentStaff);

    auto horizontalLine = new QGraphicsRectItem(
        leftX, layout.getStdNotationLine(2) +
                   0.5 * LayoutInfo::STD_NOTATION_LINE_SPACING,
        rightX - leftX, LayoutInfo::STD_NOTATION_LINE_SPACING * 0.9);

    horizontalLine->setBrush(QBrush(Qt::black));
    horizontalLine->setParentItem(myParentStaff);
}

void SystemRenderer::drawRest(const Position &pos, double x, const LayoutInfo &layout)
{
    // Position it approximately in the middle of the staff.
    double y = 2 * LayoutInfo::STD_NOTATION_LINE_SPACING -
            myMusicFontMetrics.ascent();

    QChar symbol;
    switch (pos.getDurationType())
    {
    case Position::WholeNote:
        symbol = MusicFont::WholeRest;
        y -= LayoutInfo::STD_NOTATION_LINE_SPACING - 1;
        break;
    case Position::HalfNote:
        symbol = MusicFont::HalfRest;
        break;
    case Position::QuarterNote:
        symbol = MusicFont::QuarterRest;
        break;
    case Position::EighthNote:
        symbol = MusicFont::EighthRest;
        break;
    case Position::SixteenthNote:
        symbol = MusicFont::SixteenthRest;
        break;
    case Position::ThirtySecondNote:
        symbol = MusicFont::ThirtySecondRest;
        break;
    case Position::SixtyFourthNote:
        symbol = MusicFont::SixtyFourthRest;
        y -= 3; // Compensate for the extra height of this symbol.
        break;
    }

    auto group = new QGraphicsItemGroup();
    auto text = new SimpleTextItem(symbol, myMusicNotationFont);
    text->setPos(0, y);
    group->addToGroup(text);

    // Draw dots if necessary.
    const QChar dot = MusicFont::Dot;
    const double dotX = myMusicNotationFont.pixelSize() / 2.0;
    // Position just below second line of staff.
    const double dotY = 1.6 * LayoutInfo::STD_NOTATION_LINE_SPACING -
            myMusicFontMetrics.ascent();

    if (pos.hasProperty(Position::Dotted) ||
        pos.hasProperty(Position::DoubleDotted))
    {
        auto dotText = new SimpleTextItem(dot, myMusicNotationFont);
        dotText->setPos(dotX, dotY);
        group->addToGroup(dotText);

        if (pos.hasProperty(Position::DoubleDotted))
        {
            auto dotText2 = new SimpleTextItem(dot, myMusicNotationFont);
            dotText2->setPos(dotX + 4, dotY);
            group->addToGroup(dotText2);
        }
    }

    centerHorizontally(*group, x, x + layout.getPositionSpacing() * 1.25);
    group->setY(layout.getTopStdNotationLine());
    group->setParentItem(myParentStaff);
}

void SystemRenderer::drawLedgerLines(
        const LayoutInfo &layout,
        const std::map<int, double> &minNoteLocations,
        const std::map<int, double> &maxNoteLocations,
        const std::map<int, double> &noteHeadWidths)
{
    QPainterPath path;

    for (const int position : minNoteLocations | boost::adaptors::map_keys)
    {
        const double highestNote = minNoteLocations.at(position);
        const double lowestNote = maxNoteLocations.at(position);

        const int numLedgerLinesTop = -highestNote /
                LayoutInfo::STD_NOTATION_LINE_SPACING;
        const int numLedgerLinesBottom =
                (lowestNote - layout.getStdNotationStaffHeight()) /
                LayoutInfo::STD_NOTATION_LINE_SPACING;

        std::vector<double> ledgerLineLocations;

        if (numLedgerLinesTop > 0)
        {
            const double topLine = layout.getTopStdNotationLine();
            for (int i = 1; i <= numLedgerLinesTop; ++i)
            {
                ledgerLineLocations.push_back(
                            topLine - i * LayoutInfo::STD_NOTATION_LINE_SPACING);
            }
        }

        if (numLedgerLinesBottom > 0)
        {
            const int bottomLine = layout.getBottomStdNotationLine();
            for (int i = 1; i <= numLedgerLinesBottom; ++i)
            {
                ledgerLineLocations.push_back(bottomLine +
                            i * LayoutInfo::STD_NOTATION_LINE_SPACING);
            }
        }

        const double ledgerLineWidth = noteHeadWidths.at(position) * 2;
        const double x = layout.getPositionX(position) +
                0.5 * (layout.getPositionSpacing() - ledgerLineWidth);

        for (double location : ledgerLineLocations)
        {
            path.moveTo(x, location);
            path.lineTo(x + ledgerLineWidth, location);
        }
    }

    auto ledgerlines = new QGraphicsPathItem(path);
    ledgerlines->setParentItem(myParentStaff);
}

static double getBendHeight(Bend::DrawPoint point, const Note &note,
                            const LayoutInfo &layout)
{
    if (point == Bend::LowPoint)
    {
        return layout.getTabLine(note.getString()) +
               0.5 * layout.getTabLineSpacing();
    }
    else if (point == Bend::MidPoint)
        return layout.getTopTabLine() - LayoutInfo::TAB_SYMBOL_SPACING * 1.5;
    else
        return layout.getTopTabLine() - LayoutInfo::TAB_SYMBOL_SPACING * 2.5;
}

void SystemRenderer::createBend(QGraphicsItemGroup *group, double left,
                                double right, double yStart, double yEnd,
                                int pitch, bool prebend)
{
    QPainterPath path;

    // Draw an arc for the bend.
    if (!prebend)
    {
        path.moveTo(left, yStart);
        path.cubicTo(left, yStart, right, yStart, right, yEnd);
    }
    // Draw a straight line up for the bend.
    else
    {
        path.moveTo(left, yStart);
        right = boost::algorithm::clamp(right, left, left + 3.0);
        path.lineTo(right, yStart);
        path.lineTo(right, yEnd);
    }

    group->addToGroup(new AntialiasedPathItem(path));

    // Draw arrow head, and choose the correct orientation depending on whether
    // the bend is going up or down.
    static const double ARROW_WIDTH = 5.0;
    QPolygonF arrowShape;
    arrowShape << QPointF(right - ARROW_WIDTH / 2, yEnd)
               << QPointF(right + ARROW_WIDTH / 2, yEnd)
               << QPointF(right, (yEnd < yStart) ? yEnd - ARROW_WIDTH
                                                 : yEnd + ARROW_WIDTH);

    auto *arrow = new QGraphicsPolygonItem(arrowShape);
    arrow->setBrush(QBrush(Qt::black));
    group->addToGroup(arrow);

    // Draw text for the bent pitch (e.g. "Full", "3/4", etc). Don't draw the
    // text if the bend is returning to standard pitch.
    if (pitch != 0)
    {
        mySymbolTextFont.setStyle(QFont::StyleNormal);
        auto bendText = new SimpleTextItem(
            QString::fromStdString(Bend::getPitchText(pitch)),
            mySymbolTextFont);
        bendText->setPos(right - 0.5 * bendText->boundingRect().width(),
                         yEnd - 1.75 * mySymbolTextFont.pixelSize());
        group->addToGroup(bendText);
    }
}

QGraphicsItem *SystemRenderer::createBendGroup(const SymbolGroup &group,
                                               const LayoutInfo &layout)
{
    auto itemGroup = new QGraphicsItemGroup();
    double prevX = 0.0;

    for (int i = group.getLeftPosition(); i < group.getRightPosition(); ++i)
    {
        const Position *pos =
            ScoreUtils::findByPosition(group.getVoice().getPositions(), i);
        if (!pos)
            continue;

        for (const Note &note : pos->getNotes())
        {
            if (!note.hasBend())
                continue;
            
            const Bend &bend = note.getBend();
            const Bend::BendType type = bend.getType();
            
            const double x = layout.getPositionX(i);
            const double leftX = x + 0.75 * layout.getPositionSpacing();

            // Normally, the bend extends to the end of our position.
            double rightX = x + layout.getPositionSpacing();

            int duration = bend.getDuration();
            // Bend-and-releases always last for the duration of the note.
            if (type == Bend::BendAndRelease || type == Bend::PreBendAndRelease)
                duration = 1;

            // If the bend stretches over the current note or additional notes,
            // figure out how far the bend extends.
            if (duration > 0)
            {
                const Position *nextPos = pos;
                for (int j = 0; j < duration && nextPos; ++j)
                {
                    nextPos = VoiceUtils::getNextPosition(
                        group.getVoice(), nextPos->getPosition());
                }

                rightX = nextPos ? layout.getPositionX(nextPos->getPosition())
                                 : LayoutInfo::STAFF_WIDTH - layout.getPositionSpacing();
                // Move to the middle of the position.
                rightX += 0.5 * layout.getPositionSpacing();
            }

            const double yStart = getBendHeight(bend.getStartPoint(), note, layout);
            const double yEnd = getBendHeight(bend.getEndPoint(), note, layout);
            // If the bend has a release, move the final position up a bit to
            // make room for the tab note.
            const double yRelease = yEnd - 0.5 * layout.getTabLineSpacing();
            
            if (type == Bend::ImmediateRelease)
                createDashedLine(itemGroup, prevX, rightX, yStart);
            else if (type == Bend::GradualRelease)
            {
                // Draw a dashed line to the original bend.
                auto line = new QGraphicsLineItem(
                    prevX, yStart, x + layout.getPositionSpacing(), yStart);
                line->setPen(QPen(Qt::black, 1, Qt::DashLine));
                itemGroup->addToGroup(line);

                // Draw the bend down to the new pitch.
                createBend(itemGroup, x + layout.getPositionSpacing(), rightX,
                           yStart, yRelease, bend.getReleasePitch(), false);
            }
            else if (type == Bend::NormalBend || type == Bend::BendAndHold ||
                     type == Bend::PreBend || type == Bend::PreBendAndHold)
            {
                createBend(
                    itemGroup, leftX, rightX, yStart, yEnd, bend.getBentPitch(),
                    type == Bend::PreBend || type == Bend::PreBendAndHold);
            }
            else if (type == Bend::BendAndRelease ||
                     type == Bend::PreBendAndRelease)
            {
                const double middleX = (type == Bend::BendAndRelease)
                                           ? 0.5 * (leftX + rightX)
                                           : x + layout.getPositionSpacing();

                // Figure out where to draw the first part of the bend.
                Bend::DrawPoint drawPoint = Bend::MidPoint;
                if (bend.getStartPoint() == Bend::MidPoint ||
                    bend.getEndPoint() == Bend::MidPoint)
                {
                    drawPoint = Bend::HighPoint;
                }

                const double yMiddle = getBendHeight(drawPoint, note, layout);
                createBend(itemGroup, leftX, middleX, yStart, yMiddle,
                           bend.getBentPitch(),
                           type == Bend::PreBendAndRelease);

                // Draw the second part of the bend.
                createBend(itemGroup, middleX, rightX, yMiddle, yRelease,
                           bend.getReleasePitch(), false);
            }

            prevX = rightX;
            break;
        }
    }

    return itemGroup;
}
