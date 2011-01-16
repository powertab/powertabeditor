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
    caret->setSystem(doc->GetGuitarScore()->GetSystem(0));
    caret->setStaff(caret->getCurrentSystem()->GetStaff(0));
    caret->setPosition(caret->getCurrentStaff()->GetPosition(0, 0));
    caret->setNote(caret->getCurrentPosition()->GetNote(0));

    caret->updatePosition();

    scene.addItem(caret);
}

void ScoreArea::renderScore(Score* score, int lineSpacing)
{
    // Render each system (group of staves) in the entire score
    for (uint32_t i=0; i < score->GetSystemCount(); i++)
    {
        renderSystem(score->GetSystem(i), lineSpacing);
    }
}

void ScoreArea::renderSystem(System* system, int lineSpacing)
{
    QList<StaffData> staffInformationList;
    QPointF position;

    // save the top-left position of the system
    position.setX(system->GetRect().GetLeft());
    position.setY(system->GetRect().GetTop());

    // draw system rectangle
    QPainterPath path;
    path.addRect(position.x(), position.y(), system->GetRect().GetWidth(), system->GetRect().GetHeight());
    QGraphicsPathItem *rect = new QGraphicsPathItem(path);
    rect->setOpacity(0.5);
    rect->setPen(QPen(QBrush(QColor(0,0,0)), 0.5));
    scene.addItem(rect);

    // Draw each staff
    for (uint32_t i=0; i < system->GetStaffCount(); i++)
    {
        StaffData currentStaffInfo;
        currentStaffInfo.positionWidth = system->GetPositionSpacing();

        Staff* currentStaff = system->GetStaff(i);

        // Draw the staff lines
        renderStaffLines(currentStaff, currentStaffInfo, lineSpacing, system->GetRect().GetWidth(), position);
        position.setY(currentStaffInfo.topEdge + currentStaffInfo.height);

        // Draw the clefs
        ClefPainter* clefPainter = new ClefPainter(currentStaffInfo, currentStaff);
        clefPainter->setPos(currentStaffInfo.leftEdge + system->GetClefPadding(), currentStaffInfo.getTopStdNotationLine());
        scene.addItem(clefPainter);

        drawTabClef(currentStaffInfo.leftEdge + system->GetClefPadding(), currentStaffInfo);

        renderBars(currentStaffInfo, system);

        drawTabNotes(system, currentStaff, currentStaffInfo);

        if (i == 0)
        {
            drawChordText(system, currentStaffInfo);
        }

        drawLegato(system, currentStaff, currentStaffInfo);
        drawSlides(system, currentStaff, currentStaffInfo);

        staffInformationList.insert(i, currentStaffInfo);
    }
}

// Draws the lines for a single staff
void ScoreArea::renderStaffLines(Staff* staff, StaffData& currentStaffInfo, int lineSpacing, int width, QPointF& position)
{
    // Populate the staff info structure with information from the given staff
    currentStaffInfo.leftEdge = position.x();
    currentStaffInfo.numOfStrings = staff->GetTablatureStaffType();
    currentStaffInfo.stdNotationStaffAboveSpacing = staff->GetStandardNotationStaffAboveSpacing();
    currentStaffInfo.stdNotationStaffBelowSpacing = staff->GetStandardNotationStaffBelowSpacing();
    currentStaffInfo.symbolSpacing = staff->GetSymbolSpacing();
    currentStaffInfo.tabLineSpacing = lineSpacing;
    currentStaffInfo.tabStaffBelowSpacing = staff->GetTablatureStaffBelowSpacing();
    currentStaffInfo.topEdge = position.y();
    currentStaffInfo.width = width;

    currentStaffInfo.calculateHeight();

    // Draw music staff lines
    drawStaff(currentStaffInfo.leftEdge, currentStaffInfo.getTopStdNotationLine(),
              currentStaffInfo.stdNotationLineSpacing, currentStaffInfo.width, currentStaffInfo.numOfStdNotationLines);

    // Draw tab staff lines
    drawStaff(currentStaffInfo.leftEdge, currentStaffInfo.getTopTabLine(),
              currentStaffInfo.tabLineSpacing, currentStaffInfo.width, currentStaffInfo.numOfStrings);
}

// Draw all of the barlines for the staff.
void ScoreArea::renderBars(const StaffData& currentStaffInfo, System* system)
{
    BarlinePainter *firstBarLine = new BarlinePainter(currentStaffInfo, &system->GetStartBarRef());
    if (system->GetStartBarConstRef().IsBar()) // for normal bars, display a line at the far left edge
    {
        firstBarLine->setPos(system->GetRect().GetLeft() - firstBarLine->boundingRect().width() / 2 - 0.5,
                             currentStaffInfo.topEdge);
    }
    else // otherwise, display the bar after the clef, etc, and to the left of the first note
    {
        firstBarLine->setPos(system->GetFirstPositionX() - currentStaffInfo.positionWidth, currentStaffInfo.topEdge);
    }
    scene.addItem(firstBarLine);

    // draw key signature
    const KeySignature& firstKeySig = system->GetStartBarConstRef().GetKeySignatureConstRef();
    if (firstKeySig.IsShown())
    {
        KeySignaturePainter *firstKeySigPainter = new KeySignaturePainter(currentStaffInfo, firstKeySig);
        firstKeySigPainter->setPos(system->GetRect().GetLeft() + system->GetClefWidth(), currentStaffInfo.getTopStdNotationLine());
        scene.addItem(firstKeySigPainter);
    }

    // draw time signature
    const TimeSignature& firstTimeSig = system->GetStartBarConstRef().GetTimeSignatureConstRef();
    if (firstTimeSig.IsShown())
    {
        TimeSignaturePainter *firstTimeSigPainter = new TimeSignaturePainter(currentStaffInfo, firstTimeSig);
        firstTimeSigPainter->setPos(system->GetRect().GetLeft() + system->GetClefWidth() + firstKeySig.GetWidth() + 7,
                             currentStaffInfo.getTopStdNotationLine());
        scene.addItem(firstTimeSigPainter);
    }

    for (uint32_t i=0; i < system->GetBarlineCount(); i++)
    {
        Barline *barLine = system->GetBarline(i);
        double x = system->GetPositionX(barLine->GetPosition());
        BarlinePainter *barLinePainter = new BarlinePainter(currentStaffInfo, barLine);
        barLinePainter->setPos(x, currentStaffInfo.topEdge);
        scene.addItem(barLinePainter);

        const KeySignature& keySig = barLine->GetKeySignatureConstRef();
        if (keySig.IsShown())
        {
            KeySignaturePainter* keySigPainter = new KeySignaturePainter(currentStaffInfo, keySig);
            keySigPainter->setPos(x + barLinePainter->boundingRect().width() - 1, currentStaffInfo.getTopStdNotationLine());
            scene.addItem(keySigPainter);
        }

        const TimeSignature& timeSig = barLine->GetTimeSignatureConstRef();
        if (timeSig.IsShown())
        {
            TimeSignaturePainter* timeSigPainter = new TimeSignaturePainter(currentStaffInfo, timeSig);
            timeSigPainter->setPos(x + barLinePainter->boundingRect().width() + keySig.GetWidth() + 2, currentStaffInfo.getTopStdNotationLine());
            scene.addItem(timeSigPainter);
        }
    }

    BarlinePainter *lastBarLine = new BarlinePainter(currentStaffInfo, &system->GetEndBarRef());
    lastBarLine->setPos(system->GetRect().GetRight() - lastBarLine->boundingRect().width() / 2, currentStaffInfo.topEdge);
    if (system->GetEndBarRef().IsRepeatEnd())
    {// bit of a positioning hack
        lastBarLine->setPos(lastBarLine->pos().x() - 6, lastBarLine->pos().y());
    }
    else if (system->GetEndBarRef().IsBar())
    {
        lastBarLine->setPos(lastBarLine->pos().x() + 0.5, lastBarLine->pos().y());
    }
    scene.addItem(lastBarLine);

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

void ScoreArea::drawStaff(int leftEdge, int currentHeight, int lineSpacing, int width, int numberOfLines)
{
    /*// Draw lines on each side of staff
    DrawBarLine(leftEdge, currentHeight, (numberOfLines - 1) * lineSpacing);
    DrawBarLine(leftEdge + width, currentHeight, (numberOfLines - 1) * lineSpacing);*/

    // Draw staff lines
    for (int i=0; i < numberOfLines; i++)
    {
        QPainterPath path;
        path.moveTo(leftEdge, currentHeight);
        path.lineTo(leftEdge + width, currentHeight);

        QGraphicsPathItem *line = new QGraphicsPathItem(path);
        line->setPen(QPen(QBrush(QColor(0,0,0)), 0.75));
        scene.addItem(line);

        if (i < numberOfLines - 1)
        {
            currentHeight += lineSpacing;
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

void ScoreArea::drawChordText(System* system, const StaffData& currentStaffInfo)
{
    for (uint32_t i = 0; i < system->GetChordTextCount(); i++)
    {
        ChordText* chordText = system->GetChordText(i);
        Q_ASSERT(chordText != NULL);

        const quint32 location = system->GetPositionX(chordText->GetPosition());

        ChordTextPainter* chordTextPainter = new ChordTextPainter(chordText);
        centerItem(chordTextPainter, location, location + currentStaffInfo.positionWidth,
                   currentStaffInfo.topEdge + System::CHORD_TEXT_SPACING); // TODO - create an appropriate constant for the height offset
        scene.addItem(chordTextPainter);
    }
}

void ScoreArea::drawTabNotes(System* system, Staff* staff, const StaffData& currentStaffInfo)
{
    Barline* currentBarline = NULL;
    for (uint32_t i=0; i < staff->GetPositionCount(0); i++)
    {
        Position* currentPosition = staff->GetPosition(0, i);
        const quint32 location = system->GetPositionX(currentPosition->GetPosition());
        currentBarline = system->GetPrecedingBarline(currentPosition->GetPosition());

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

        StdNotationPainter* stdNotePainter = new StdNotationPainter(currentStaffInfo, currentPosition, currentGuitar, currentBarline->GetKeySignaturePtr());
        centerItem(stdNotePainter, location, location+currentStaffInfo.positionWidth,
                   currentStaffInfo.getTopStdNotationLine());
        scene.addItem(stdNotePainter);

        for (uint32_t j=0; j < currentPosition->GetNoteCount(); j++)
        {
            Note* note = currentPosition->GetNote(j);

            TabNotePainter* tabNote = new TabNotePainter(note);
            centerItem(tabNote, location, location + currentStaffInfo.positionWidth,
                       currentStaffInfo.getTabLineHeight(note->GetString()) + 13);
            scene.addItem(tabNote);
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
    if (note->GetString() == staff->GetTablatureStaffType() - 1)
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
