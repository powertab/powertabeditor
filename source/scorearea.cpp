#include "scorearea.h"

#include <QGraphicsPathItem>
#include <QDebug>

#include <powertabdocument/powertabdocument.h>
#include <painters/staffdata.h>
#include <painters/barlinepainter.h>
#include <painters/tabnotepainter.h>
#include <painters/caret.h>
#include <painters/keysignaturepainter.h>
#include <painters/timesignaturepainter.h>
#include <painters/clefpainter.h>
#include <painters/stdnotationpainter.h>
#include <painters/chordtextpainter.h>
#include <painters/staffpainter.h>
#include <painters/systempainter.h>
#include <painters/tempomarkerpainter.h>

#include <cmath>

ScoreArea::ScoreArea(QWidget *parent) :
        QGraphicsView(parent)
{
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);

    setScene(&scene);
    setRenderHints(QPainter::HighQualityAntialiasing);

    activeStaff = NULL;
    activeSystem = NULL;
}

void ScoreArea::renderDocument(std::shared_ptr<PowerTabDocument> doc)
{
    scene.clear();
    systemList.clear();
    document = doc;
    int lineSpacing = document->GetTablatureStaffLineSpacing();

    // Render each score
    // Only worry about the guitar score so far
    renderScore(document->GetGuitarScore(), lineSpacing);

    // Set up the caret
    caret = new Caret(doc->GetTablatureStaffLineSpacing());
    connect(caret, SIGNAL(moved()), this, SLOT(adjustScroll()));

    caret->setScore(doc->GetGuitarScore());
    caret->updatePosition();

    scene.addItem(caret);
}

/// Updates the system after changes have been made
/// @param systemIndex The index of the system that was modified
void ScoreArea::updateSystem(const quint32 systemIndex)
{
    // delete and remove the system from the scene
    delete systemList.takeAt(systemIndex);

    // redraw the system
    Score* currentScore = document->GetGuitarScore();
    renderSystem(currentScore, currentScore->GetSystem(systemIndex), document->GetTablatureStaffLineSpacing());
    systemList.insert(systemIndex, activeSystem);

    // Adjust the position of subsequent systems
    for (int i = systemIndex; i < systemList.size(); i++)
    {
        QGraphicsItem* systemPainter = systemList.at(i);
        const Rect currentSystemRect = currentScore->GetSystem(i)->GetRect();
        systemPainter->setPos(currentSystemRect.GetLeft(), currentSystemRect.GetTop());
    }

    caret->updatePosition();
}

void ScoreArea::renderScore(Score* score, int lineSpacing)
{
    // Render each system (group of staves) in the entire score
    for (uint32_t i=0; i < score->GetSystemCount(); i++)
    {
        renderSystem(score, score->GetSystem(i), lineSpacing);
        systemList << activeSystem;
    }
}

void ScoreArea::renderSystem(Score* score, System* system, int lineSpacing)
{
    const Rect systemRectangle = system->GetRect();

    // save the top-left position of the system
    const int leftEdge = systemRectangle.GetLeft();
    const int topEdge = systemRectangle.GetTop();
    const int width = systemRectangle.GetWidth();

    // draw system rectangle
    SystemPainter* sysPainter = new SystemPainter(system);
    sysPainter->setPos(leftEdge, topEdge);
    scene.addItem(sysPainter);
    activeSystem = sysPainter;

    // Draw each staff
    for (uint32_t i=0; i < system->GetStaffCount(); i++)
    {
        StaffData currentStaffInfo;
        currentStaffInfo.positionWidth = system->GetPositionSpacing();

        Staff* currentStaff = system->GetStaff(i);

        // Populate the staff info structure with information from the given staff
        currentStaffInfo.leftEdge = leftEdge;
        currentStaffInfo.numOfStrings = currentStaff->GetTablatureStaffType();
        currentStaffInfo.stdNotationStaffAboveSpacing = currentStaff->GetStandardNotationStaffAboveSpacing();
        currentStaffInfo.stdNotationStaffBelowSpacing = currentStaff->GetStandardNotationStaffBelowSpacing();
        currentStaffInfo.symbolSpacing = currentStaff->GetSymbolSpacing();
        currentStaffInfo.tabLineSpacing = lineSpacing;
        currentStaffInfo.tabStaffBelowSpacing = currentStaff->GetTablatureStaffBelowSpacing();
        currentStaffInfo.topEdge = system->GetStaffHeightOffset(i, true);
        currentStaffInfo.width = width;
        currentStaffInfo.calculateHeight();

        // Draw the staff lines
        StaffPainter* staffPainter = new StaffPainter(system, currentStaff, currentStaffInfo);
        staffPainter->setPos(0, system->GetStaffHeightOffset(i));
        staffPainter->setParentItem(activeSystem);
        activeStaff = staffPainter;

        // Draw the clefs
        ClefPainter* clefPainter = new ClefPainter(currentStaffInfo, currentStaff);
        clefPainter->setPos(system->GetClefPadding(), currentStaffInfo.getTopStdNotationLine(false));
        clefPainter->setParentItem(activeStaff);

        drawTabClef(system->GetClefPadding(), currentStaffInfo);

        renderBars(currentStaffInfo, system);

        drawTabNotes(system, currentStaff, currentStaffInfo);
        drawStdNotation(system, currentStaff, currentStaffInfo);

        if (i == 0)
        {
            drawSystemSymbols(score, system, currentStaffInfo);
        }

        drawLegato(system, currentStaff, currentStaffInfo);
        drawSlides(system, currentStaff, currentStaffInfo);
        drawSymbols(system, currentStaff, currentStaffInfo);
    }
}

// Draw all of the barlines for the staff.
void ScoreArea::renderBars(const StaffData& currentStaffInfo, System* system)
{
    std::vector<Barline*> barlines;
    system->GetBarlines(barlines);

    for (size_t i = 0; i < barlines.size(); i++)
    {
        Barline* currentBarline = barlines.at(i);
        const KeySignature& keySig = currentBarline->GetKeySignatureConstRef();
        const TimeSignature& timeSig = currentBarline->GetTimeSignatureConstRef();

        BarlinePainter* barlinePainter = new BarlinePainter(currentStaffInfo, currentBarline);

        double x = system->GetPositionX(currentBarline->GetPosition());
        double keySigX = x + barlinePainter->boundingRect().width() - 1;
        double timeSigX = x + barlinePainter->boundingRect().width() + keySig.GetWidth();
        double rehearsalSignX = x;

        if (i == 0) // start bar of system
        {
            if (currentBarline->IsBar()) // for normal bars, display a line at the far left edge
            {
                x = 0 - barlinePainter->boundingRect().width() / 2 - 0.5;
            }
            else // otherwise, display the bar after the clef, etc, and to the left of the first note
            {
                x = system->GetFirstPositionX() - currentStaffInfo.positionWidth;
            }

            keySigX = system->GetClefWidth();
            timeSigX = system->GetClefWidth() + keySig.GetWidth();
        }
        else if (i == barlines.size() - 1) // last barline of system
        {
            x = system->GetRect().GetWidth() - barlinePainter->boundingRect().width() / 2 - 1;

            if (currentBarline->IsRepeatEnd())
            {
                x -= 6;     // bit of a positioning hack
            }
            else if (currentBarline->IsBar())
            {
                x += 0.5;
            }
        }

        barlinePainter->setPos(x, 0);
        barlinePainter->setParentItem(activeStaff);

        if (keySig.IsShown())
        {
            KeySignaturePainter* keySigPainter = new KeySignaturePainter(currentStaffInfo, keySig);
            keySigPainter->setPos(keySigX, currentStaffInfo.getTopStdNotationLine(false));
            keySigPainter->setParentItem(activeStaff);
        }

        if (timeSig.IsShown())
        {
            TimeSignaturePainter* timeSigPainter = new TimeSignaturePainter(currentStaffInfo, timeSig);
            timeSigPainter->setPos(timeSigX, currentStaffInfo.getTopStdNotationLine(false));
            timeSigPainter->setParentItem(activeStaff);
        }

        const RehearsalSign& rehearsalSign = currentBarline->GetRehearsalSignConstRef();
        if (rehearsalSign.IsSet())
        {
            const int y = 1;
            QFont displayFont("Helvetica");
            displayFont.setPixelSize(12);

            QGraphicsSimpleTextItem* signLetter = new QGraphicsSimpleTextItem;
            signLetter->setText(QString(QChar(rehearsalSign.GetLetter())));
            signLetter->setPos(rehearsalSignX, y);
            signLetter->setFont(displayFont);

            QGraphicsSimpleTextItem* signText = new QGraphicsSimpleTextItem;
            signText->setText(QString().fromStdString(rehearsalSign.GetDescription()));
            signText->setFont(displayFont);
            signText->setPos(rehearsalSignX + signLetter->boundingRect().width() + 7, y);

            // draw rectangle around rehearsal sign letter
            QRectF boundingRect = signLetter->boundingRect();
            boundingRect.setWidth(boundingRect.width() + 7);
            boundingRect.translate(-4, 0);
            QGraphicsRectItem* rect = new QGraphicsRectItem(boundingRect);
            rect->setPos(rehearsalSignX, y);

            rect->setParentItem(activeSystem);
            signText->setParentItem(activeSystem);
            signLetter->setParentItem(activeSystem);
        }
    }
}

void ScoreArea::drawSlides(System* system, Staff* staff, const StaffData& currentStaffInfo)
{
    const int voice = 0;
    for (int string = 0; string < staff->GetTablatureStaffType(); string++)
    {
        for (uint32_t j = 0; j < staff->GetPositionCount(voice); j++)
        {
            Position* position = staff->GetPosition(voice, j);
            Note* note = position->GetNoteByString(string);

            if (note == NULL)
            {
                continue;
            }

            if (note->HasShiftSlide() || note->HasLegatoSlide())
            {
                if (j == staff->GetPositionCount(voice) - 1)
                {
                    continue;
                }
                Position* nextPosition = staff->GetPosition(voice, j + 1);
                Note* nextNote = nextPosition->GetNoteByString(string);

                const double leftPos = system->GetPositionX(position->GetPosition());
                const double width = system->GetPositionX(nextPosition->GetPosition()) - leftPos;
                double height = 5;
                double y = (currentStaffInfo.getTabLineHeight(string + 1, false) + currentStaffInfo.getTabLineHeight(string, false)) / 2;

                if (nextNote->GetFretNumber() > note->GetFretNumber())
                {
                    height = -height;
                    y += currentStaffInfo.tabLineSpacing + 1;
                }

                QPainterPath path;
                path.lineTo(width - currentStaffInfo.positionWidth / 2, height);

                QGraphicsPathItem* line = new QGraphicsPathItem(path);
                line->setPos(leftPos + system->GetPositionSpacing() / 1.5 + 1, y + height / 2);
                line->setParentItem(activeStaff);

                drawComplexSymbolText(staff, currentStaffInfo, note, leftPos);
            }
        }
    }
}

void ScoreArea::drawLegato(System* system, Staff* staff, const StaffData& currentStaffInfo)
{
    const int voice = 0;
    for (int string = 0; string < staff->GetTablatureStaffType(); string++)
    {
        int startPos = -1;
        for (uint32_t j = 0; j < staff->GetPositionCount(voice); j++)
        {
            Position* position = staff->GetPosition(voice, j);
            Note* note = position->GetNoteByString(string);

            const int currentPosition = position->GetPosition();

            if (note == NULL)
            {
                startPos = -1;
                continue;
            }
            if (note->HasHammerOn() || note->HasPullOff() || note->HasLegatoSlide())
            {
                if (startPos == -1) // set the start position of an arc
                {
                    startPos = currentPosition;
                }

                if (!note->HasLegatoSlide()) // this is done during the drawSlides() function
                {
                    drawComplexSymbolText(staff, currentStaffInfo, note, system->GetPositionX(currentPosition));
                }
            }

            else if (startPos != -1) // if an arc has been started, and the current note is not a hammer-on/pull-off, end the arc
            {
                const double leftPos = system->GetPositionX(startPos);
                const double width = system->GetPositionX(currentPosition) - leftPos;
                double height = 7.5;
                double y = currentStaffInfo.getTabLineHeight(string, false) - 2;

                if (string >= currentStaffInfo.numOfStrings / 2)
                {
                    // for notes on the bottom half of the staff, flip the arc and place it below the notes
                    y += 2 * currentStaffInfo.tabLineSpacing + height / 2.5;
                    height = -height;
                }

                QPainterPath path;
                path.moveTo(width, height / 2);
                path.arcTo(0, 0, width, height, 0, 180);

                QGraphicsPathItem *arc = new QGraphicsPathItem(path);
                arc->setPos(leftPos + system->GetPositionSpacing() / 2, y);
                arc->setParentItem(activeStaff);

                startPos = -1;
            }

            if (note->HasHammerOnFromNowhere() || note->HasPullOffToNowhere())
            {
                drawComplexSymbolText(staff, currentStaffInfo, note, system->GetPositionX(currentPosition));

                const double height = 10;
                const double width = 6;
                QPainterPath path;
                path.moveTo(width, height / 2);
                path.arcTo(0, 0, width, height, 0, 180);
                QGraphicsPathItem *arc = new QGraphicsPathItem(path);
                arc->setPos(system->GetPositionX(currentPosition) + 2,
                            currentStaffInfo.getTabLineHeight(string, false) - 2);
                arc->setParentItem(activeStaff);

                startPos = -1;
            }
        }
    }
}

void ScoreArea::drawTabClef(int x, const StaffData& staffInfo)
{
    // Draw the tab clef
    QGraphicsSimpleTextItem* tabClef = new QGraphicsSimpleTextItem;
    tabClef->setPos(x, staffInfo.getTopTabLine(false) - 12);
    musicFont.setSymbol(tabClef, MusicFont::TabClef, staffInfo.numOfStrings * 4.25);
    tabClef->setParentItem(activeStaff);
}

void ScoreArea::drawDividerLine(const StaffData& currentStaffInfo, quint32 y)
{
    QGraphicsLineItem* line = new QGraphicsLineItem;
    line->setLine(0, y, currentStaffInfo.width, y);
    line->setOpacity(0.5);
    line->setPen(QPen(Qt::black, 0.5, Qt::DashLine));

    line->setParentItem(activeSystem);
}

void ScoreArea::drawSystemSymbols(Score* score, System* system, const StaffData& currentStaffInfo)
{
    quint32 height = 0;

    if (system->HasRehearsalSign()) // rehearsal signs are drawn at the same time as barlines
    {
        height += System::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(currentStaffInfo, height);
    }

    std::vector<TempoMarker*> tempoMarkers;
    score->GetTempoMarkersInSystem(tempoMarkers, system);

    if (tempoMarkers.size() > 0)
    {
        drawTempoMarkers(tempoMarkers, system, height);
        height += System::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(currentStaffInfo, height);
    }

    if (system->GetChordTextCount() > 0)
    {
        drawChordText(system, height, currentStaffInfo);
        height += System::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(currentStaffInfo, height);
    }

}

void ScoreArea::drawTempoMarkers(std::vector<TempoMarker*> tempoMarkers, System* system, quint32 height)
{
    foreach(TempoMarker* tempoMarker, tempoMarkers)
    {
        const quint32 location = system->GetPositionX(tempoMarker->GetPosition());

        TempoMarkerPainter* tempoMarkerPainter = new TempoMarkerPainter(tempoMarker);
        tempoMarkerPainter->setPos(location, height + 4);
        tempoMarkerPainter->setParentItem(activeSystem);
    }
}

void ScoreArea::drawChordText(System* system, quint32 height, const StaffData& currentStaffInfo)
{
    for (uint32_t i = 0; i < system->GetChordTextCount(); i++)
    {
        ChordText* chordText = system->GetChordText(i);
        Q_ASSERT(chordText != NULL);

        const quint32 location = system->GetPositionX(chordText->GetPosition());

        ChordTextPainter* chordTextPainter = new ChordTextPainter(chordText);
        centerItem(chordTextPainter, location, location + currentStaffInfo.positionWidth, height + 4);
        chordTextPainter->setParentItem(activeSystem);
    }
}

void ScoreArea::drawTabNotes(System* system, Staff* staff, const StaffData& currentStaffInfo)
{
    for (uint32_t i=0; i < staff->GetPositionCount(0); i++)
    {
        Position* currentPosition = staff->GetPosition(0, i);
        const quint32 location = system->GetPositionX(currentPosition->GetPosition());

        for (uint32_t j=0; j < currentPosition->GetNoteCount(); j++)
        {
            Note* note = currentPosition->GetNote(j);

            TabNotePainter* tabNote = new TabNotePainter(note);
            centerItem(tabNote, location, location + currentStaffInfo.positionWidth,
                       currentStaffInfo.getTabLineHeight(note->GetString(), false) + currentStaffInfo.tabLineSpacing / 2 - 1);
            tabNote->setParentItem(activeStaff);
        }
    }
}

void ScoreArea::drawStdNotation(System* system, Staff* staff, const StaffData& currentStaffInfo)
{
    Barline* currentBarline = NULL;
    Barline* prevBarline = system->GetStartBarPtr();

    QList<StdNotationPainter*> notePainters;
    QMultiMap<double, StdNotationPainter*> accidentalsMap;
    QList<BeamingInfo> beamings;

    for (uint32_t i=0; i < staff->GetPositionCount(0); i++)
    {
        Position* currentPosition = staff->GetPosition(0, i);
        const quint32 location = system->GetPositionX(currentPosition->GetPosition());
        currentBarline = system->GetPrecedingBarline(currentPosition->GetPosition());

        // if we reach a new bar, we can adjust all of the accidentals for the previous bar
        if (currentBarline != prevBarline)
        {
            adjustAccidentals(accidentalsMap);
            accidentalsMap.clear();
            prevBarline = currentBarline;
        }

        KeySignature* currentKeySig = currentBarline->GetKeySignaturePtr();

        // Find the guitar corresponding to the current staff
        Guitar* currentGuitar = NULL;
        for (uint32_t j = 0; j < system->GetStaffCount(); j++)
        {
            if (system->GetStaff(j) == staff)
            {
                currentGuitar = document->GetGuitarScore()->GetGuitar(j);
            }
        }

        Q_ASSERT(currentGuitar != NULL);

        // just draw rests right away, since we don't have to worry about beaming or accidentals
        if (currentPosition->IsRest())
        {
            StdNotationPainter* stdNotePainter = new StdNotationPainter(currentStaffInfo, currentPosition, NULL, currentGuitar->GetTuningPtr(), currentKeySig);
            centerItem(stdNotePainter, location, location+currentStaffInfo.positionWidth * 1.25,
                       currentStaffInfo.getTopStdNotationLine(false));
            stdNotePainter->setParentItem(activeStaff);
            continue;
        }

        BeamingInfo beamingInfo;
        beamingInfo.position = currentPosition;

        for (uint32_t j=0; j < currentPosition->GetNoteCount(); j++)
        {
            Note* note = currentPosition->GetNote(j);

            StdNotationPainter* stdNotePainter = new StdNotationPainter(currentStaffInfo, currentPosition,
                                                                        note, currentGuitar->GetTuningPtr(), currentKeySig);
            notePainters << stdNotePainter;

            // map all of the notes for each position on the staff, so that we can adjust accidentals later
            accidentalsMap.insert(stdNotePainter->getYLocation(), stdNotePainter);

            beamingInfo.bottomNotePos = std::max(beamingInfo.bottomNotePos, stdNotePainter->getYLocation());
            beamingInfo.topNotePos = std::min(beamingInfo.topNotePos, stdNotePainter->getYLocation());
        }

        beamingInfo.topNotePos += currentStaffInfo.getTopStdNotationLine(false);
        beamingInfo.bottomNotePos += currentStaffInfo.getTopStdNotationLine(false);
        beamingInfo.beamUp = currentStaffInfo.getStdNotationLineHeight(3, false) < beamingInfo.bottomNotePos;
        beamingInfo.location = location;

        if (currentPosition->GetDurationType() != 1)
        {
            beamings << beamingInfo;
        }
    }

    // after adjusting accidentals, etc, we can add the painters to the scene
    foreach(StdNotationPainter* painter, notePainters)
    {
        const quint32 location = system->GetPositionX(painter->getPositionObject()->GetPosition());
        painter->setPos(location, currentStaffInfo.getTopStdNotationLine(false));
        painter->setParentItem(activeStaff);
    }

    QList<BeamingInfo> beamingGroup;
    const double beamLength = currentStaffInfo.stdNotationLineSpacing * 3.5;

    // draw beams
    for (int i = 0; i < beamings.size(); i++)
    {
        BeamingInfo beaming = beamings[i];

        beamingGroup << beaming;

        // if the position is staccato, draw the dot near either the top or bottom note of the position, depending on beam direction
        if (beaming.position->IsStaccato())
        {
            const double y = beaming.beamUp ? beaming.bottomNotePos : beaming.topNotePos - 15;

            QGraphicsTextItem* dot = new QGraphicsTextItem(QChar(MusicFont::Dot));
            dot->setFont(musicFont.getFontRef());
            centerItem(dot, beaming.location, beaming.location + currentStaffInfo.positionWidth + 2, y - 25);
            dot->setParentItem(activeStaff);
        }

        // if we're at the end of a beam group, or have a note by itself (not part of a group), draw the beams
        if (beaming.position->IsBeamEnd() || (!beaming.position->IsBeamStart() && beamingGroup.size() == 1))
        {
            // figure out which direction all of the beams in the group should point, based on how many point upwards/downwards
            uint8_t notesAboveMiddle = 0, notesBelowMiddle = 0;
            foreach(BeamingInfo beam, beamingGroup)
            {
                if (beam.beamUp)
                {
                    notesBelowMiddle++;
                }
                else
                {
                    notesAboveMiddle++;
                }
            }

            bool beamDirectionUp = notesBelowMiddle >= notesAboveMiddle;

            // find the highest/lowest position of a beam within the group - all other beams will stretch to this position
            double highestPos = 10000000, lowestPos = -10000000;
            if (beamDirectionUp)
            {
                foreach(BeamingInfo beam, beamingGroup)
                {
                    highestPos = std::min(highestPos, beam.topNotePos);
                }
            }
            else
            {
                foreach(BeamingInfo beam, beamingGroup)
                {
                    lowestPos = std::max(lowestPos, beam.bottomNotePos);
                }
            }

            // set the position of each beam, and draw them
            for (int j = 0; j < beamingGroup.size(); j++)
            {
                beamingGroup[j].beamUp = beamDirectionUp;
                if (beamDirectionUp)
                {
                    beamingGroup[j].location += currentStaffInfo.getNoteHeadRightEdge() - 1;
                    beamingGroup[j].topNotePos = highestPos - beamLength;
                }
                else
                {
                    beamingGroup[j].bottomNotePos = lowestPos + beamLength;
                    beamingGroup[j].location += currentStaffInfo.getNoteHeadRightEdge() - StdNotationPainter::getNoteHeadWidth();
                    if (beamingGroup[j].position->GetDurationType() == 2) // visual adjustment for half notes
                    {
                        beamingGroup[j].location -= 2;
                    }
                }

                {
                    QGraphicsLineItem* line = new QGraphicsLineItem;
                    line->setLine(beamingGroup[j].location, beamingGroup[j].topNotePos, beamingGroup[j].location, beamingGroup[j].bottomNotePos);
                    line->setParentItem(activeStaff);
                }

                // extra beams (for 16th notes, etc)
                // 16th note gets 1 extra beam, 32nd gets two, etc
                // Calculate log_2 of the note duration, and subtract three (so log_2(16) - 3 = 1)
                const int extraBeams = log(beamingGroup[j].position->GetPreviousBeamDurationType()) / log(2) - 3;

                for (int k = 1; k <= extraBeams; k++)
                {
                    double y = beamDirectionUp ? beamingGroup[j].topNotePos : beamingGroup[j].bottomNotePos;
                    y += k * 3 * (beamDirectionUp ? 1 : -1);

                    QGraphicsLineItem* line = new QGraphicsLineItem;
                    line->setLine(beamingGroup[j-1].location + 1, y, beamingGroup[j].location - 1, y);
                    line->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
                    line->setParentItem(activeStaff);
                }

            }

            const double beamConnectorHeight = beamDirectionUp ? beamingGroup.first().topNotePos :
                                               beamingGroup.first().bottomNotePos;

            if (beamingGroup.size() > 1)
            {
                // draw the line that connects all of the beams
                QGraphicsLineItem* connector = new QGraphicsLineItem;

                connector->setLine(beamingGroup.first().location + 1, beamConnectorHeight,
                                   beamingGroup.last().location - 1, beamConnectorHeight);
                connector->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
                connector->setParentItem(activeStaff);
            }

            // Draw fermatas if necessary
            foreach(BeamingInfo beam, beamingGroup)
            {
                if (beam.position->HasFermata())
                {
                    drawFermata(currentStaffInfo, beam.location, beamConnectorHeight, beamDirectionUp);
                }
            }

            beamingGroup.clear();
        }
    }
}

void ScoreArea::drawFermata(const StaffData& currentStaffInfo, double x, double beamConnectorHeight, bool beamDirectionUp)
{
    double y = 0;
    // position the fermata directly above/below the staff if possible, unless the note beaming extends
    // beyond the std. notation staff.
    // After positioning, offset the height due to the way that QGraphicsTextItem positions text
    if (beamDirectionUp)
    {
        y = std::min<double>(beamConnectorHeight, currentStaffInfo.getTopStdNotationLine(false));
        y -= 33;
    }
    else
    {
        y = std::max<double>(beamConnectorHeight, currentStaffInfo.getBottomStdNotationLine(false));
        y -= 25;
    }

    const QChar symbol = beamDirectionUp ? MusicFont::FermataUp : MusicFont::FermataDown;
    QGraphicsSimpleTextItem* fermata = new QGraphicsSimpleTextItem(symbol);
    fermata->setFont(musicFont.getFontRef());
    fermata->setPos(x, y);
    fermata->setParentItem(activeStaff);
}

/// Draws the symbols that appear between the tab and standard notation staves
void ScoreArea::drawSymbols(System *system, Staff *staff, const StaffData &currentStaffInfo)
{
    typedef QGraphicsItem* (ScoreArea::*SymbolCreator)(uint8_t, const StaffData&);

    std::list<SymbolInfo> symbols; // holds all of the symbols that we will draw

    std::vector<Staff::PositionProperty> groupableSymbolPredicates = {
        &Position::HasLetRing, &Position::HasVibrato, &Position::HasWideVibrato,
        &Position::HasPalmMuting, &Position::HasNoteWithNaturalHarmonic,
        &Position::HasNoteWithArtificialHarmonic
    };

    // function pointers for drawing symbols corresponding to each of the items in the groupableSymbolPredicates list
    std::vector<SymbolCreator> groupableSymbolCreators = {
        &ScoreArea::createLetRing, &ScoreArea::createVibrato, &ScoreArea::createWideVibrato,
        &ScoreArea::createPalmMute, &ScoreArea::createNaturalHarmonic,
        &ScoreArea::createArtificialHarmonic
    };

    // Generates the bounding rectangles for each symbol group (i.e. consecutive 'let ring' symbols)
    // - each rectangle has equal height, and a fixed x-coordinate (the position that a symbol group starts at)
    // - the rectangle's width represents the number of positions that the symbol group spans
    // - we later adjust the rectangles' y-coordinates to arrange the symbols without overlap
    for (auto predicate = groupableSymbolPredicates.begin(); predicate != groupableSymbolPredicates.end(); ++predicate)
    {
        bool inGroup = false;
        SymbolInfo currentSymbolInfo;

        for (size_t i = 0; i < staff->GetPositionCount(0); i++)
        {
            Position* currentPosition = staff->GetPosition(0, i);

            const bool propertySet = (currentPosition->**predicate)();

            if (propertySet)
            {
                if (inGroup)
                {
                    currentSymbolInfo.rect.setRight(currentPosition->GetPosition());
                }
                else
                {
                    currentSymbolInfo.rect.setRect(currentPosition->GetPosition(), 0, 1, 1);
                    inGroup = true;
                }
            }
            else if (inGroup)
            {
                // close the rectangle and render the symbol

                currentSymbolInfo.rect.setRight(currentPosition->GetPosition() - 1);
                inGroup = false;
                SymbolCreator symbolCreator = groupableSymbolCreators.at(predicate - groupableSymbolPredicates.begin());
                currentSymbolInfo.symbol = (this->*symbolCreator)(currentSymbolInfo.rect.width(), currentStaffInfo);
                symbols.push_back(currentSymbolInfo);
            }
        }
    }

    std::vector<Staff::PositionProperty> singleSymbolPredicates = {
        &Position::HasVolumeSwell, &Position::HasTremoloPicking,
        &Position::HasTremoloBar, &Position::HasNoteWithTrill
    };
    // function pointers for drawing symbols corresponding to each of the items in the singleSymbolPredicates list
    std::vector<SymbolCreator> singleSymbolCreators = {
        &ScoreArea::createPalmMute, &ScoreArea::createPalmMute,
        &ScoreArea::createPalmMute, &ScoreArea::createTrill
    };

    // Now, generate the rectangles for individual (non-groupable) symbols
    for (auto predicate = singleSymbolPredicates.begin(); predicate != singleSymbolPredicates.end(); ++predicate)
    {
        SymbolInfo currentSymbolInfo;

        for (size_t i = 0; i < staff->GetPositionCount(0); i++)
        {
            Position* currentPosition = staff->GetPosition(0, i);

            const bool propertySet = (currentPosition->**predicate)();

            if (propertySet)
            {
                // create the rectangle and render the symbol
                currentSymbolInfo.rect.setRect(currentPosition->GetPosition(), 0, 1, 1);

                SymbolCreator symbolCreator = singleSymbolCreators.at(predicate - singleSymbolPredicates.begin());
                currentSymbolInfo.symbol = (this->*symbolCreator)(currentSymbolInfo.rect.width(), currentStaffInfo);
                symbols.push_back(currentSymbolInfo);
            }
        }
    }

    // stores the maximum height that has been occupied at each position
    // this is used to figure out how low we can place a symbol group
    std::vector<uint8_t> heightMap(system->CalculatePositionCount(currentStaffInfo.positionWidth), 0);

    for (auto i = symbols.begin(); i != symbols.end(); i++)
    {
        // find the height to draw the symbol group at
        const int left = i->rect.left();
        const int right = i->rect.right();

        // find the lowest height we can place a rectangle at without overlapping the already-placed rectangles
        const int height = *std::max_element(heightMap.begin() + left, heightMap.begin() + right) + 1;
        // update the height map and adjust the current symbol group's height
        std::fill_n(heightMap.begin() + left, i->rect.width(), height);
        i->rect.moveTop(height);

        // draw the symbol group
        i->symbol->setPos(system->GetPositionX(i->rect.left()),
                          currentStaffInfo.getTopTabLine(false) - (i->rect.y() + 1) * Staff::TAB_SYMBOL_HEIGHT);
        i->symbol->setParentItem(activeStaff);
    }
}

// Draws symbols that are grouped across multiple positions (i.e. consecutive "let ring" symbols)
QGraphicsItem* ScoreArea::createConnectedSymbolGroup(const QString& text, uint8_t width,
                                                     const StaffData& currentStaffInfo, QFont::Style style)
{
    static QFont font("Liberation Sans");
    font.setPixelSize(9);
    font.setStyle(style);

    // Render the description (i.e. "let ring")
    QGraphicsSimpleTextItem* description = new QGraphicsSimpleTextItem;
    description->setText(text);
    description->setFont(font);

    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(description);

    // Draw dashed line across the remaining positions in the group
    if (width > 1)
    {
        const double rightEdge = width * currentStaffInfo.positionWidth - 0.5 * currentStaffInfo.positionWidth;
        const double leftEdge = description->boundingRect().right();
        const double middleHeight = Staff::TAB_SYMBOL_HEIGHT / 2.0;
        QGraphicsLineItem* line = new QGraphicsLineItem(leftEdge, middleHeight, rightEdge, middleHeight);
        line->setPen(QPen(Qt::black, 1, Qt::DashLine));

        group->addToGroup(line);

        // Draw a vertical line at the end of the dotted lines
        QGraphicsLineItem* lineEnd = new QGraphicsLineItem(line->boundingRect().right(), 1,
                                                           line->boundingRect().right(), Staff::TAB_SYMBOL_HEIGHT - 1);
        group->addToGroup(lineEnd);
    }

    return group;
}

QGraphicsItem* ScoreArea::createPalmMute(uint8_t width, const StaffData &currentStaffInfo)
{
    return createConnectedSymbolGroup("P.M.", width, currentStaffInfo);
}

QGraphicsItem* ScoreArea::createLetRing(uint8_t width, const StaffData &currentStaffInfo)
{
    return createConnectedSymbolGroup("let ring", width, currentStaffInfo, QFont::StyleItalic);
}

QGraphicsItem* ScoreArea::createNaturalHarmonic(uint8_t width, const StaffData &currentStaffInfo)
{
    return createConnectedSymbolGroup("N.H.", width, currentStaffInfo);
}

QGraphicsItem* ScoreArea::createArtificialHarmonic(uint8_t width, const StaffData &currentStaffInfo)
{
    return createConnectedSymbolGroup("A.H.", width, currentStaffInfo);
}

QGraphicsItem* ScoreArea::drawContinuousFontSymbols(QChar symbol, uint8_t width, const StaffData& currentStaffInfo)
{
    QFont font = musicFont.getFont();
    font.setPixelSize(25);

    const double symbolWidth = QFontMetricsF(font).width(symbol);
    const int numSymbols = width * currentStaffInfo.positionWidth / symbolWidth;
    QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(QString(numSymbols, symbol));
    text->setFont(font);
    text->setPos(0, -25);

    // a bit of a hack for getting around the height offset caused by the music font
    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(text);

    return group;
}

QGraphicsItem* ScoreArea::createVibrato(uint8_t width, const StaffData& currentStaffInfo)
{
    return drawContinuousFontSymbols(MusicFont::getSymbol(MusicFont::Vibrato), width, currentStaffInfo);
}

QGraphicsItem* ScoreArea::createWideVibrato(uint8_t width, const StaffData& currentStaffInfo)
{
    return drawContinuousFontSymbols(MusicFont::getSymbol(MusicFont::WideVibrato), width, currentStaffInfo);
}

QGraphicsItem* ScoreArea::createTrill(uint8_t width, const StaffData& currentStaffInfo)
{
    Q_UNUSED(width);

    QFont font(musicFont.getFont());
    font.setPixelSize(21);

    QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(MusicFont::getSymbol(MusicFont::Trill));
    text->setFont(font);
    centerItem(text, 0, currentStaffInfo.positionWidth, -18);

    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(text);

    return group;
}

// Centers an item, by using it's width to calculate the necessary offset from xmin
void ScoreArea::centerItem(QGraphicsItem* item, float xmin, float xmax, float y)
{
    float itemWidth = item->boundingRect().width();
    float centredX = xmin + ((xmax - (xmin + itemWidth)) / 2);
    item->setPos(centredX, y);
}

// ensures that the caret is visible when it changes sections
void ScoreArea::adjustScroll()
{
    ensureVisible(caret, 50, 100);
}

void ScoreArea::drawComplexSymbolText(Staff* staff, const StaffData& currentStaffInfo, Note* note, const int x)
{
    QString text;

    QFont displayFont("Liberation Sans");
    displayFont.setPixelSize(10);

    int y = currentStaffInfo.getBottomTabLine(false) + 2;

    // for hammerons and pulloffs, we need to push the text further down to avoid overlap with the arc
    if ((int)note->GetString() == (int)staff->GetTablatureStaffType() - 1)
    {
        y += 8;
    }

    // draw the appropriate text below the staff (i.e. 'H', 'P', etc)
    if (note->HasHammerOn() || note->HasHammerOnFromNowhere())
    {
        text = "H";
    }
    else if (note->HasPullOff() || note->HasPullOffToNowhere())
    {
        text = "P";
    }
    else if (note->HasSlide())
    {
        text = "sl.";
        displayFont.setItalic(true);
    }

    QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(text);
    textItem->setFont(displayFont);

    // for these items, put the text directly under the note
    if (note->HasHammerOnFromNowhere() || note->HasPullOffToNowhere())
    {
        centerItem(textItem, x, x + currentStaffInfo.positionWidth, y);
    }
    // for these, put the text between this note and the next one
    else
    {
        centerItem(textItem, x, x + 2 * currentStaffInfo.positionWidth, y);
    }

    textItem->setParentItem(activeStaff);
}

void ScoreArea::adjustAccidentals(QMultiMap<double, StdNotationPainter*>& accidentalsMap)
{
    QList<double> keys = accidentalsMap.uniqueKeys();
    auto i = keys.begin();

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
                    note->accidental = StdNotationPainter::NATURAL;
                }
            }
        }

        ++i;
    }
}
