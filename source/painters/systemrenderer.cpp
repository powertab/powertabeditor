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

#include <app/common.h>
#include <app/scorearea.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <painters/barlinepainter.h>
#include <painters/clefpainter.h>
#include <painters/directionpainter.h>
#include <painters/keysignaturepainter.h>
#include <painters/layoutinfo.h>
#include <painters/staffpainter.h>
#include <painters/tabnotepainter.h>
#include <painters/tempomarkerpainter.h>
#include <painters/timesignaturepainter.h>
#include <QBrush>
#include <QDebug>
#include <QGraphicsItem>
#include <QPen>
#include <score/scorelocation.h>
#include <score/system.h>

SystemRenderer::SystemRenderer(const ScoreArea *scoreArea, const Score &score)
    : myScoreArea(scoreArea),
      myScore(score),
      myParentSystem(NULL),
      myParentStaff(NULL),
      myMusicNotationFont(myMusicFont.getFont()),
      myPlainTextFont("Liberation Sans"),
      mySymbolTextFont("Liberation Sans"),
      myRehearsalSignFont("Helvetica")
{
    myPlainTextFont.setPixelSize(10);
    mySymbolTextFont.setPixelSize(9);
    myRehearsalSignFont.setPixelSize(12);
}

QGraphicsItem *SystemRenderer::operator()(const System &system,
                                          int systemIndex, Staff::ViewType view)
{
    // Draw the bounding rectangle for the system.
    myParentSystem = new QGraphicsRectItem();
    myParentSystem->setPen(QPen(QBrush(QColor(0, 0, 0, 127)), 0.5));

    // Draw each staff.
    double height = 0;
    int i = 0;
    BOOST_FOREACH(const Staff &staff, system.getStaves())
    {
        if (staff.getViewType() != view)
        {
            ++i;
            continue;
        }

        const bool isFirstStaff = (height == 0);
        LayoutConstPtr layout = boost::make_shared<LayoutInfo>(myScore, system,
                                                               staff);

        if (isFirstStaff)
        {
            drawSystemSymbols(system, *layout);
            height += layout->getSystemSymbolSpacing();
        }

        myParentStaff = new StaffPainter(layout,
                                         ScoreLocation(myScore, systemIndex, i),
                                         myScoreArea->getSelectionPubSub());
        myParentStaff->setPos(0, height);
        myParentStaff->setParentItem(myParentSystem);
        height += layout->getStaffHeight();

        // Draw the clefs.
        ClefPainter* clef = new ClefPainter(staff.getClefType(),
                                            myMusicNotationFont);
        clef->setPos(LayoutInfo::CLEF_PADDING, layout->getTopStdNotationLine());
        clef->setParentItem(myParentStaff);

        drawTabClef(LayoutInfo::CLEF_PADDING, *layout);

        drawBarlines(system, systemIndex, layout, isFirstStaff);
        drawTabNotes(staff, layout);
        drawLegato(staff, *layout);

        drawSymbolsAboveStdNotationStaff(*layout);
        drawSymbolsBelowStdNotationStaff(*layout);
        drawSymbolsBelowTabStaff(*layout);

        drawPlayerChanges(system, i, *layout);

        ++i;
    }

    myParentSystem->setRect(0, 0, LayoutInfo::STAFF_WIDTH, height);
    return myParentSystem;
}

void SystemRenderer::drawTabClef(double x, const LayoutInfo &layout)
{
    QGraphicsSimpleTextItem *tabClef = new QGraphicsSimpleTextItem();

    // Determine the size of the clef symbol based on the number of strings and
    // the line spacing.
    const int pixelSize = (layout.getStringCount() - 1) *
            layout.getTabLineSpacing() * 0.6;

    // Position the clef symbol.
    tabClef->setPos(x, layout.getTopTabLine() - pixelSize / 2.1);
    myMusicFont.setSymbol(tabClef, MusicFont::TabClef, pixelSize);
    tabClef->setParentItem(myParentStaff);
}

void SystemRenderer::drawBarlines(const System &system, int systemIndex,
                                  const LayoutConstPtr &layout,
                                  bool isFirstStaff)
{
    BOOST_FOREACH(const Barline &barline, system.getBarlines())
    {
        const ScoreLocation location(myScore, systemIndex, -1,
                                     barline.getPosition());

        const KeySignature &keySig = barline.getKeySignature();
        const TimeSignature &timeSig = barline.getTimeSignature();

        BarlinePainter *barlinePainter = new BarlinePainter(layout, barline,
                location, myScoreArea->getBarlinePubSub());

        double x = layout->getPositionX(barline.getPosition());
        double keySigX = x + barlinePainter->boundingRect().width() - 1;
        double timeSigX = x + barlinePainter->boundingRect().width() +
                layout->getWidth(keySig);
        double rehearsalSignX = x;

        if (barline == system.getBarlines().front()) // Start bar of system.
        {
            // For normal bars, display a line at the far left edge.
            if (barline.getBarType() == Barline::SingleBar)
            {
                x = 0 - barlinePainter->boundingRect().width() / 2 - 0.5;
            }
            else
            {
                // Otherwise, display the bar after the clef, etc, and to the
                // left of the first note.
                x = layout->getFirstPositionX() - layout->getPositionSpacing();
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
                        myScoreArea->getKeySignaturePubSub());

            keySigPainter->setPos(keySigX, layout->getTopStdNotationLine());
            keySigPainter->setParentItem(myParentStaff);
        }

        if (timeSig.isVisible())
        {
            TimeSignaturePainter *timeSigPainter = new TimeSignaturePainter(
                        layout, timeSig, location,
                        myScoreArea->getTimeSignaturePubSub());

            timeSigPainter->setPos(timeSigX, layout->getTopStdNotationLine());
            timeSigPainter->setParentItem(myParentStaff);
        }

        if (barline.hasRehearsalSign() && isFirstStaff)
        {
            const RehearsalSign &sign = barline.getRehearsalSign();
            const int y = 1;

            QGraphicsSimpleTextItem *signLetters = new QGraphicsSimpleTextItem();
            signLetters->setText(QString::fromStdString(sign.getLetters()));
            signLetters->setPos(rehearsalSignX, y);
            signLetters->setFont(myRehearsalSignFont);

            QGraphicsSimpleTextItem *signText = new QGraphicsSimpleTextItem();
            signText->setText(QString::fromStdString(sign.getDescription()));
            signText->setFont(myRehearsalSignFont);
            signText->setPos(rehearsalSignX +
                             signLetters->boundingRect().width() + 7, y);

            // Draw rectangle around rehearsal sign letters.
            QRectF boundingRect = signLetters->boundingRect();
            boundingRect.setWidth(boundingRect.width() + 7);
            boundingRect.translate(-4, 0);
            QGraphicsRectItem *rect = new QGraphicsRectItem(boundingRect);
            rect->setPos(rehearsalSignX, y);

            rect->setParentItem(myParentSystem);
            signText->setParentItem(myParentSystem);
            signLetters->setParentItem(myParentSystem);
        }
    }
}

void SystemRenderer::drawTabNotes(const Staff &staff,
                                  const LayoutConstPtr &layout)
{
    for (int voice = 0; voice < Staff::NUM_VOICES; ++voice)
    {
        BOOST_FOREACH(const Position &pos, staff.getVoice(voice))
        {
            if (pos.isRest() || pos.getNotes().empty())
                continue;

            const double location = layout->getPositionX(pos.getPosition());

            BOOST_FOREACH(const Note &note, pos.getNotes())
            {
                TabNotePainter *tabNote = new TabNotePainter(note);
                centerItem(tabNote, location,
                           location + layout->getPositionSpacing(),
                           layout->getTabLine(note.getString()) +
                           layout->getTabLineSpacing() / 2 - 1);
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

void SystemRenderer::centerItem(QGraphicsItem *item, double xmin,
                                double xmax, double y)
{
    double itemWidth = item->boundingRect().width();
    double centredX = xmin + ((xmax - (xmin + itemWidth)) / 2);
    item->setPos(centredX, y);
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
    const QChar arpeggioSymbol = MusicFont::getSymbol(MusicFont::Vibrato);
    const double symbolWidth = QFontMetricsF(myMusicNotationFont).width(
                arpeggioSymbol);
    const int numSymbols = height / symbolWidth;

    QGraphicsSimpleTextItem *arpeggio = new QGraphicsSimpleTextItem(
                QString(numSymbols, arpeggioSymbol));
    arpeggio->setFont(myMusicNotationFont);
    arpeggio->setPos(x + arpeggio->boundingRect().height() / 2.0 - 3.0, top);
    arpeggio->setRotation(90);
    arpeggio->setParentItem(myParentStaff);

    // Draw the end of the arpeggio.
    const QChar arpeggioEnd = position.hasProperty(Position::ArpeggioUp) ?
                MusicFont::getSymbol(MusicFont::ArpeggioUp) :
                MusicFont::getSymbol(MusicFont::ArpeggioDown);

    QGraphicsSimpleTextItem *endPoint = new QGraphicsSimpleTextItem(arpeggioEnd);
    const double y = position.hasProperty(Position::ArpeggioUp) ? top : bottom;
    endPoint->setFont(myMusicNotationFont);
    endPoint->setPos(x, y - 1.45 * myMusicNotationFont.pixelSize());
    endPoint->setParentItem(myParentStaff);
}

void SystemRenderer::drawSystemSymbols(const System &system,
                                       const LayoutInfo& layout)
{
    double height = 0;

    // Allocate space for any rehearsal signs - they are drawn at the same
    // time as barlines.
    BOOST_FOREACH(const Barline &barline, system.getBarlines())
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

#if 0 // TODO - implement chord text for the new file format.
    if (system->GetChordTextCount() > 0)
    {
        drawChordText(height, currentStaffInfo);
        height += System::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(currentStaffInfo, height);
    }
#endif
}

void SystemRenderer::drawDividerLine(double y)
{
    QGraphicsLineItem *line = new QGraphicsLineItem();
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

    BOOST_FOREACH(const AlternateEnding &ending, system.getAlternateEndings())
    {
        const double location = layout.getPositionX(ending.getPosition());

        // Draw the vertical line.
        QGraphicsLineItem *vertLine = new QGraphicsLineItem();
        vertLine->setLine(0, TOP_LINE_OFFSET, 0,
                          LayoutInfo::SYSTEM_SYMBOL_SPACING - TOP_LINE_OFFSET);
        vertLine->setPos(location, height);
        vertLine->setParentItem(myParentSystem);

        // Draw the text indicating the repeat numbers.
        QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(
                QString::fromStdString(boost::lexical_cast<std::string>(ending)));
        text->setFont(myPlainTextFont);
        text->setPos(location + TEXT_PADDING, height + TEXT_PADDING / 2.0);
        text->setParentItem(myParentSystem);

        // The horizontal line either stretches to the next repeat end bar
        // in the system, or just to the next bar.
        double endX = 0;
        BOOST_FOREACH(const Barline &barline, system.getBarlines())
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

        // Ensure that the line doesn't extend past the edge of the system.
        endX = Common::clamp(endX, 0.0, LayoutInfo::STAFF_WIDTH);

        QGraphicsLineItem *horizLine = new QGraphicsLineItem();
        horizLine->setLine(0, TOP_LINE_OFFSET, endX - location, TOP_LINE_OFFSET);
        horizLine->setPos(location, height);
        horizLine->setParentItem(myParentSystem);
    }
}

void SystemRenderer::drawTempoMarkers(const System &system,
                                      const LayoutInfo &layout,
                                      double height)
{
    BOOST_FOREACH(const TempoMarker &tempo, system.getTempoMarkers())
    {
        const double location = layout.getPositionX(tempo.getPosition());

        TempoMarkerPainter *painter = new TempoMarkerPainter(tempo);
        painter->setPos(location, height + 4);
        painter->setParentItem(myParentSystem);
    }
}

double SystemRenderer::drawDirections(const System &system,
                                      const LayoutInfo &layout,
                                      double height)
{
    double maxHeight = 0;

    BOOST_FOREACH(const Direction &direction, system.getDirections())
    {
        double localHeight = 0;
        const double location = layout.getPositionX(direction.getPosition());

        BOOST_FOREACH(const DirectionSymbol &symbol, direction.getSymbols())
        {
            DirectionPainter *painter = new DirectionPainter(symbol);
            centerItem(painter, location,
                       location + layout.getPositionSpacing(),
                       height + localHeight + 4);
            painter->setParentItem(myParentSystem);

            localHeight += LayoutInfo::SYSTEM_SYMBOL_SPACING;
        }

        maxHeight = std::max(maxHeight, localHeight);
    }

    return maxHeight;
}

#if 0
void SystemRenderer::drawChordText(uint32_t height, const StaffData& currentStaffInfo)
{
    for (uint32_t i = 0; i < system->GetChordTextCount(); i++)
    {
        shared_ptr<const ChordText> chordText = system->GetChordText(i);

        const quint32 location = system->GetPositionX(chordText->GetPosition());

        ChordTextPainter* chordTextPainter = new ChordTextPainter(chordText);
        centerItem(chordTextPainter, location, location + currentStaffInfo.positionWidth, height + 4);
        chordTextPainter->setParentItem(parentSystem);
    }
}

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
    BOOST_FOREACH(RhythmSlashPainter* painter, slashPainters)
    {
        painter->setParentItem(parentSystem);
    }
}

#endif

void SystemRenderer::drawLegato(const Staff &staff, const LayoutInfo &layout)
{
    for (int voice = 0; voice < Staff::NUM_VOICES; ++voice)
    {
        std::map<int, int> arcs;

        BOOST_FOREACH(const Position &pos, staff.getVoice(voice))
        {
            const int position = pos.getPosition();

            BOOST_FOREACH(const Note &note, pos.getNotes())
            {
                const int string = note.getString();

                // TODO - include legato slides here.
                if (note.hasProperty(Note::HammerOn) ||
                    note.hasProperty(Note::PullOff))
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

                    QGraphicsPathItem *arc = new QGraphicsPathItem(path);
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

                    QGraphicsPathItem *arc = new QGraphicsPathItem(path);
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
    BOOST_FOREACH(const PlayerChange &change, system.getPlayerChanges())
    {
        const std::vector<ActivePlayer> activePlayers =
                change.getActivePlayers(staffIndex);
        if (activePlayers.empty())
            continue;

        QString description;
        BOOST_FOREACH(const ActivePlayer &player, activePlayers)
        {
            if (!description.isEmpty())
                description += ", ";
            description += QString::number(player.getPlayerNumber() + 1);
        }

        QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(
                    "Player " + description);
        text->setFont(myPlainTextFont);
        text->setPos(layout.getPositionX(change.getPosition()),
                     layout.getBottomStdNotationLine() +
                     LayoutInfo::STAFF_BORDER_SPACING);
        text->setParentItem(myParentStaff);
    }
}

#if 0
/// Draws all of the slides for a staff
void SystemRenderer::drawSlides(const StaffData& currentStaffInfo)
{
    for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        // iterate across the staff for each string
        for (quint8 string = 0; string < staff->GetTablatureStaffType(); string++)
        {
            const size_t positionCount = staff->GetPositionCount(voice);
            for (size_t i = 0; i < positionCount; i++)
            {
                const Position* currentPosition = staff->GetPosition(voice, i);
                const Note* note = currentPosition->GetNoteByString(string);

                if (note == NULL)
                {
                    continue;
                }

                quint8 type = 0;
                qint8 steps = 0;

                // draw any slides out of the note
                note->GetSlideOutOf(type, steps);

                if (type != Note::slideOutOfNone)
                {
                    // figure out if we're sliding up or down
                    const bool slideUp = (type == Note::slideOutOfUpwards) || (steps > 0);
                    const uint32_t curPosIndex = currentPosition->GetPosition();

                    // Get the index of the next position, and also handle the
                    // edge case where we are at the end of a system.
                    const uint32_t nextPosIndex = std::max(staff->GetIndexOfNextPosition(voice, system, currentPosition),
                                                           curPosIndex + 1);

                    drawSlidesHelper(currentStaffInfo, string, slideUp,
                                     curPosIndex, nextPosIndex);
                }

                // draw any slides into the note
                note->GetSlideInto(type);
                if (type != Note::slideIntoNone)
                {
                    const int currentPosIndex = currentPosition->GetPosition();
                    const bool slideUp = (type == Note::slideIntoFromBelow);
                    const int prevPosIndex = currentPosIndex - 1;

                    drawSlidesHelper(currentStaffInfo, string, slideUp,
                                     prevPosIndex, currentPosIndex);
                }
            }
        }
    }
}

/// Helper function to perform the actual rendering of a slide
/// Draws a slide on the given string, between the two position indexes
void SystemRenderer::drawSlidesHelper(const StaffData& currentStaffInfo,
                                      quint8 string, bool slideUp, int posIndex1,
                                      int posIndex2)
{
    Q_ASSERT(posIndex1 <= posIndex2);

    double leftPos = system->GetPositionX(std::max(posIndex1, 0));
    // Handle slides at the first position of a system.
    if (posIndex1 < 0)
    {
        leftPos -= system->GetPositionSpacing();
    }

    const int numPositions = system->GetPositionCount();
    double rightPos = system->GetPositionX(std::min(posIndex2, numPositions));
    if (posIndex2 > numPositions)
    {
        rightPos += system->GetPositionSpacing();
    }
    const double width = rightPos - leftPos;

    double height = 5;
    double y = (currentStaffInfo.getTabLineHeight(string + 1) + currentStaffInfo.getTabLineHeight(string)) / 2;

    if (slideUp) // if we're sliding up, flip the diagonal line
    {
        height = -height;
        y += currentStaffInfo.tabLineSpacing + 1;
    }

    QGraphicsLineItem* slide = new QGraphicsLineItem(parentStaff);
    slide->setLine(0, 0, width - currentStaffInfo.positionWidth / 2, height);
    slide->setPos(leftPos + currentStaffInfo.positionWidth / 1.5 + 1,
                  y + height / 2);
}

#endif

void SystemRenderer::drawSymbolsBelowTabStaff(const LayoutInfo& layout)
{
    BOOST_FOREACH(const SymbolGroup &symbolGroup, layout.getTabStaffBelowSymbols())
    {
        QGraphicsItem* renderedSymbol = NULL;

        switch (symbolGroup.getSymbolType())
        {
        case SymbolGroup::PickStrokeUp:
            renderedSymbol = createPickStroke(MusicFont::getSymbol(
                                                  MusicFont::PickStrokeUp));
            break;
        case SymbolGroup::PickStrokeDown:
            renderedSymbol = createPickStroke(MusicFont::getSymbol(
                                                  MusicFont::PickStrokeDown));
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
        // TODO - handle slides and harmonics.
        default:
            Q_ASSERT(false);
            break;
        }

#if 0
        case Layout::SymbolSlide:
            renderedSymbol = createPlainText("sl.", QFont::StyleItalic);
            break;

        case Layout::SymbolArtificialHarmonic:
            renderedSymbol = createArtificialHarmonicText(
                        staff->GetPositionByPosition(symbolGroup.voice,
                                                     symbolGroup.leftPosIndex));
            break;
#endif

        centerItem(renderedSymbol, symbolGroup.getX(),
                   symbolGroup.getX() + symbolGroup.getWidth(),
                   layout.getBottomTabLine() +
                   symbolGroup.getHeight() * LayoutInfo::TAB_SYMBOL_SPACING);

        renderedSymbol->setParentItem(myParentStaff);
    }
}

QGraphicsItem *SystemRenderer::createPickStroke(const QString &text)
{
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(text);
    textItem->setFont(myMusicNotationFont);
    textItem->setPos(2, -28);

    // Sticking the text in a QGraphicsItemGroup allows us to offset the
    // position of the text from its default location
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    group->addToGroup(textItem);
    return group;
}

QGraphicsItem *SystemRenderer::createPlainTextSymbol(const QString &text,
                                                     QFont::Style style)
{
    myPlainTextFont.setStyle(style);

    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(text);
    textItem->setFont(myPlainTextFont);
    textItem->setPos(0, -8);

    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    group->addToGroup(textItem);
    return group;
}

#if 0
/// Creates the text portion of an artificial harmonic - displaying the note value
QGraphicsItem* SystemRenderer::createArtificialHarmonicText(const Position* position)
{
    std::vector<const Note*> notes;
    position->GetNotes(notes);

    // find the note with an artificial harmonic
    std::vector<const Note*>::const_iterator location = std::find_if(notes.begin(), notes.end(),
                                                               std::mem_fun(&Note::HasArtificialHarmonic));

     // this function should not even be executed if there is no note with an artificial harmonic
    Q_ASSERT(location != notes.end());

    const Note* note = *location;

    quint8 key = 0, variation = 0, octave = 0;
    note->GetArtificialHarmonic(key, variation, octave);

    // use the ChordName class to convert the key/variation data to a text representation
    ChordName name(key, variation, ChordName::DEFAULT_FORMULA, ChordName::DEFAULT_FORMULA_MODIFICATIONS);

    const QString text = QString().fromStdString(name.GetKeyText(false));

    return createPlainText(text, QFont::StyleNormal);
}

/// Draws the symbols that appear between the tab and standard notation staves
void SystemRenderer::drawSymbols(const StaffData& staffInfo)
{
    std::vector<Layout::SymbolGroup> symbolGroups;
    Layout::CalculateSymbolLayout(symbolGroups, myScore, system, staff);

    BOOST_FOREACH(const Layout::SymbolGroup& symbolGroup, symbolGroups)
    {
        QGraphicsItem* renderedSymbol = NULL;

        const int width = Common::clamp(symbolGroup.width, 0, system->GetRect().GetWidth() - symbolGroup.leftX);

        switch(symbolGroup.symbolType)
        {
        case Layout::SymbolLetRing:
            renderedSymbol = createConnectedSymbolGroup("let ring", QFont::StyleItalic, width, staffInfo);
            break;

        case Layout::SymbolVolumeSwell:
        {
            // figure out the direction of the volume swell
            uint8_t startVolume = 0, endVolume = 0, duration = 0;
            staff->GetPosition(0, symbolGroup.leftPosIndex)->GetVolumeSwell(startVolume, endVolume, duration);

            renderedSymbol = createVolumeSwell(width, staffInfo,
                                               (startVolume <= endVolume) ? VolumeIncreasing : VolumeDecreasing);
            break;
        }

        case Layout::SymbolVibrato:
            renderedSymbol = drawContinuousFontSymbols(MusicFont::Vibrato, width);
            break;

        case Layout::SymbolWideVibrato:
            renderedSymbol = drawContinuousFontSymbols(MusicFont::WideVibrato, width);
            break;

        case Layout::SymbolPalmMuting:
            renderedSymbol = createConnectedSymbolGroup("P.M.", QFont::StyleNormal, width, staffInfo);
            break;

        case Layout::SymbolTremoloPicking:
            renderedSymbol = createTremoloPicking(staffInfo);
            break;

        case Layout::SymbolTremoloBar:
            renderedSymbol = new TremoloBarPainter(staff->GetPosition(0, symbolGroup.leftPosIndex),
                                                   width);
            break;

        case Layout::SymbolTrill:
            renderedSymbol = createTrill(staffInfo);
            break;

        case Layout::SymbolNaturalHarmonic:
            renderedSymbol = createConnectedSymbolGroup("N.H.", QFont::StyleNormal, width, staffInfo);
            break;

        case Layout::SymbolArtificialHarmonic:
            renderedSymbol = createConnectedSymbolGroup("A.H.", QFont::StyleNormal, width, staffInfo);
            break;

        case Layout::SymbolDynamic:
        {
            const Position* pos = staff->GetPosition(0, symbolGroup.leftPosIndex);
            renderedSymbol = createDynamic(myScore->FindDynamic(myScore->FindSystemIndex(system),
                                                              system->FindStaffIndex(staff),
                                                              pos->GetPosition()));
            break;
        }

        case Layout::SymbolBend:
        {
            renderedSymbol = createBend(staff->GetPosition(0, symbolGroup.leftPosIndex), staffInfo);
            break;
        }

        default:
            Q_ASSERT(false); // all symbol types should have been dealt with by now ...
            break;
        }

        if (symbolGroup.symbolType != Layout::SymbolBend)
        {
            renderedSymbol->setPos(symbolGroup.leftX,
                                   staffInfo.getTopTabLine() - (symbolGroup.height + 1) * Staff::TAB_SYMBOL_HEIGHT);
        }
        else
        {
            renderedSymbol->setPos(symbolGroup.leftX, 0);
        }

        renderedSymbol->setParentItem(parentStaff);
    }
}

#endif

void SystemRenderer::drawSymbolsAboveStdNotationStaff(const LayoutInfo& layout)
{
    BOOST_FOREACH(const SymbolGroup &symbolGroup,
                  layout.getStdNotationStaffAboveSymbols())
    {
        QGraphicsItem *renderedSymbol = NULL;

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

        renderedSymbol->setPos(symbolGroup.getX(), 0);
        renderedSymbol->setParentItem(myParentStaff);
    }
}

void SystemRenderer::drawSymbolsBelowStdNotationStaff(const LayoutInfo& layout)
{
    BOOST_FOREACH(const SymbolGroup &symbolGroup,
                  layout.getStdNotationStaffBelowSymbols())
    {
        QGraphicsItem *renderedSymbol = NULL;

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

        renderedSymbol->setPos(symbolGroup.getX(),
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
    QGraphicsSimpleTextItem *description = new QGraphicsSimpleTextItem ();
    description->setText(text);
    description->setFont(mySymbolTextFont);

    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    group->addToGroup(description);

    // Draw dashed line across the remaining positions in the group.
    if (width > layout.getPositionSpacing())
    {
        const double rightEdge = width - 0.5 * layout.getPositionSpacing();
        const double leftEdge = description->boundingRect().right();
        const double middleHeight = LayoutInfo::TAB_SYMBOL_SPACING / 2.0;

        QGraphicsLineItem *line = new QGraphicsLineItem(leftEdge, middleHeight,
                                                        rightEdge, middleHeight);
        line->setPen(QPen(Qt::black, 1, Qt::DashLine));
        group->addToGroup(line);

        // Draw a vertical line at the end of the dotted lines.
        QGraphicsLineItem *lineEnd = new QGraphicsLineItem(
                    line->boundingRect().right(), 1,
                    line->boundingRect().right(),
                    LayoutInfo::TAB_SYMBOL_SPACING - 1);
        group->addToGroup(lineEnd);
    }

    return group;
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

QGraphicsItem* SystemRenderer::drawContinuousFontSymbols(QChar symbol, int width)
{
    QFont font = musicFont.getFont();
    font.setPixelSize(25);

    const double symbolWidth = QFontMetricsF(font).width(symbol);
    const int numSymbols = width / symbolWidth;
    QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(QString(numSymbols, symbol));
    text->setFont(font);
    text->setPos(0, -25);

    // a bit of a hack for getting around the height offset caused by the music font
    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(text);

    return group;
}

QGraphicsItem* SystemRenderer::createTremoloPicking(const StaffData& currentStaffInfo)
{
    const double offset = Staff::TAB_SYMBOL_HEIGHT / 3;

    QGraphicsItemGroup* group = new QGraphicsItemGroup;

    for (int i = 0; i < 3; i++)
    {
        QGraphicsSimpleTextItem* line = new QGraphicsSimpleTextItem(MusicFont::getSymbol(MusicFont::TremoloPicking));
        line->setFont(musicNotationFont);
        centerItem(line, 0, currentStaffInfo.positionWidth * 1.25, -37 + i * offset);
        group->addToGroup(line);
    }

    return group;
}

QGraphicsItem* SystemRenderer::createTrill(const StaffData& currentStaffInfo)
{
    QFont font(musicFont.getFont());
    font.setPixelSize(21);

    QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(MusicFont::getSymbol(MusicFont::Trill));
    text->setFont(font);
    centerItem(text, 0, currentStaffInfo.positionWidth, -18);

    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(text);

    return group;
}

QGraphicsItem* SystemRenderer::createDynamic(boost::shared_ptr<const Dynamic> dynamic)
{
    QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(QString::fromStdString(dynamic->GetText(false)));
    textItem->setFont(musicNotationFont);
    textItem->setPos(0, -20);

    // Sticking the text in a QGraphicsItemGroup allows us to offset the position of the text from its default location
    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(textItem);

    return group;
}

void SystemRenderer::drawStdNotation(const StaffData& currentStaffInfo)
{
    System::BarlineConstPtr currentBarline;
    System::BarlineConstPtr prevBarline = system->GetStartBar();

    QList<StdNotationPainter*> notePainters;
    QMultiMap<int, StdNotationPainter*> accidentalsMap;
    std::vector<NoteStem> stems;

    for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        for (quint32 i=0; i < staff->GetPositionCount(voice); i++)
        {
            const Position* currentPosition = staff->GetPosition(voice, i);
            const quint32 location = system->GetPositionX(currentPosition->GetPosition());
            currentBarline = system->GetPrecedingBarline(currentPosition->GetPosition());
            const KeySignature& currentKeySig = currentBarline->GetKeySignature();

            // Find the guitar corresponding to the current staff
            shared_ptr<Guitar> currentGuitar = myScore->GetGuitar(system->FindStaffIndex(staff));
            Q_ASSERT(currentGuitar);

            std::vector<int> noteLocations;

            // if we reach a new bar, we can adjust all of the accidentals for the previous bar
            if (currentBarline != prevBarline)
            {
                adjustAccidentals(accidentalsMap);
                accidentalsMap.clear();
                prevBarline = currentBarline;
            }

            if (currentPosition->HasMultibarRest())
            {
                uint8_t measureCount = 0;
                currentPosition->GetMultibarRest(measureCount);
                drawMultiBarRest(currentBarline, currentStaffInfo, measureCount);
                continue;
            }

            // just draw rests right away, since we don't have to worry about beaming or accidentals
            if (currentPosition->IsRest())
            {
                RestPainter* restPainter = new RestPainter(*currentPosition);
                centerItem(restPainter, location, location + currentStaffInfo.positionWidth * 1.25,
                           currentStaffInfo.getTopStdNotationLine());
                restPainter->setParentItem(parentStaff);
                continue;
            }

            StdNotationPainter* stdNotePainter = NULL;
            for (uint32_t j = 0; j < currentPosition->GetNoteCount(); j++)
            {
                const Note* note = currentPosition->GetNote(j);

                stdNotePainter = new StdNotationPainter(currentStaffInfo, staff, currentPosition,
                                                        note, currentGuitar->GetTuning(), currentKeySig);
                notePainters << stdNotePainter;

                // map all of the notes for each position on the staff, so that we can adjust accidentals later
                accidentalsMap.insert(stdNotePainter->getYLocation(), stdNotePainter);

                noteLocations.push_back(stdNotePainter->getYLocation());
            }

            // add note stem for any notes other than whole notes
            if (NoteStem::needsStem(currentPosition))
            {
                NoteStem stem(currentPosition, location, noteLocations,
                              stdNotePainter->noteHeadWidth(),
                              stdNotePainter->noteHeadRightEdge());
                stems.push_back(stem);
            }

            if (!noteLocations.empty())
            {
                drawLedgerLines(noteLocations, location, currentStaffInfo,
                                stdNotePainter->noteHeadWidth());
            }
        }
    }

    // make sure we adjust accidentals for the last bar of the staff
    adjustAccidentals(accidentalsMap);

    // after adjusting accidentals, etc, we can add the painters to the scene
    foreach(StdNotationPainter* painter, notePainters)
    {
        const quint32 location = system->GetPositionX(painter->getPositionObject()->GetPosition());
        painter->setPos(location, currentStaffInfo.getTopStdNotationLine());
        painter->setParentItem(parentStaff);
    }

    std::vector<NoteStem> currentStemGroup;
    std::vector<NoteStem> updatedStems;

    // group all of the stems into their beaming groups, and draw them
    BOOST_FOREACH(const NoteStem& stem, stems)
    {
        currentStemGroup.push_back(stem);

        if (stem.position->IsBeamEnd() ||
                (!stem.position->IsBeamStart() && currentStemGroup.size() == 1))
        {
            BeamGroup group(currentStaffInfo, currentStemGroup);
            currentStemGroup.clear();

            group.drawStems(parentStaff);

            // grab a copy of the updated note stems for later use
            std::vector<NoteStem> temp;
            group.copyNoteStems(temp);
            updatedStems.insert(updatedStems.end(), temp.begin(), temp.end());
        }
    }

    // Now, draw any irregular note groupings (triplets, etc)
    // This must be done after the beams are drawn, since the note stems will be adjusted during that process
    std::vector<NoteStem> currentIrregularNoteGroup;

    BOOST_FOREACH(const NoteStem& stem, updatedStems)
    {
        // check if this note isn't part of an irregular grouping
        if (!stem.position->IsIrregularGroupingEnd() &&
                !stem.position->IsIrregularGroupingMiddle() &&
                !stem.position->IsIrregularGroupingStart())
        {
            currentIrregularNoteGroup.clear();
            continue;
        }

        currentIrregularNoteGroup.push_back(stem);

        // draw the grouping
        if (stem.position->IsIrregularGroupingEnd())
        {
            IrregularNoteGroup irregularGroup(currentIrregularNoteGroup);
            irregularGroup.draw(parentStaff);

            currentIrregularNoteGroup.clear();
        }
    }
}

void SystemRenderer::adjustAccidentals(QMultiMap<int, StdNotationPainter*>& accidentalsMap)
{
    QList<int> keys = accidentalsMap.uniqueKeys();
    QList<int>::const_iterator i = keys.begin();

    while(i != keys.end())
    {
        QList<StdNotationPainter*> notes = accidentalsMap.values(*i);
        int currentAccidental = StdNotationPainter::NO_ACCIDENTAL;

        for (int j = notes.size() - 1; j >= 0; j--)
        {
            StdNotationPainter* note = notes.at(j);

            if (note->accidental == currentAccidental)
            {
                note->accidental = StdNotationPainter::NO_ACCIDENTAL;
            }
            else
            {
                currentAccidental = note->accidental;
                if (currentAccidental == StdNotationPainter::NO_ACCIDENTAL)
                {
                    // If we return to a note in the key signature, force its
                    // accidental (or a natural sign) to be displayed.
                    note->refreshAccidental(true);
                }
            }
        }

        ++i;
    }
}

void SystemRenderer::drawMultiBarRest(boost::shared_ptr<const Barline> currentBarline,
                                      const StaffData& currentStaffInfo, int measureCount)
{
    System::BarlineConstPtr nextBarline = system->GetNextBarline(currentBarline->GetPosition());

    const double leftX = (currentBarline->GetPosition() == 0) ?
                system->GetPositionX(currentBarline->GetPosition()) :
                system->GetPositionX(currentBarline->GetPosition() + 1);

    const double rightX = Common::clamp(static_cast<double>(system->GetPositionX(nextBarline->GetPosition())),
										0.0, system->GetRect().GetWidth() - system->GetPositionSpacing() / 2.0);

    // draw measure count
    QGraphicsSimpleTextItem* measureCountText = new QGraphicsSimpleTextItem;
    measureCountText->setText(QString::number(measureCount));
    measureCountText->setFont(musicNotationFont);

    centerItem(measureCountText, leftX, rightX,
               currentStaffInfo.getTopStdNotationLine() - musicNotationFont.pixelSize() * 1.5);

    measureCountText->setParentItem(parentStaff);

    // draw symbol across std. notation staff
    QGraphicsLineItem* vertLineLeft = new QGraphicsLineItem(leftX,
                                                            currentStaffInfo.getStdNotationLineHeight(2),
                                                            leftX,
                                                            currentStaffInfo.getStdNotationLineHeight(4));
    vertLineLeft->setParentItem(parentStaff);

    QGraphicsLineItem* vertLineRight = new QGraphicsLineItem(rightX,
                                                             currentStaffInfo.getStdNotationLineHeight(2),
                                                             rightX,
                                                             currentStaffInfo.getStdNotationLineHeight(4));
    vertLineRight->setParentItem(parentStaff);

    QGraphicsRectItem* horizontalLine = new QGraphicsRectItem(leftX,
                                                              currentStaffInfo.getStdNotationLineHeight(2) +
                                                              0.5 * Staff::STD_NOTATION_LINE_SPACING,
                                                              rightX - leftX,
                                                              Staff::STD_NOTATION_LINE_SPACING * 0.9);
    horizontalLine->setBrush(QBrush(Qt::black));
    horizontalLine->setParentItem(parentStaff);
}

/// Draws ledger lines for all notes at a position
/// @param noteLocations - List of y-coordinates of all notes at the position
void SystemRenderer::drawLedgerLines(const std::vector<int> &noteLocations,
                                     const double xLocation,
                                     const StaffData& staffData,
                                     const double noteHeadWidth)
{
    const int highestNote = *std::min_element(noteLocations.begin(), noteLocations.end());
    const int lowestNote = *std::max_element(noteLocations.begin(), noteLocations.end());

    const int numLedgerLinesTop = -highestNote / Staff::STD_NOTATION_LINE_SPACING;
    const int numLedgerLinesBottom = (lowestNote - staffData.getStdNotationStaffSize()) /
                                      Staff::STD_NOTATION_LINE_SPACING;

    std::vector<int> ledgerLineLocations;

    if (numLedgerLinesTop > 0)
    {
        const int topLineLocation = staffData.getTopStdNotationLine();
        for (int i = 1; i<= numLedgerLinesTop; i++)
        {
            ledgerLineLocations.push_back(topLineLocation - i * Staff::STD_NOTATION_LINE_SPACING);
        }
    }
    if (numLedgerLinesBottom > 0)
    {
        const int bottomLineLocation = staffData.getBottomStdNotationLine();
        for (int i = 1; i<= numLedgerLinesBottom; i++)
        {
            ledgerLineLocations.push_back(bottomLineLocation + i * Staff::STD_NOTATION_LINE_SPACING);
        }
    }

    const double ledgerLineWidth = noteHeadWidth * 2;
    QPainterPath path;

    for (std::vector<int>::const_iterator location = ledgerLineLocations.begin();
         location != ledgerLineLocations.end(); ++location)
    {
        path.moveTo(0, *location);
        path.lineTo(ledgerLineWidth, *location);
    }

    QGraphicsPathItem* ledgerlines = new QGraphicsPathItem(path, parentStaff);
    centerItem(ledgerlines, xLocation, xLocation + staffData.positionWidth, 0);
}

QGraphicsItem* SystemRenderer::createBend(const Position* position, const StaffData& staffInfo)
{
    QGraphicsItemGroup* itemGroup = new QGraphicsItemGroup;

    QPainterPath path;

    const double leftX = 0.75 * staffInfo.positionWidth;
    const double rightX = staffInfo.positionWidth;

    for (size_t i = 0; i < position->GetNoteCount(); ++i)
    {
        const Note* note = position->GetNote(i);

        if (note->HasBend())
        {
            const double yBottom = staffInfo.getTabLineHeight(note->GetString()) + 0.5 * Staff::STD_NOTATION_LINE_SPACING;
            const double yTop = staffInfo.getTopTabLine() - Staff::TAB_SYMBOL_HEIGHT * 1.5;

            // draw arc for bend
            // TODO - support drawing different bend types
            path.moveTo(leftX, yBottom);
            path.cubicTo(leftX, yBottom, rightX, yBottom,
                         rightX, yTop);

            // draw arrow head
            const double ARROW_WIDTH = 5;

            QPolygonF arrowShape;
            arrowShape << QPointF(rightX - ARROW_WIDTH / 2, yTop)
                       << QPointF(rightX + ARROW_WIDTH / 2, yTop)
                       << QPointF(rightX, yTop - ARROW_WIDTH);

            QGraphicsPolygonItem* arrow = new QGraphicsPolygonItem(arrowShape);
            arrow->setBrush(QBrush(Qt::black));
            itemGroup->addToGroup(arrow);

            // draw text for bend (e.g. "Full", "3/4", etc)
            uint8_t bendType = 0, bentPitch = 0, releasePitch = 0,
                    duration = 0, drawStartPoint = 0, drawEndPoint = 0;
            note->GetBend(bendType, bentPitch, releasePitch, duration, drawStartPoint, drawEndPoint);

            if (bendType != Note::gradualRelease && bendType != Note::immediateRelease)
            {
                QGraphicsTextItem* bendText = new QGraphicsTextItem(QString::fromStdString(Note::GetBendText(bentPitch)));
                symbolTextFont.setStyle(QFont::StyleNormal);
                bendText->setFont(symbolTextFont);
                centerItem(bendText, leftX, rightX, yTop - 2 * symbolTextFont.pixelSize());
                itemGroup->addToGroup(bendText);
            }
        }
    }

    itemGroup->addToGroup(new QGraphicsPathItem(path));
    return itemGroup;
}

QGraphicsItem* SystemRenderer::operator()(boost::shared_ptr<const System> system)
{
    bool isFirstVisibleStaff = true;

    // Draw each staff
    for (uint32_t i = 0; i < system->GetStaffCount(); i++)
    {
        staff = system->GetStaff(i);
        if (!staff->IsShown())
        {
            continue;
        }

        drawStdNotation(currentStaffInfo);

        drawSlides(currentStaffInfo);
        drawSymbols(currentStaffInfo);
    }

    return parentSystem;
}

#endif
