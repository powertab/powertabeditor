#include "scorearea.h"

#include <QGraphicsPathItem>
#include <QDebug>
#include <QMultiMap>

#include <powertabdocument/powertabdocument.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/rect.h>
#include <powertabdocument/system.h>
#include <powertabdocument/tempomarker.h>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>
#include <powertabdocument/score.h>
#include <powertabdocument/direction.h>
#include <powertabdocument/chordtext.h>
#include <powertabdocument/rhythmslash.h>

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
#include <painters/notestem.h>
#include <painters/beamgroup.h>
#include <painters/irregularnotegroup.h>
#include <painters/directionpainter.h>
#include <painters/tremolobarpainter.h>
#include <painters/rhythmslashpainter.h>

#include <functional>
#include <algorithm>

using std::shared_ptr;

ScoreArea::ScoreArea(QWidget *parent) :
        QGraphicsView(parent)
{
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);

    setScene(&scene);
    setRenderHints(QPainter::HighQualityAntialiasing);

    activeStaff = NULL;
    activeSystem = NULL;

    redrawOnNextRefresh = false;
}

void ScoreArea::renderDocument(std::shared_ptr<PowerTabDocument> doc)
{
    scene.clear();
    systemList.clear();
    document = doc;
    int lineSpacing = document->GetTablatureStaffLineSpacing();

    // Set up the caret
    caret = new Caret(doc->GetTablatureStaffLineSpacing());
    connect(caret, SIGNAL(moved()), this, SLOT(adjustScroll()));

    caret->setScore(doc->GetGuitarScore());
    caret->updatePosition();

    scene.addItem(caret);

    // Render each score
    // Only worry about the guitar score so far
    renderScore(document->GetGuitarScore(), lineSpacing);
}

/// Updates the system after changes have been made
/// @param systemIndex The index of the system that was modified
void ScoreArea::updateSystem(const quint32 systemIndex)
{
    if (redrawOnNextRefresh)
    {
        redrawOnNextRefresh = false;
        renderDocument(document);
    }
    else
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
}

/// Used to request that a full redraw is performed when the ScoreArea is next updated
void ScoreArea::requestFullRedraw()
{
    Q_ASSERT(!redrawOnNextRefresh);
    redrawOnNextRefresh = true;
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

void ScoreArea::renderSystem(Score* score, shared_ptr<System> system, int lineSpacing)
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
        connect(staffPainter, SIGNAL(selectionUpdated(int,int)), caret, SLOT(updateSelection(int,int)));

        staffPainter->setPos(0, system->GetStaffHeightOffset(i));
        staffPainter->setParentItem(activeSystem);
        activeStaff = staffPainter;

        // Draw the clefs
        ClefPainter* clefPainter = new ClefPainter(currentStaffInfo, currentStaff);
        clefPainter->setPos(system->GetClefPadding(), currentStaffInfo.getTopStdNotationLine());
        clefPainter->setParentItem(activeStaff);

        drawTabClef(system->GetClefPadding(), currentStaffInfo);

        renderBars(currentStaffInfo, system);

        drawTabNotes(system, currentStaff, currentStaffInfo);
        drawStdNotation(system, currentStaff, currentStaffInfo);

        if (i == 0)
        {
            drawSystemSymbols(score, system, currentStaffInfo);
            drawRhythmSlashes(system);
        }

        drawLegato(system, currentStaff, currentStaffInfo);
        drawSlides(system, currentStaff, currentStaffInfo);
        drawSymbols(system, currentStaff, currentStaffInfo);
        drawSymbolsBelowTabStaff(system, currentStaff, currentStaffInfo);
    }
}

// Draw all of the barlines for the staff.
void ScoreArea::renderBars(const StaffData& currentStaffInfo, shared_ptr<System> system)
{
    std::vector<const Barline*> barlines;
    system->GetBarlines(barlines);

    for (size_t i = 0; i < barlines.size(); i++)
    {
        const Barline* currentBarline = barlines.at(i);
        const KeySignature& keySig = currentBarline->GetKeySignatureConstRef();
        const TimeSignature& timeSig = currentBarline->GetTimeSignatureConstRef();

        BarlinePainter* barlinePainter = new BarlinePainter(currentStaffInfo, currentBarline);

        // forward the barline's signal to our signal
        connect(barlinePainter, SIGNAL(clicked(int)), this, SIGNAL(barlineClicked(int)));

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
            keySigPainter->setPos(keySigX, currentStaffInfo.getTopStdNotationLine());
            keySigPainter->setParentItem(activeStaff);
        }

        if (timeSig.IsShown())
        {
            TimeSignaturePainter* timeSigPainter = new TimeSignaturePainter(currentStaffInfo, timeSig);
            timeSigPainter->setPos(timeSigX, currentStaffInfo.getTopStdNotationLine());
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

/// draws all of the slides for a staff
void ScoreArea::drawSlides(shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo)
{
    for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        // iterate across the staff for each string
        for (quint8 string = 0; string < staff->GetTablatureStaffType(); string++)
        {
            for (uint32_t j = 0; j < staff->GetPositionCount(voice); j++)
            {
                Position* currentPosition = staff->GetPosition(voice, j);
                Note* note = currentPosition->GetNoteByString(string);

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

                    // get the index of the next position
                    const size_t nextPosIndex = staff->GetIndexOfNextPosition(voice, system, currentPosition);

                    drawSlidesHelper(system, currentStaffInfo, string, slideUp, currentPosition->GetPosition(), nextPosIndex);
                }

                // draw any slides into the note
                note->GetSlideInto(type);
                if (type != Note::slideIntoNone)
                {
                    const quint32 currentPosIndex = currentPosition->GetPosition();

                    const bool slideUp = (type == Note::slideIntoFromBelow);

                    const quint32 prevPosIndex = (currentPosIndex == 0) ? 0 : currentPosIndex - 1;

                    drawSlidesHelper(system, currentStaffInfo, string, slideUp, prevPosIndex, currentPosIndex);
                }
            }
        }
    }
}

/// Helper function to perform the actual rendering of a slide
/// Draws a slide on the given string, between the two position indexes
void ScoreArea::drawSlidesHelper(shared_ptr<System> system, const StaffData& currentStaffInfo, quint8 string,
                                 bool slideUp, quint32 posIndex1, quint32 posIndex2)
{
    Q_ASSERT(posIndex1 <= posIndex2);

    const double leftPos = system->GetPositionX(posIndex1);
    const double width = system->GetPositionX(posIndex2) - leftPos;

    double height = 5;
    double y = (currentStaffInfo.getTabLineHeight(string + 1) + currentStaffInfo.getTabLineHeight(string)) / 2;

    if (slideUp) // if we're sliding up, flip the diagonal line
    {
        height = -height;
        y += currentStaffInfo.tabLineSpacing + 1;
    }

    QPainterPath path;
    path.lineTo(width - currentStaffInfo.positionWidth / 2, height);

    QGraphicsPathItem* line = new QGraphicsPathItem(path);
    line->setPos(leftPos + currentStaffInfo.positionWidth / 1.5 + 1, y + height / 2);
    line->setParentItem(activeStaff);
}

void ScoreArea::drawLegato(shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo)
{
    for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
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
                }

                else if (startPos != -1) // if an arc has been started, and the current note is not a hammer-on/pull-off, end the arc
                {
                    const double leftPos = system->GetPositionX(startPos);
                    const double width = system->GetPositionX(currentPosition) - leftPos;
                    double height = 7.5;
                    double y = currentStaffInfo.getTabLineHeight(string) - 2;

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
                    const double height = 10;
                    const double width = 6;
                    QPainterPath path;
                    path.moveTo(width, height / 2);
                    path.arcTo(0, 0, width, height, 0, 180);
                    QGraphicsPathItem *arc = new QGraphicsPathItem(path);
                    arc->setPos(system->GetPositionX(currentPosition) + 2,
                                currentStaffInfo.getTabLineHeight(string) - 2);
                    arc->setParentItem(activeStaff);

                    startPos = -1;
                }
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

void ScoreArea::drawRhythmSlashes(shared_ptr<const System> system)
{
    const uint32_t y = system->GetExtraSpacing();

    for (size_t i = 0; i < system->GetRhythmSlashCount(); i++)
    {
        const RhythmSlash* rhythmSlash = system->GetRhythmSlash(i);
        const uint32_t x = system->GetPositionX(rhythmSlash->GetPosition());

        RhythmSlashPainter* painter = new RhythmSlashPainter(rhythmSlash);
        painter->setPos(x, y);
        painter->setParentItem(activeSystem);
    }
}

void ScoreArea::drawSystemSymbols(Score* score, shared_ptr<System> system, const StaffData& currentStaffInfo)
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

    if (system->GetDirectionCount() > 0)
    {
        drawDirections(system, height, currentStaffInfo);
        height += System::SYSTEM_SYMBOL_SPACING * system->MaxDirectionSymbolCount();
        drawDividerLine(currentStaffInfo, height);
    }

    if (system->GetChordTextCount() > 0)
    {
        drawChordText(system, height, currentStaffInfo);
        height += System::SYSTEM_SYMBOL_SPACING;
        drawDividerLine(currentStaffInfo, height);
    }

}

void ScoreArea::drawTempoMarkers(std::vector<TempoMarker*> tempoMarkers, shared_ptr<System> system, quint32 height)
{
    foreach(TempoMarker* tempoMarker, tempoMarkers)
    {
        const quint32 location = system->GetPositionX(tempoMarker->GetPosition());

        TempoMarkerPainter* tempoMarkerPainter = new TempoMarkerPainter(tempoMarker);
        tempoMarkerPainter->setPos(location, height + 4);
        tempoMarkerPainter->setParentItem(activeSystem);
    }
}

void ScoreArea::drawChordText(shared_ptr<System> system, quint32 height, const StaffData& currentStaffInfo)
{
    for (uint32_t i = 0; i < system->GetChordTextCount(); i++)
    {
        const ChordText* chordText = system->GetChordText(i);

        const quint32 location = system->GetPositionX(chordText->GetPosition());

        ChordTextPainter* chordTextPainter = new ChordTextPainter(chordText);
        centerItem(chordTextPainter, location, location + currentStaffInfo.positionWidth, height + 4);
        chordTextPainter->setParentItem(activeSystem);
    }
}

void ScoreArea::drawDirections(shared_ptr<const System> system, quint32 height,
                               const StaffData& currentStaffInfo)
{
    for (uint32_t i = 0; i < system->GetDirectionCount(); i++)
    {
        const Direction* direction = system->GetDirection(i);

        quint32 directionHeight = height; // keeps track of the height for this Direction only

        const quint32 location = system->GetPositionX(direction->GetPosition());

        // draw each symbol for the Direction
        for (size_t symbol = 0; symbol < direction->GetSymbolCount(); symbol++)
        {
            DirectionPainter* directionPainter = new DirectionPainter(direction, symbol);
            centerItem(directionPainter, location, location + currentStaffInfo.positionWidth, directionHeight + 4);
            directionPainter->setParentItem(activeSystem);

            directionHeight += System::SYSTEM_SYMBOL_SPACING;
        }
    }
}

/// Draws the tab notes for all notes (and voices) in the staff
void ScoreArea::drawTabNotes(shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo)
{
    for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        for (quint32 i=0; i < staff->GetPositionCount(voice); i++)
        {
            Position* currentPosition = staff->GetPosition(voice, i);
            const quint32 location = system->GetPositionX(currentPosition->GetPosition());

            for (quint32 j=0; j < currentPosition->GetNoteCount(); j++)
            {
                Note* note = currentPosition->GetNote(j);

                TabNotePainter* tabNote = new TabNotePainter(note);
                centerItem(tabNote, location, location + currentStaffInfo.positionWidth,
                           currentStaffInfo.getTabLineHeight(note->GetString()) + currentStaffInfo.tabLineSpacing / 2 - 1);
                tabNote->setParentItem(activeStaff);
            }

            // draw arpeggios if necessary
            if (currentPosition->HasArpeggioDown() || currentPosition->HasArpeggioUp())
            {
                drawArpeggio(currentPosition, location, currentStaffInfo);
            }
        }
    }
}

void ScoreArea::drawArpeggio(Position* position, quint32 x, const StaffData& currentStaffInfo)
{
    // get the highest and lowest strings used at this position, and
    // convert the string indices to positions on the staff
    std::pair<quint8, quint8> bounds = position->GetStringBounds();
    bounds.first = currentStaffInfo.getTabLineHeight(bounds.first + 1);
    bounds.second = currentStaffInfo.getTabLineHeight(bounds.second + 1);

    const uint8_t height = bounds.second - bounds.first;

    // take a vibrato segment, spanning the distance from top to bottom note, and then rotate it by 90 degrees
    QFont font = musicFont.getFont();
    const QChar arpeggioSymbol = musicFont.getSymbol(MusicFont::Vibrato);
    const double symbolWidth = QFontMetricsF(font).width(arpeggioSymbol);
    const int numSymbols = height / symbolWidth;

    QGraphicsSimpleTextItem* arpeggio = new QGraphicsSimpleTextItem(QString(numSymbols, arpeggioSymbol));
    arpeggio->setFont(font);
    arpeggio->setPos(x + arpeggio->boundingRect().height() / 2.0 - 3.0, bounds.first);
    arpeggio->setRotation(90);
    arpeggio->setParentItem(activeStaff);

    // draw the end of the arpeggio
    const QChar arpeggioEnd = position->HasArpeggioUp() ? musicFont.getSymbol(MusicFont::ArpeggioUp) :
                              musicFont.getSymbol(MusicFont::ArpeggioDown);

    QGraphicsSimpleTextItem* endPoint = new QGraphicsSimpleTextItem(arpeggioEnd);
    const double y = position->HasArpeggioUp() ? bounds.first : bounds.second;
    endPoint->setFont(font);
    endPoint->setPos(x, y - 1.45 * font.pixelSize());
    endPoint->setParentItem(activeStaff);
}

void ScoreArea::drawStdNotation(shared_ptr<System> system, Staff* staff, const StaffData& currentStaffInfo)
{
    Barline* currentBarline = NULL;
    Barline* prevBarline = system->GetStartBarPtr();

    QList<StdNotationPainter*> notePainters;
    QMultiMap<double, StdNotationPainter*> accidentalsMap;
    std::list<NoteStem> stems;

    for (quint32 voice = 0; voice < Staff::NUM_STAFF_VOICES; voice++)
    {
        for (quint32 i=0; i < staff->GetPositionCount(voice); i++)
        {
            const Position* currentPosition = staff->GetPosition(voice, i);
            const quint32 location = system->GetPositionX(currentPosition->GetPosition());
            currentBarline = system->GetPrecedingBarline(currentPosition->GetPosition());
            KeySignature* currentKeySig = currentBarline->GetKeySignaturePtr();

            // Find the guitar corresponding to the current staff
            shared_ptr<Guitar> currentGuitar = document->GetGuitarScore()->GetGuitar(system->FindStaffIndex(staff));
            Q_ASSERT(currentGuitar);

            std::vector<double> noteLocations;

            // if we reach a new bar, we can adjust all of the accidentals for the previous bar
            if (currentBarline != prevBarline)
            {
                adjustAccidentals(accidentalsMap);
                accidentalsMap.clear();
                prevBarline = currentBarline;
            }

            // just draw rests right away, since we don't have to worry about beaming or accidentals
            if (currentPosition->IsRest())
            {
                StdNotationPainter* stdNotePainter = new StdNotationPainter(currentStaffInfo, staff, currentPosition, 
                                                                            NULL, currentGuitar->GetTuningPtr(), currentKeySig);
                centerItem(stdNotePainter, location, location+currentStaffInfo.positionWidth * 1.25,
                           currentStaffInfo.getTopStdNotationLine());
                stdNotePainter->setParentItem(activeStaff);
                continue;
            }

            for (uint32_t j = 0; j < currentPosition->GetNoteCount(); j++)
            {
                const Note* note = currentPosition->GetNote(j);

                StdNotationPainter* stdNotePainter = new StdNotationPainter(currentStaffInfo, staff, currentPosition,
                                                                            note, currentGuitar->GetTuningPtr(), currentKeySig);
                notePainters << stdNotePainter;

                // map all of the notes for each position on the staff, so that we can adjust accidentals later
                accidentalsMap.insert(stdNotePainter->getYLocation(), stdNotePainter);

                noteLocations.push_back(stdNotePainter->getYLocation());
            }

            if (currentPosition->GetDurationType() != 1)
            {
                NoteStem stem(currentStaffInfo, currentPosition, location, noteLocations);
                stems.push_back(stem);
            }
        }
    }

    // after adjusting accidentals, etc, we can add the painters to the scene
    foreach(StdNotationPainter* painter, notePainters)
    {
        const quint32 location = system->GetPositionX(painter->getPositionObject()->GetPosition());
        painter->setPos(location, currentStaffInfo.getTopStdNotationLine());
        painter->setParentItem(activeStaff);
    }

    std::vector<NoteStem> currentStemGroup;
    std::list<NoteStem> updatedStems;

    // group all of the stems into their beaming groups, and draw them
    while(!stems.empty())
    {
        NoteStem stem = stems.front();
        stems.pop_front();

        currentStemGroup.push_back(stem);

        if (stem.position()->IsBeamEnd() || (!stem.position()->IsBeamStart() && currentStemGroup.size() == 1))
        {
            BeamGroup group(currentStaffInfo, currentStemGroup);
            currentStemGroup.clear();

            group.drawStems(activeStaff);

            // grab a copy of the updated note stems for later use
            std::vector<NoteStem> temp;
            group.copyNoteSteams(temp);
            updatedStems.insert(updatedStems.end(), temp.begin(), temp.end());
        }
    }

    // Now, draw any irregular note groupings (triplets, etc)
    // This must be done after the beams are drawn, since the note stems will be adjusted during that process
    std::vector<NoteStem> currentIrregularNoteGroup;

    while(!updatedStems.empty())
    {
        NoteStem stem = updatedStems.front();
        updatedStems.pop_front();

        // check if this note isn't part of an irregular grouping
        if (!stem.position()->IsIrregularGroupingEnd() &&
                !stem.position()->IsIrregularGroupingMiddle() &&
                !stem.position()->IsIrregularGroupingStart())
        {
            currentIrregularNoteGroup.clear();
            continue;
        }

        currentIrregularNoteGroup.push_back(stem);

        // draw the grouping
        if (stem.position()->IsIrregularGroupingEnd())
        {
            IrregularNoteGroup irregularGroup(currentIrregularNoteGroup);
            irregularGroup.draw(activeStaff);

            currentIrregularNoteGroup.clear();
        }
    }
}

/// Draws the text symbols that appear below the tab staff (hammerons, slides, etc)
void ScoreArea::drawSymbolsBelowTabStaff(shared_ptr<System> system, Staff *staff, const StaffData &currentStaffInfo)
{
    std::vector<Staff::PositionProperty> positionPredicates = {
        &Position::HasPickStrokeDown, &Position::HasPickStrokeUp, &Position::HasTap,
        &Position::HasNoteWithHammeron, &Position::HasNoteWithPulloff,
        &Position::HasNoteWithHammeronFromNowhere, &Position::HasNoteWithPulloffToNowhere,
        &Position::HasNoteWithSlide, &Position::HasNoteWithTappedHarmonic,
        &Position::HasNoteWithArtificialHarmonic
    };

    typedef std::function<QGraphicsItem* (void)> SymbolCreationFn;
    using std::bind;
    using namespace std::placeholders;

    // functions for creating symbols, corresponding to each element of positionPredicates
    std::vector<SymbolCreationFn> symbolCreationsFns = {
        bind(&ScoreArea::createPickStroke, this, musicFont.getSymbol(MusicFont::PickStrokeDown)),
        bind(&ScoreArea::createPickStroke, this, musicFont.getSymbol(MusicFont::PickStrokeUp)),
        bind(&ScoreArea::createPlainText, this, "T", QFont::StyleNormal),
        bind(&ScoreArea::createPlainText, this, "H", QFont::StyleNormal),
        bind(&ScoreArea::createPlainText, this, "P", QFont::StyleNormal),
        bind(&ScoreArea::createPlainText, this, "H", QFont::StyleNormal),
        bind(&ScoreArea::createPlainText, this, "P", QFont::StyleNormal),
        bind(&ScoreArea::createPlainText, this, "sl.", QFont::StyleItalic),
        bind(&ScoreArea::createPlainText, this, "T", QFont::StyleNormal),
        // temporary placeholder - we will properly bind this later with the correct text for the harmonic's note value
        bind(&ScoreArea::createArtificialHarmonicText, this, (Position*)NULL),
    };

    std::list<SymbolInfo> symbols;

    // check each property for each position
    for (auto predicate = positionPredicates.begin(); predicate != positionPredicates.end(); ++predicate)
    {
        for (size_t i = 0; i < staff->GetPositionCount(0); i++)
        {
            Position* currentPosition = staff->GetPosition(0, i);

            const bool propertySet = (currentPosition->**predicate)();

            if (propertySet)
            {
                SymbolInfo symbolInfo;

                // symbols that are centered between adjacent positions
                if (*predicate == &Position::HasNoteWithSlide || *predicate == &Position::HasNoteWithHammeron ||
                    *predicate == &Position::HasNoteWithPulloff)
                {
                    int nextPos = staff->GetIndexOfNextPosition(0, system, currentPosition);
                    if (nextPos != system->GetPositionCount() - 1)
                        nextPos++;

                    symbolInfo.rect.setRect(currentPosition->GetPosition(), 0, nextPos - currentPosition->GetPosition(), 1);
                }
                else // symbols that appear directly below a position
                {
                    symbolInfo.rect.setRect(currentPosition->GetPosition(), 0, 1, 1);
                }

                // draw the symbol
                SymbolCreationFn symbolCreator = symbolCreationsFns.at(predicate - positionPredicates.begin());

                if (*predicate == &Position::HasNoteWithArtificialHarmonic)
                {
                    symbolCreator = bind(&ScoreArea::createArtificialHarmonicText, this, currentPosition);
                }

                symbolInfo.symbol = symbolCreator();
                symbols.push_back(symbolInfo);
            }
        }
    }

    // stores the maximum height that has been occupied by symbols at each position
    // this is used to figure out how low we can place a symbol
    std::vector<uint8_t> heightMap(system->GetPositionCount(), 0);

    // draw each symbol
    for (auto i = symbols.begin(); i != symbols.end(); i++)
    {
        const int left = i->rect.left();

        // find the lowest height we can place the symbol at without overlapping the already-placed symbols
        const int height = heightMap.at(left) + 1;
        // update the height map and adjust the current symbol's height
        heightMap[left] = height;
        i->rect.moveTop(height);

        // draw the symbol group
        centerItem(i->symbol, system->GetPositionX(left), system->GetPositionX(left + i->rect.width()),
                   currentStaffInfo.getBottomTabLine() + i->rect.y() * Staff::TAB_SYMBOL_HEIGHT);

        i->symbol->setParentItem(activeStaff);
    }
}

/// Draws the symbols that appear between the tab and standard notation staves
void ScoreArea::drawSymbols(shared_ptr<System> system, Staff *staff, const StaffData &currentStaffInfo)
{
    typedef std::function<QGraphicsItem* (uint8_t, const StaffData&)> SymbolCreationFn;
    using std::bind;
    using namespace std::placeholders;

    std::list<SymbolInfo> symbols; // holds all of the symbols that we will draw

    std::vector<Staff::PositionProperty> groupableSymbolPredicates = {
        &Position::HasLetRing, &Position::HasVibrato, &Position::HasWideVibrato,
        &Position::HasPalmMuting, &Position::HasNoteWithNaturalHarmonic,
        &Position::HasNoteWithArtificialHarmonic
    };

    // function objects for drawing symbols corresponding to each of the items in the groupableSymbolPredicates list
    std::vector<SymbolCreationFn> groupableSymbolCreators = {
        bind(&ScoreArea::createConnectedSymbolGroup, this, "let ring", QFont::StyleItalic, _1, _2),
        bind(&ScoreArea::drawContinuousFontSymbols, this, MusicFont::Vibrato, _1, _2),
        bind(&ScoreArea::drawContinuousFontSymbols, this, MusicFont::WideVibrato, _1, _2),
        bind(&ScoreArea::createConnectedSymbolGroup, this, "P.M.", QFont::StyleNormal, _1, _2),
        bind(&ScoreArea::createConnectedSymbolGroup, this, "N.H.", QFont::StyleNormal, _1, _2),
        bind(&ScoreArea::createConnectedSymbolGroup, this, "A.H.", QFont::StyleNormal, _1, _2),
    };

    // Generates the bounding rectangles for each symbol group (i.e. consecutive 'let ring' symbols)
    // - each rectangle has equal height, and a fixed x-coordinate (the position that a symbol group starts at)
    // - the rectangle's width represents the number of positions that the symbol group spans
    // - we later adjust the rectangles' y-coordinates to arrange the symbols without overlap
    for (auto predicate = groupableSymbolPredicates.begin(); predicate != groupableSymbolPredicates.end(); ++predicate)
    {
        bool inGroup = false;
        SymbolInfo currentSymbolInfo;
        const size_t numPositions = staff->GetPositionCount(0);
        SymbolCreationFn symbolCreator = groupableSymbolCreators.at(predicate - groupableSymbolPredicates.begin());

        for (size_t i = 0; i < numPositions; i++)
        {
            const Position* currentPosition = staff->GetPosition(0, i);

            const bool propertySet = (currentPosition->**predicate)();

            if (propertySet && inGroup)
            {
                // extend the rectangle to the current position
                currentSymbolInfo.rect.setRight(currentPosition->GetPosition());
            }
            if (propertySet && !inGroup)
            {
                // start the rectangle
                currentSymbolInfo.rect.setRect(currentPosition->GetPosition(), 0, 1, 1);
                inGroup = true;
            }
            if (!propertySet && inGroup)
            {
                // close up the rectangle and draw the symbol
                currentSymbolInfo.rect.setRight(currentPosition->GetPosition() - 1);
                inGroup = false;
                currentSymbolInfo.symbol = symbolCreator(currentSymbolInfo.rect.width(), currentStaffInfo);
                symbols.push_back(currentSymbolInfo);
            }
        }

        // if we're at the end of the staff, close the current rectangle if necessary
        if (inGroup)
        {
            currentSymbolInfo.symbol = symbolCreator(currentSymbolInfo.rect.width(), currentStaffInfo);
            symbols.push_back(currentSymbolInfo);
        }
    }

    std::vector<Staff::PositionProperty> singleSymbolPredicates = {
        &Position::HasVolumeSwell, &Position::HasTremoloPicking,
        &Position::HasTremoloBar, &Position::HasNoteWithTrill
    };

    // function objects for drawing symbols corresponding to each of the items in the singleSymbolPredicates list
    std::vector<SymbolCreationFn> singleSymbolCreators = {
        bind(&ScoreArea::createVolumeSwell, this, _1, _2, VolumeIncreasing), // set as VolumeIncreasing for now - we will update it later
        bind(&ScoreArea::createTremoloPicking, this, _1, _2),
        bind(&ScoreArea::createTremoloBar, this, _1, _2, (Position*)NULL),
        bind(&ScoreArea::createTrill, this, _1, _2),
    };

    // Now, generate the rectangles for individual (non-groupable) symbols
    for (auto predicate = singleSymbolPredicates.begin(); predicate != singleSymbolPredicates.end(); ++predicate)
    {
        SymbolInfo currentSymbolInfo;

        for (size_t i = 0; i < staff->GetPositionCount(0); i++)
        {
            const Position* currentPosition = staff->GetPosition(0, i);

            const bool propertySet = (currentPosition->**predicate)();

            if (propertySet)
            {
                // create the rectangle and render the symbol
                currentSymbolInfo.rect.setRect(currentPosition->GetPosition(), 0, 1, 1);

                SymbolCreationFn symbolCreator = singleSymbolCreators.at(predicate - singleSymbolPredicates.begin());

                // if a volume swell is set, set the symbol width to span across the duration of the volume swell
                if (*predicate == &Position::HasVolumeSwell)
                {
                    quint8 startVolume = 0, endVolume = 0, duration = 0;
                    currentPosition->GetVolumeSwell(startVolume, endVolume, duration);

                    uint32_t lastPosIndex = 0;

                    // get the position of the last note of the volume swell
                    // if the volume swell extends onto the next system, just display until the end of this staff
                    if (!staff->IsValidPositionIndex(0, i + duration))
                    {
                        lastPosIndex = system->GetPositionCount() - 1;
                    }
                    else
                    {
                        lastPosIndex = staff->GetPosition(0, i + duration)->GetPosition();
                    }
                    currentSymbolInfo.rect.setRight(lastPosIndex);

                    // need to re-bind the function using the correct volume swell type
                    const VolumeSwellType type = (startVolume <= endVolume) ? VolumeIncreasing : VolumeDecreasing;
                    symbolCreator = bind(&ScoreArea::createVolumeSwell, this, _1, _2, type);
                }
                else if (*predicate == &Position::HasTremoloBar)
                {
                    uint8_t type = 0, duration = 0, pitch = 0;
                    currentPosition->GetTremoloBar(type, duration, pitch);

                    if (!staff->IsValidPositionIndex(0, i + duration))
                    {
                        currentSymbolInfo.rect.setRight(system->GetPositionCount() - 1);
                    }
                    else
                    {
                        currentSymbolInfo.rect.setRight(staff->GetPosition(0, i + duration)->GetPosition() + 1);
                    }
                    symbolCreator = bind(&ScoreArea::createTremoloBar, this, _1, _2 , currentPosition);
                }

                currentSymbolInfo.symbol = symbolCreator(currentSymbolInfo.rect.width(), currentStaffInfo);
                symbols.push_back(currentSymbolInfo);
            }
        }
    }

    // stores the maximum height that has been occupied at each position
    // this is used to figure out how low we can place a symbol group
    std::vector<uint8_t> heightMap(system->GetPositionCount(), 0);

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
                          currentStaffInfo.getTopTabLine() - (i->rect.y() + 1) * Staff::TAB_SYMBOL_HEIGHT);
        i->symbol->setParentItem(activeStaff);
    }
}

// Draws symbols that are grouped across multiple positions (i.e. consecutive "let ring" symbols)
QGraphicsItem* ScoreArea::createConnectedSymbolGroup(const QString& text, QFont::Style style, uint8_t width,
                                                     const StaffData& currentStaffInfo)
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

QGraphicsItem* ScoreArea::createTrill(uint8_t width, const StaffData& currentStaffInfo) const
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

QGraphicsItem* ScoreArea::createTremoloPicking(uint8_t width, const StaffData& currentStaffInfo) const
{
    Q_UNUSED(width);

    const double offset = Staff::TAB_SYMBOL_HEIGHT / 3;

    QGraphicsItemGroup* group = new QGraphicsItemGroup;

    for (int i = 0; i < 3; i++)
    {
        QGraphicsSimpleTextItem* line = new QGraphicsSimpleTextItem(MusicFont::getSymbol(MusicFont::TremoloPicking));
        line->setFont(musicFont.getFontRef());
        centerItem(line, 0, currentStaffInfo.positionWidth * 1.25, -37 + i * offset);
        group->addToGroup(line);
    }

    return group;
}

QGraphicsItem* ScoreArea::createTremoloBar(uint8_t width, const StaffData& currentStaffInfo,
                                           const Position* position) const
{
    return new TremoloBarPainter(position, width * currentStaffInfo.positionWidth);
}

/// Creates a volume swell QGraphicsItem of the specified type
QGraphicsItem* ScoreArea::createVolumeSwell(uint8_t width, const StaffData &currentStaffInfo, VolumeSwellType type)
{
    double leftX = currentStaffInfo.positionWidth / 2.0;
    double rightX = width * currentStaffInfo.positionWidth;

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

// Centers an item, by using it's width to calculate the necessary offset from xmin
void ScoreArea::centerItem(QGraphicsItem* item, float xmin, float xmax, float y) const
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

/// Creates a plain text item; useful for symbols that don't use the music font (hammerons, slides, etc)
QGraphicsItem* ScoreArea::createPlainText(const QString& text, QFont::Style style)
{
    QFont displayFont("Liberation Sans");
    displayFont.setPixelSize(10);
    displayFont.setStyle(style);

    QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(text);
    textItem->setFont(displayFont);
    textItem->setPos(0, -8);

    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(textItem);

    return group;
}

QGraphicsItem* ScoreArea::createPickStroke(const QString& text)
{
    QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(text);
    textItem->setFont(musicFont.getFontRef());
    textItem->setPos(2, -28);

    // Sticking the text in a QGraphicsItemGroup allows us to offset the position of the text from its default location
    QGraphicsItemGroup* group = new QGraphicsItemGroup;
    group->addToGroup(textItem);

    return group;
}

// Creates the text portion of an artificial harmonic - displaying the note value
QGraphicsItem* ScoreArea::createArtificialHarmonicText(Position* position)
{
    std::vector<Note*> notes;
    position->GetNotes(notes);

    // find the note with an artificial harmonic
    auto location = std::find_if(notes.begin(), notes.end(), std::mem_fun(&Note::HasArtificialHarmonic));

     // this function should not even be executed if there is no note with an artificial harmonic
    Q_ASSERT(location != notes.end());

    Note* note = *location;

    quint8 key = 0, variation = 0, octave = 0;
    note->GetArtificialHarmonic(key, variation, octave);

    // use the ChordName class to convert the key/variation data to a text representation
    ChordName name(key, variation, ChordName::DEFAULT_FORMULA, ChordName::DEFAULT_FORMULA_MODIFICATIONS);

    const QString text = QString().fromStdString(name.GetKeyText(false));

    return createPlainText(text, QFont::StyleNormal);
}
