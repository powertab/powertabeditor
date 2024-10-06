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
  
#ifndef PAINTERS_MUSICFONT_H
#define PAINTERS_MUSICFONT_H

#include <QFont>

/// All of the available music symbols.
namespace MusicSymbol
{
// TODO - add the rest of the symbols
static inline constexpr QChar WholeRest(0xe102);
static inline constexpr QChar HalfRest(0xe103);
static inline constexpr QChar QuarterRest(0xe107);
static inline constexpr QChar EighthRest(0xe109);
static inline constexpr QChar SixteenthRest(0xe10a);
static inline constexpr QChar ThirtySecondRest(0xe10b);
static inline constexpr QChar SixtyFourthRest(0xe10c);
static inline constexpr QChar AccidentalSharp(0xe10e);
static inline constexpr QChar AccidentalDoubleSharp(0xe125);
static inline constexpr QChar AccidentalFlat(0xe11a);
static inline constexpr QChar AccidentalDoubleFlat(0xe123);
static inline constexpr QChar Natural(0xe116);
static inline constexpr QChar Dot(0xe130);
static inline constexpr QChar TrebleClef(0xe1a9);
static inline constexpr QChar BassClef(0xe1a7);
static inline constexpr QChar TabClef(0xe1ad);
static inline constexpr QChar CommonTime(0xe1af);
static inline constexpr QChar CutTime(0xe1b0);
static inline constexpr QChar WholeNote(0xe134);
static inline constexpr QChar HalfNote(0xe135);
static inline constexpr QChar QuarterNoteOrLess(0xe136);
static inline constexpr QChar FermataUp(0xe161);
static inline constexpr QChar FermataDown(0xe162);
static inline constexpr QChar Vibrato(0xe188);
static inline constexpr QChar WideVibrato(0xe18c);
static inline constexpr QChar Trill(0xe17a);
static inline constexpr QChar TremoloPicking(0xe19f);
static inline constexpr QChar ArpeggioDown(0xe189);
static inline constexpr QChar ArpeggioUp(0xe18a);
static inline constexpr QChar Marcato(0xe16a);
static inline constexpr QChar Sforzando(0xe172);
static inline constexpr QChar PickStrokeUp(0xe17d);
static inline constexpr QChar PickStrokeDown(0xe177);
static inline constexpr QChar FlagUp1(0xe199);
static inline constexpr QChar FlagUp2(0xe19a);
static inline constexpr QChar FlagUp3(0xe19b);
static inline constexpr QChar FlagUp4(0xe19c);
static inline constexpr QChar FlagDown1(0xe19e);
static inline constexpr QChar FlagDown2(0xe1a1);
static inline constexpr QChar FlagDown3(0xe1a2);
static inline constexpr QChar FlagDown4(0xe1a3);
static inline constexpr QChar GraceNoteSlashUp(0xe19f);
static inline constexpr QChar GraceNoteSlashDown(0xe1a0);
static inline constexpr QChar HarmonicNoteHeadOpen(0xe1d4);
static inline constexpr QChar HarmonicNoteHeadFull(0xe1d5);
static inline constexpr QChar Coda(0xe181);
static inline constexpr QChar Segno(0xe180);
static inline constexpr QChar RhythmSlashFilled(0xe141);
static inline constexpr QChar RhythmSlashNoFill(0xe140);
static inline constexpr QChar MutedNoteHead(0xe144);
} // namespace MusicSymbol

/// Provides an abstraction over the music notation font, by allowing one to
/// access notation symbols without directly specifying the Unicode value.
namespace MusicFont
{
    static constexpr int DEFAULT_FONT_SIZE = 22;
    static constexpr int GRACE_NOTE_SIZE = 15;

    QFont getFont(int pixel_size);
};

#endif
