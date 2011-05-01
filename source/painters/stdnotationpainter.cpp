#include "stdnotationpainter.h"

#include <QPainter>

#include <powertabdocument/position.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/keysignature.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>

#include <musicfont.h>

QFont StdNotationPainter::musicFont = MusicFont().getFont();

StdNotationPainter::StdNotationPainter(const StaffData& staffInfo, std::shared_ptr<const Staff> staff,
                                       const Position* position, const Note* note,
                                       const Tuning* tuning, const KeySignature* keySignature):
    staffInfo(staffInfo),
    staff(staff),
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
    yLocation = staff->GetNoteLocation(note, keySignature, tuning) * 0.5 * staffInfo.stdNotationLineSpacing + 1;
    
    const quint8 pitch = note->GetPitch(tuning);
    const bool usesSharps = keySignature->UsesSharps() || keySignature->HasNoKeyAccidentals();
    
    const QString noteText = QString::fromStdString(midi::GetMidiNoteText(pitch, usesSharps, 
                                                                          keySignature->NumberOfAccidentals()));
    accidental = findAccidentalType(noteText);
}

QRectF StdNotationPainter::boundingRect() const
{
    return QRectF(0, -staffInfo.getTopStdNotationLine(), width, staffInfo.getTopTabLine());
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

    double xPos = staffInfo.getNoteHeadRightEdge();

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

    // Display a different note head for harmonics
    if (note->IsNaturalHarmonic())
    {
        noteHead = MusicFont::NaturalHarmonicNoteHead;
        xPos += 1;
    }
    else if (note->HasArtificialHarmonic() || note->HasTappedHarmonic())
    {
        noteHead = MusicFont::ArtificialHarmonicNoteHead;
        xPos += 1;
    }

    const QString displayText = getAccidentalText() + noteHead;
    width = QFontMetricsF(musicFont).width(displayText);

    painter->drawText(xPos - width, yLocation, displayText);
    addDots(painter, xPos + 2, yLocation);
}

double StdNotationPainter::getNoteHeadWidth()
{
    return QFontMetricsF(musicFont).width(QChar(MusicFont::QuarterNoteOrLess));
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
