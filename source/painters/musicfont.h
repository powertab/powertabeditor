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
class QGraphicsSimpleTextItem;

/*
 Provides an abstraction over the music notation font, by allowing one to
 access notation symbols without directly specifying the Unicode value.
 */
class MusicFont
{
public:
    /// All of the available music symbols.
    enum MusicSymbol
    {
        // TODO - add the rest of the symbols
        WholeRest = 0xe102,
        HalfRest = 0xe103,
        QuarterRest = 0xe107,
        EighthRest = 0xe109,
        SixteenthRest = 0xe10a,
        ThirtySecondRest = 0xe10b,
        SixtyFourthRest = 0xe10c,
        AccidentalSharp = 0xe10e,
        AccidentalDoubleSharp = 0xe125,
        AccidentalFlat = 0xe11a,
        AccidentalDoubleFlat = 0xe123,
        Natural = 0xe116,
        Dot = 0xe130,
        TrebleClef = 0xe1a9,
        BassClef = 0xe1a7,
        TabClef = 0xe1ad,
        CommonTime = 0xe1af,
        CutTime = 0xe1b0,
        WholeNote = 0xe134,
        HalfNote = 0xe135,
        QuarterNoteOrLess = 0xe136,
        FermataUp = 0xe161,
        FermataDown = 0xe162,
        Vibrato = 0xe188,
        WideVibrato = 0xe18c,
        Trill = 0xe17a,
        TremoloPicking = 0xe19f,
        ArpeggioDown = 0xe189,
        ArpeggioUp = 0xe18a,
        Marcato = 0xe16a,
        Sforzando = 0xe172,
        PickStrokeUp = 0xe17d,
        PickStrokeDown = 0xe177,
        FlagUp1 = 0xe199,
        FlagUp2 = 0xe19a,
        FlagUp3 = 0xe19b,
        FlagUp4 = 0xe19c,
        FlagDown1 = 0xe19e,
        FlagDown2 = 0xe1a1,
        FlagDown3 = 0xe1a2,
        FlagDown4 = 0xe1a3,
        GraceNoteSlashUp = 0xe19f,
        GraceNoteSlashDown = 0xe1a0,
        HarmonicNoteHeadOpen = 0xe1d4,
        HarmonicNoteHeadFull = 0xe1d5,
        Coda = 0xe181,
        Segno = 0xe180,
        RhythmSlashFilled = 0xe141,
        RhythmSlashNoFill = 0xe140,
        MutedNoteHead = 0xe144
    };

    static const int DEFAULT_FONT_SIZE = 22;
    static const int GRACE_NOTE_SIZE = 15;

    static QFont getFont(int pixel_size);
};

#endif
