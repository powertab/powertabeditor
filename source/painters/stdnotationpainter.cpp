#include "stdnotationpainter.h"

#include <QPainter>
#include <cmath>

#include <powertabdocument/position.h>
#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/keysignature.h>
#include <powertabdocument/note.h>

#include <musicfont.h>
#include "staffdata.h"

QFont StdNotationPainter::musicFont = MusicFont().getFont();

StdNotationPainter::StdNotationPainter(const StaffData& staffInfo, Position* position, Note* note, Tuning* tuning, KeySignature* keySignature):
        staffInfo(staffInfo),
        position(position),
        note(note),
        tuning(tuning),
        keySignature(keySignature)
{
    if (!position->IsRest())
    {
        init();
    }
    width = 10;
}

void StdNotationPainter::init()
{
    quint8 pitch = note->GetPitch(tuning);

    const bool usesSharps = keySignature->UsesSharps() || keySignature->HasNoKeyAccidentals();
    const QString noteText = QString::fromStdString(midi::GetMidiNoteText(pitch, usesSharps, keySignature->NumberOfAccidentals()));

    const int octaveDiff = getOctaveDiff(note, pitch);
    const double octaveShift = octaveDiff * staffInfo.stdNotationLineSpacing * 3.5;
    int displayOffset = getDisplayPosition(noteText);

    if (midi::GetMidiNotePitch(pitch) == 5)
    {
        displayOffset = 0; // special case, so that F# and Fb are not treated the same as F natural
    }

    yLocation = displayOffset * 0.5 * staffInfo.stdNotationLineSpacing + 1 + octaveShift;
    accidental = findAccidentalType(noteText);
}

QRectF StdNotationPainter::boundingRect() const
{
    return QRectF(0, -staffInfo.getTopStdNotationLine(false), width, staffInfo.getTopTabLine(false));
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

    QString displayText = getAccidentalText() + noteHead;
    width = QFontMetricsF(musicFont).width(displayText);

    painter->drawText(staffInfo.getNoteHeadRightEdge() - width, yLocation, displayText);
    addDots(painter, staffInfo.getNoteHeadRightEdge() + 2, yLocation);
}

double StdNotationPainter::getNoteHeadWidth()
{
    return QFontMetricsF(musicFont).width(QChar(MusicFont::QuarterNoteOrLess));
}

int StdNotationPainter::getOctaveDiff(const Note* currentNote, const int pitch) const
{
    // get octave difference
    int octaveDiff = floor((midi::MIDI_NOTE_F4 - pitch) / 12.0);

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

    painter->drawText(0, height, textToDraw);
    addDots(painter, QFontMetricsF(musicFont).width(textToDraw) + 2, 1.6 * staffInfo.stdNotationLineSpacing);

    return;
}

int StdNotationPainter::getDisplayPosition(const QString& noteName)
{
    char note = noteName.at(0).toAscii();

    switch(note)
    {
    case 'E': return 1;
    case 'F': return 7;
    case 'G': return 6;
    case 'A': return 5;
    case 'B': return 4;
    case 'C': return 3;
    case 'D': return 2;
    default:
        return 0;
    }
}

int StdNotationPainter::findAccidentalType(const QString& noteText) const
{
    if (noteText.endsWith("##"))
    {
        return DOUBLE_SHARP;
    }
    if (noteText.endsWith("#"))
    {
        return SHARP;
    }
    if (noteText.endsWith("bb"))
    {
        return DOUBLE_FLAT;
    }
    if (noteText.endsWith("b"))
    {
        return FLAT;
    }

    return NO_ACCIDENTAL;
}

QString StdNotationPainter::getAccidentalText() const
{
    QString text;

    switch(accidental)
    {
    case FLAT:
        text += QChar(MusicFont::AccidentalFlat);
        break;
    case SHARP:
        text += QChar(MusicFont::AccidentalSharp);
        break;
    case DOUBLE_FLAT:
        text += QString(2, QChar(MusicFont::AccidentalFlat));
        break;
    case DOUBLE_SHARP:
        text += QString(2, QChar(MusicFont::AccidentalSharp));
        break;
    case NATURAL:
        text += QChar(MusicFont::Natural);
        break;
    }

    return text;
}

void StdNotationPainter::addDots(QPainter* painter, double x, double y) const
{
    const QChar dot = MusicFont::getSymbol(MusicFont::Dot);

    if (position->IsDotted())
    {
        painter->drawText(x, y, dot);
    }
    else if (position->IsDoubleDotted())
    {
        painter->drawText(x, y, dot);
        painter->drawText(x + 4, y, dot);
    }
}
