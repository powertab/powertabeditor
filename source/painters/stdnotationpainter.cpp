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
  
#include "stdnotationpainter.h"

#include <QPainter>

#include <powertabdocument/position.h>
#include <powertabdocument/generalmidi.h>
#include <powertabdocument/keysignature.h>
#include <powertabdocument/note.h>
#include <powertabdocument/staff.h>

#include <painters/musicfont.h>

QFont StdNotationPainter::musicFont = MusicFont().getFont();

StdNotationPainter::StdNotationPainter(const StaffData& staffInfo, boost::shared_ptr<const Staff> staff,
                                       const Position* position, const Note* note,
                                       const Tuning& tuning, const KeySignature& keySignature):
    staffInfo(staffInfo),
    staff(staff),
    position(position),
    note(note),
    tuning(tuning),
    keySignature(keySignature),
    yLocation(0), xLocation(0), rightEdge(0), noteHead('X')
{
    init();
}

void StdNotationPainter::init()
{
    yLocation = staff->GetNoteLocation(note, keySignature, tuning) *
            0.5 * Staff::STD_NOTATION_LINE_SPACING;

    refreshAccidental(false);
    setNoteHead();
}

/// Update the accidental for the note.
/// @param forceAccidental Force accidentals or natural signs to be displayed
/// even if the note is in the key signature (used for undoing the effects of
/// preceding natural signs or accidentals).
void StdNotationPainter::refreshAccidental(bool forceAccidental)
{
    const quint8 pitch = note->GetPitch(tuning);
    const bool usesSharps = keySignature.UsesSharps() ||
            keySignature.HasNoKeyAccidentals();

    const QString noteText = QString::fromStdString(
                midi::GetMidiNoteText(pitch, usesSharps,
                                      keySignature.NumberOfAccidentals(),
                                      forceAccidental));

    accidental = findAccidentalType(noteText);
}

/// Choose the correct symbol to use for the note head.
void StdNotationPainter::setNoteHead()
{
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
    }
    else if (note->HasArtificialHarmonic() || note->HasTappedHarmonic())
    {
        noteHead = MusicFont::ArtificialHarmonicNoteHead;
    }

    const double noteWidth = noteHeadWidth();
    xLocation = (staffInfo.positionWidth - noteWidth) * 0.5;
    rightEdge = xLocation + noteWidth;
}

double StdNotationPainter::noteHeadWidth() const
{
    QFontMetricsF metrics(musicFont);
    return metrics.width(noteHead); // TODO - cache this value?
}

double StdNotationPainter::noteHeadRightEdge() const
{
    return rightEdge;
}

void StdNotationPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(musicFont);

    const QString accidentalText = getAccidentalText();
    QFontMetricsF metrics(musicFont);
    const double accidentalWidth = metrics.width(accidentalText);

    const QString displayText = accidentalText + noteHead;
    const double displayWidth = metrics.width(displayText);
    bounds = QRectF(0, -staffInfo.getTopStdNotationLine(), displayWidth,
                    staffInfo.getTopTabLine());

    painter->drawText(xLocation - accidentalWidth, yLocation + 1, displayText);
    addDots(painter, rightEdge + 2, yLocation + 1);
}

StdNotationPainter::AccidentalType StdNotationPainter::findAccidentalType(
        const QString& noteText) const
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
    if (noteText.endsWith("="))
    {
        return NATURAL;
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
    case NO_ACCIDENTAL:
        break; // Do nothing.
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

QRectF StdNotationPainter::boundingRect() const
{
    return bounds;
}

double StdNotationPainter::getYLocation() const
{
    return yLocation;
}

const Position *StdNotationPainter::getPositionObject() const
{
    return position;
}
