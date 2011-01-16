#include "stdnotationpainter.h"

#include <QPainter>
#include <cmath>

#include <powertabdocument/position.h>
#include <powertabdocument/guitar.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/keysignature.h>
#include <musicfont.h>
#include "staffdata.h"

QFont StdNotationPainter::musicFont = MusicFont().getFont();

StdNotationPainter::StdNotationPainter(const StaffData& staffInfo, Position* position, Guitar* guitar, KeySignature* keySignature):
        staffInfo(staffInfo),
        position(position),
        guitar(guitar),
        keySignature(keySignature)
{
}

void StdNotationPainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void StdNotationPainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
}

void StdNotationPainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

QRectF StdNotationPainter::boundingRect() const
{
    return QRectF(0, -staffInfo.getTopStdNotationLine(false), 10, staffInfo.getTopTabLine(false));
}

void StdNotationPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(musicFont);

    if (position->IsRest())
    {
        drawRest(painter);
        return;
    }

    // Choose the correct symbol to use for the note head
    QChar noteHead;
    if (position->GetDurationType() == 1)
    {
        noteHead = MusicFont::WholeNote;
    }
    else if (position->GetDurationType() == 2)
    {
        noteHead = MusicFont::HalfNote;
    }
    else
    {
        noteHead = MusicFont::QuarterNoteOrLess;
    }

    for (quint32 i=0; i < position->GetNoteCount(); i++)
    {
        Note* currentNote = position->GetNote(i);
        quint8 pitch = guitar->GetTuning().GetNote(currentNote->GetString()) + currentNote->GetFretNumber();

        const bool usesSharps = keySignature->UsesSharps() || keySignature->HasNoKeyAccidentals();
        const QString noteText = QString::fromStdString(GetMidiNoteText(pitch, usesSharps));

        const int octaveDiff = getOctaveDiff(currentNote, pitch);
        const double octaveShift = octaveDiff * staffInfo.stdNotationLineSpacing * 3.5;
        const int displayOffset = getDisplayPosition(noteText);

        painter->drawText(0, displayOffset * 0.5 * staffInfo.stdNotationLineSpacing + 1 + octaveShift, noteHead);
    }
}

int StdNotationPainter::getOctaveDiff(const Note* currentNote, const int pitch) const
{
    // get octave difference
    int octaveDiff = floor((MIDI_NOTE_F4 - pitch) / 12.0);

    // apply any octave-related notation instructions, like 8va or 8vb
    if (currentNote->IsOctave8va())
    {
        octaveDiff += 1;
    }
    if (currentNote->IsOctave15ma())
    {
        octaveDiff += 2;
    }
    if (currentNote->IsOctave8vb())
    {
        octaveDiff -= 1;
    }
    if (currentNote->IsOctave15mb())
    {
        octaveDiff -= 2;
    }

    return octaveDiff;
}

void StdNotationPainter::drawRest(QPainter *painter)
{
    QChar restSymbol;
    int height = 2 * staffInfo.stdNotationLineSpacing - 1; // position is in middle of staff by default

    switch(position->GetDurationType()) // find the correct symbol to display, and adjust the height if necessary
    {
    case 1:
        restSymbol = MusicFont::WholeRest;
        height = staffInfo.stdNotationLineSpacing + 1;
        break;
    case 2:
        restSymbol = MusicFont::HalfRest;
        height += 1;
        break;
    case 4:
        restSymbol = MusicFont::QuarterRest;
        break;
    case 8:
        restSymbol = MusicFont::EighthRest;
        break;
    case 16:
        restSymbol = MusicFont::SixteenthRest;
        break;
    case 32:
        restSymbol = MusicFont::ThirtySecondRest;
        break;
    case 64:
        restSymbol = MusicFont::SixtyFourthRest;
        height -= 3;
        break;
    }

    QString textToDraw = restSymbol;

    // add dots if necessary
    if (position->IsDotted())
    {
        textToDraw.append(QChar(MusicFont::Dot));
    }
    if (position->IsDoubleDotted())
    {
        textToDraw += QString(2, (QChar(MusicFont::Dot)));
        textToDraw.insert(2, " ");
    }

    painter->drawText(0, height, textToDraw);

    return;
}

int StdNotationPainter::getDisplayPosition(const QString& noteName)
{
    char note = noteName.at(0).toAscii();

    switch(note)
    {
    case 'E': return 1;
    case 'F': return noteName.length() == 1 ? 0 : 7; // special case, so that F# and Fb are not treated the same as F natural
    case 'G': return 6;
    case 'A': return 5;
    case 'B': return 4;
    case 'C': return 3;
    case 'D': return 2;
    default:
        return 0;
    }
}
