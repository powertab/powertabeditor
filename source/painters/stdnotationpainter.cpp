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
    width(10)
{
    init();
}

void StdNotationPainter::init()
{
    yLocation = staff->GetNoteLocation(note, keySignature, tuning) * 0.5 * Staff::STD_NOTATION_LINE_SPACING + 1;
    
    const quint8 pitch = note->GetPitch(tuning);
    const bool usesSharps = keySignature.UsesSharps() || keySignature.HasNoKeyAccidentals();
    
    const QString noteText = QString::fromStdString(midi::GetMidiNoteText(pitch, usesSharps, 
                                                                          keySignature.NumberOfAccidentals()));
    accidental = findAccidentalType(noteText);
}

void StdNotationPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setFont(musicFont);

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
    bounds = QRectF(0, -staffInfo.getTopStdNotationLine(), width, staffInfo.getTopTabLine());

    painter->drawText(xPos - width, yLocation, displayText);
    addDots(painter, xPos + 2, yLocation);
}

double StdNotationPainter::getNoteHeadWidth()
{
    return QFontMetricsF(musicFont).width(QChar(MusicFont::QuarterNoteOrLess));
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
