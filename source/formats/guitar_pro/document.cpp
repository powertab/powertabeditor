/*
  * Copyright (C) 2014 Cameron White
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

#include "document.h"

#include <formats/guitar_pro/inputstream.h>

static const int NUM_LYRIC_LINES = 5;

namespace Gp
{
Header::Header() : myTripletFeel(false), myLyricTrack(0)
{
}

Header::LyricLine::LyricLine(int measure, const std::string &contents)
    : myMeasure(measure), myContents(contents)
{
}

void Header::load(InputStream &stream)
{
    myTitle = stream.readString();
    mySubtitle = stream.readString();
    myArtist = stream.readString();
    myAlbum = stream.readString();

    myLyricist = stream.readString();
    if (stream.version > Gp::Version4)
        myComposer = stream.readString();
    else
        myComposer = myLyricist;

    myCopyright = stream.readString();
    myTranscriber = stream.readString();

    myInstructions = stream.readString();

    const uint32_t n = stream.read<uint32_t>();
    for (uint32_t i = 0; i < n; ++i)
        myNotices.push_back(stream.readString());

    if (stream.getVersion() <= Version4)
        myTripletFeel = stream.read<uint8_t>();

    if (stream.version >= Version4)
    {
        myLyricTrack = stream.read<uint32_t>();

        for (int i = 0; i < NUM_LYRIC_LINES; ++i)
        {
            myLyrics.emplace_back(stream.read<int32_t>(),
                                  stream.readIntString());
        }
    }

    // Ignore page setup information.
    if (stream.version > Version4)
    {
        if (stream.version == Version5_0)
            stream.skip(30);
        else if (stream.version == Version5_1)
            stream.skip(49);

        for (int i = 0; i < 11; ++i)
        {
            stream.skip(4);
            stream.readFixedLengthString(0);
        }
    }
}

void Document::load(InputStream &stream)
{
    myHeader.load(stream);
}

}