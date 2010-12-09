#include "scorearea.h"

#include <QGraphicsPathItem>
#include <QDebug>

#include "painters/staffdata.h"
#include "painters/barlinepainter.h"
#include "painters/tabnotepainter.h"
#include "powertabdocument/powertabdocument.h"
#include "painters/caret.h"
#include "painters/keysignaturepainter.h"
#include "painters/timesignaturepainter.h"
#include "painters/clefpainter.h"

ScoreArea::ScoreArea(QWidget *parent) :
        QGraphicsView(parent)
{
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);

    setScene(&scene);
    setRenderHints(QPainter::HighQualityAntialiasing);

}

void ScoreArea::RenderDocument()
{
    RenderDocument(document);
}

void ScoreArea::RenderDocument(PowerTabDocument *doc)
{
    scene.clear();
    document = doc;
    int lineSpacing = document->GetTablatureStaffLineSpacing();

    // Render each score
    // Only worry about the guitar score so far
    RenderScore(document->GetGuitarScore(), lineSpacing);

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

void ScoreArea::RenderScore(Score* score, int lineSpacing)
{
    // Render each system (group of staves) in the entire score
    for (uint32_t i=0; i < score->GetSystemCount(); i++)
    {
        RenderSystem(score->GetSystem(i), lineSpacing);
    }
}

void ScoreArea::RenderSystem(System* system, int lineSpacing)
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
        RenderStaffLines(currentStaff, currentStaffInfo, lineSpacing, system->GetRect().GetWidth(), position);
        position.setY(currentStaffInfo.topEdge + currentStaffInfo.height);

        // Draw the clefs
        ClefPainter* clefPainter = new ClefPainter(currentStaffInfo, currentStaff);
        clefPainter->setPos(currentStaffInfo.leftEdge + system->GetClefPadding(), currentStaffInfo.getTopStdNotationLine());
        scene.addItem(clefPainter);

        DrawTabClef(currentStaffInfo.leftEdge + system->GetClefPadding(), currentStaffInfo);

        RenderBars(currentStaffInfo, system);

        DrawTabNotes(system, currentStaff, currentStaffInfo);

        staffInformationList.insert(i, currentStaffInfo);
    }
}

// Draws the lines for a single staff
void ScoreArea::RenderStaffLines(Staff* staff, StaffData& currentStaffInfo, int lineSpacing, int width, QPointF& position)
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
    DrawStaff(currentStaffInfo.leftEdge, currentStaffInfo.getStdNotationLineHeight(1),
              currentStaffInfo.stdNotationLineSpacing, currentStaffInfo.width, currentStaffInfo.numOfStdNotationLines);

    // Draw tab staff lines
    DrawStaff(currentStaffInfo.leftEdge, currentStaffInfo.getTabLineHeight(1),
              currentStaffInfo.tabLineSpacing, currentStaffInfo.width, currentStaffInfo.numOfStrings);
}

// Draw all of the barlines for the staff.
void ScoreArea::RenderBars(const StaffData& currentStaffInfo, System* system)
{
    BarlinePainter *firstBarLine = new BarlinePainter(currentStaffInfo, &system->GetStartBarRef());
    if (system->GetStartBarConstRef().IsBar()) // for normal bars, display a line at the far left edge
    {
        firstBarLine->setPos(system->GetRect().GetLeft() - firstBarLine->boundingRect().width() / 2 - 0.5,
                             currentStaffInfo.getStdNotationLineHeight(1));
    }
    else // otherwise, display the bar after the clef, etc, and to the left of the first note
    {
        firstBarLine->setPos(system->GetFirstPositionX() - currentStaffInfo.positionWidth, currentStaffInfo.getStdNotationLineHeight(1));
    }
    scene.addItem(firstBarLine);

    // draw key signature
    const KeySignature& firstKeySig = system->GetStartBarConstRef().GetKeySignatureConstRef();
    if (firstKeySig.IsShown())
    {
        KeySignaturePainter *firstKeySigPainter = new KeySignaturePainter(currentStaffInfo, firstKeySig);
        firstKeySigPainter->setPos(system->GetRect().GetLeft() + system->GetClefWidth(), currentStaffInfo.getStdNotationLineHeight(1));
        scene.addItem(firstKeySigPainter);
    }

    // draw time signature
    const TimeSignature& firstTimeSig = system->GetStartBarConstRef().GetTimeSignatureConstRef();
    if (firstTimeSig.IsShown())
    {
        TimeSignaturePainter *firstTimeSigPainter = new TimeSignaturePainter(currentStaffInfo, firstTimeSig);
        firstTimeSigPainter->setPos(system->GetRect().GetLeft() + system->GetClefWidth() + firstKeySig.GetWidth() + 7,
                             currentStaffInfo.getStdNotationLineHeight(1));
        scene.addItem(firstTimeSigPainter);
    }

    for (uint32_t i=0; i < system->GetBarlineCount(); i++)
    {
        Barline *barLine = system->GetBarline(i);
        double x = system->GetPositionX(barLine->GetPosition());
        BarlinePainter *barLinePainter = new BarlinePainter(currentStaffInfo, barLine);
        barLinePainter->setPos(x, currentStaffInfo.getStdNotationLineHeight(1));
        scene.addItem(barLinePainter);

        const KeySignature& keySig = barLine->GetKeySignatureConstRef();
        if (keySig.IsShown())
        {
            KeySignaturePainter* keySigPainter = new KeySignaturePainter(currentStaffInfo, keySig);
            keySigPainter->setPos(x + barLinePainter->boundingRect().width() - 1, currentStaffInfo.getStdNotationLineHeight(1));
            scene.addItem(keySigPainter);
        }

        const TimeSignature& timeSig = barLine->GetTimeSignatureConstRef();
        if (timeSig.IsShown())
        {
            TimeSignaturePainter* timeSigPainter = new TimeSignaturePainter(currentStaffInfo, timeSig);
            timeSigPainter->setPos(x + barLinePainter->boundingRect().width() + keySig.GetWidth() + 2, currentStaffInfo.getStdNotationLineHeight(1));
            scene.addItem(timeSigPainter);
        }
    }

    BarlinePainter *lastBarLine = new BarlinePainter(currentStaffInfo, &system->GetEndBarRef());
    lastBarLine->setPos(system->GetRect().GetRight() - lastBarLine->boundingRect().width() / 2, currentStaffInfo.getStdNotationLineHeight(1));
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

void ScoreArea::DrawStaff(int leftEdge, int currentHeight, int lineSpacing, int width, int numberOfLines)
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

void ScoreArea::DrawTabClef(int x, StaffData& staffInfo)
{
    // Draw the tab clef
    QGraphicsSimpleTextItem* tabClef = new QGraphicsSimpleTextItem;
    tabClef->setPos(x, staffInfo.getTopTabLine() - 12);
    musicFont.setSymbol(tabClef, MusicFont::TabClef, staffInfo.numOfStrings * 4.25);
    scene.addItem(tabClef);
}

void ScoreArea::DrawTabNotes(System* system, Staff* staff, const StaffData& currentStaffInfo)
{
    for (uint32_t i=0; i < staff->GetPositionCount(0); i++)
    {
        Position* currentPosition = staff->GetPosition(0, i);
        uint32_t location = system->GetPositionX(currentPosition->GetPosition());

        for (uint32_t j=0; j < currentPosition->GetNoteCount(); j++)
        {
            Note* note = currentPosition->GetNote(j);

            TabNotePainter* tabNote = new TabNotePainter(note);
            CenterItem(tabNote, location, location + currentStaffInfo.positionWidth,
                       currentStaffInfo.getTabLineHeight(note->GetString()) + 13);
            scene.addItem(tabNote);
        }
    }
}

// Centers an item, by using it's width to calculate the necessary offset from xmin
void ScoreArea::CenterItem(QGraphicsItem* item, float xmin, float xmax, float y)
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
