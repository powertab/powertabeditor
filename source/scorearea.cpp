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

}

void ScoreArea::renderDocument()
{
    renderDocument(document);
}

void ScoreArea::renderDocument(PowerTabDocument *doc)
{
    scene.clear();
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

void ScoreArea::renderScore(Score* score, int lineSpacing)
{
    // Render each system (group of staves) in the entire score
    for (uint32_t i=0; i < score->GetSystemCount(); i++)
    {
        renderSystem(score, score->GetSystem(i), lineSpacing);
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
        staffPainter->setPos(currentStaffInfo.leftEdge, system->GetStaffHeightOffset(i, true));
        scene.addItem(staffPainter);

        // Draw the clefs
        ClefPainter* clefPainter = new ClefPainter(currentStaffInfo, currentStaff);
        clefPainter->setPos(currentStaffInfo.leftEdge + system->GetClefPadding(), currentStaffInfo.getTopStdNotationLine());
        scene.addItem(clefPainter);

        drawTabClef(currentStaffInfo.leftEdge + system->GetClefPadding(), currentStaffInfo);

        renderBars(currentStaffInfo, system);

        drawTabNotes(system, currentStaff, currentStaffInfo);
        drawStdNotation(system, currentStaff, currentStaffInfo);

        if (i == 0)
        {
            drawSystemSymbols(score, system, currentStaffInfo);
        }

        drawLegato(system, currentStaff, currentStaffInfo);
        drawSlides(system, currentStaff, currentStaffInfo);
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
        double timeSigX = x + barlinePainter->boundingRect().width() + keySig.GetWidth() + 2;
        double rehearsalSignX = x;

        if (i == 0) // start bar of system
        {
            if (currentBarline->IsBar()) // for normal bars, display a line at the far left edge
            {
                x = currentStaffInfo.leftEdge - barlinePainter->boundingRect().width() / 2 - 0.5;
            }
            else // otherwise, display the bar after the clef, etc, and to the left of the first note
            {
                x = system->GetFirstPositionX() - currentStaffInfo.positionWidth;
            }

            keySigX = currentStaffInfo.leftEdge + system->GetClefWidth();
            timeSigX = currentStaffInfo.leftEdge + system->GetClefWidth() + keySig.GetWidth() + 7;
        }
        else if (i == barlines.size() - 1) // last barline of system
        {
            x = system->GetRect().GetRight() - barlinePainter->boundingRect().width() / 2;

            if (currentBarline->IsRepeatEnd())
            {
                x -= 6;     // bit of a positioning hack
            }
            else if (currentBarline->IsBar())
            {
                x += 0.5;
            }
        }

        barlinePainter->setPos(x, currentStaffInfo.topEdge);
        scene.addItem(barlinePainter);

        if (keySig.IsShown())
        {
            KeySignaturePainter* keySigPainter = new KeySignaturePainter(currentStaffInfo, keySig);
            keySigPainter->setPos(keySigX, currentStaffInfo.getTopStdNotationLine());
            scene.addItem(keySigPainter);
        }

        if (timeSig.IsShown())
        {
            TimeSignaturePainter* timeSigPainter = new TimeSignaturePainter(currentStaffInfo, timeSig);
            timeSigPainter->setPos(timeSigX, currentStaffInfo.getTopStdNotationLine());
            scene.addItem(timeSigPainter);
        }

        const RehearsalSign& rehearsalSign = currentBarline->GetRehearsalSignConstRef();
        if (rehearsalSign.IsSet())
        {
            const int y = system->GetRect().GetTop() + 1;
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
            scene.addItem(rect);

            scene.addItem(signText);
            scene.addItem(signLetter);
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

            if ((note->HasShiftSlide() || note->HasLegatoSlide()) && !note->HasSlideOutOf())
            {
                Position* nextPosition = staff->GetPosition(voice, j + 1);
                Note* nextNote = nextPosition->GetNoteByString(string);

                const double leftPos = system->GetPositionX(position->GetPosition());
                const double width = system->GetPositionX(nextPosition->GetPosition()) - leftPos;
                double height = 5;
                double y = (currentStaffInfo.getTabLineHeight(string + 1, true) + currentStaffInfo.getTabLineHeight(string, true)) / 2;

                if (nextNote->GetFretNumber() > note->GetFretNumber())
                {
                    height = -height;
                    y += currentStaffInfo.tabLineSpacing + 1;
                }

                QPainterPath path;
                path.lineTo(width - currentStaffInfo.positionWidth / 2, height);

                QGraphicsPathItem *line = new QGraphicsPathItem(path);
                line->setPos(leftPos + system->GetPositionSpacing() / 1.5 + 1, y + height / 2);
                scene.addItem(line);

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
                double y = currentStaffInfo.getTabLineHeight(string, true) - 2;

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
                scene.addItem(arc);

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
                arc->setPos(system->GetPositionX(currentPosition) + 2, currentStaffInfo.getTabLineHeight(string, true) - 2);
                scene.addItem(arc);

                startPos = -1;
            }
        }
    }
}

void ScoreArea::drawTabClef(int x, const StaffData& staffInfo)
{
    // Draw the tab clef
    QGraphicsSimpleTextItem* tabClef = new QGraphicsSimpleTextItem;
    tabClef->setPos(x, staffInfo.getTopTabLine() - 12);
    musicFont.setSymbol(tabClef, MusicFont::TabClef, staffInfo.numOfStrings * 4.25);
    scene.addItem(tabClef);
}

void ScoreArea::drawDividerLine(const StaffData& currentStaffInfo, quint32 height)
{
    QGraphicsLineItem* line = new QGraphicsLineItem;
    line->setLine(currentStaffInfo.leftEdge, height, currentStaffInfo.leftEdge + currentStaffInfo.width, height);
    line->setOpacity(0.5);
    line->setPen(QPen(Qt::black, 0.5, Qt::DashLine));
    scene.addItem(line);
}

void ScoreArea::drawSystemSymbols(Score* score, System* system, const StaffData& currentStaffInfo)
{
    quint32 height = system->GetRect().GetTop();

    if (system->HasRehearsalSign()) // rehearsal signs are drawn at the same time as barlines
    {
        height += System::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(currentStaffInfo, height);
    }

    std::vector<TempoMarker*> tempoMarkers;
    score->GetTempoMarkersInSystem(tempoMarkers, system);

    if (tempoMarkers.size() > 0)
    {
        drawTempoMarkers(tempoMarkers, system, height, currentStaffInfo);
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

void ScoreArea::drawTempoMarkers(std::vector<TempoMarker*> tempoMarkers, System* system, quint32 height, const StaffData& currentStaffInfo)
{
    foreach(TempoMarker* tempoMarker, tempoMarkers)
    {
        const quint32 location = system->GetPositionX(tempoMarker->GetPosition());

        TempoMarkerPainter* tempoMarkerPainter = new TempoMarkerPainter(tempoMarker);
        tempoMarkerPainter->setPos(location, height + 4);
        scene.addItem(tempoMarkerPainter);
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
        scene.addItem(chordTextPainter);
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
                       currentStaffInfo.getTabLineHeight(note->GetString()) + currentStaffInfo.tabLineSpacing / 2 - 1);
            scene.addItem(tabNote);
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
                       currentStaffInfo.getTopStdNotationLine());
            scene.addItem(stdNotePainter);
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

        beamingInfo.topNotePos += currentStaffInfo.getTopStdNotationLine();
        beamingInfo.bottomNotePos += currentStaffInfo.getTopStdNotationLine();
        beamingInfo.beamUp = currentStaffInfo.getStdNotationLineHeight(3) < beamingInfo.bottomNotePos;
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
        painter->setPos(location, currentStaffInfo.getTopStdNotationLine());
        scene.addItem(painter);
    }

    QList<BeamingInfo> beamingGroup;
    const double beamLength = currentStaffInfo.stdNotationLineSpacing * 3.5;

    // draw beams
    for (int i = 0; i < beamings.size(); i++)
    {
        BeamingInfo beaming = beamings[i];

        beamingGroup << beaming;

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
                    scene.addItem(line);
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
                    scene.addItem(line);
                }

            }

            if (beamingGroup.size() > 1)
            {
                // draw the line that connects all of the beams
                QGraphicsLineItem* connector = new QGraphicsLineItem;

                double height = beamDirectionUp ? beamingGroup.first().topNotePos : beamingGroup.first().bottomNotePos;
                connector->setLine(beamingGroup.first().location + 1, height, beamingGroup.last().location - 1, height);
                connector->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
                scene.addItem(connector);
            }

            beamingGroup.clear();
        }
    }
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

    int y = currentStaffInfo.getBottomTabLine() + 2;

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
    scene.addItem(textItem);
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
