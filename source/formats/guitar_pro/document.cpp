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
#include <score/generalmidi.h>

static const int NUM_LYRIC_LINES = 5;
static const int NUM_MIDI_CHANNELS = 64;

enum MeasureHeader
{
    Numerator,
    Denominator,
    RepeatBegin,
    RepeatEnd,
    AltEnding,
    Marker,
    KeySignatureChange,
    DoubleBar
};

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
    if (stream.version > Version4)
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
    // TODO - figure out if there is any useful information in here.
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

Channel::Channel()
    : myInstrument(0),
      myVolume(0),
      myBalance(0),
      myChorus(0),
      myReverb(0),
      myPhaser(0),
      myTremolo(0)
{
}

void Channel::load(InputStream &stream)
{
    myInstrument = boost::algorithm::clamp<int32_t>(stream.read<int32_t>(), 0,
                                                    Midi::NUM_MIDI_PRESETS);
    myVolume = readChannelProperty(stream);
    myBalance = readChannelProperty(stream);
    myChorus = readChannelProperty(stream);
    myReverb = readChannelProperty(stream);
    myPhaser = readChannelProperty(stream);
    myTremolo = readChannelProperty(stream);

    // TODO - figure out what these bytes are used for.
    stream.skip(2);
}

uint8_t Channel::readChannelProperty(InputStream &stream)
{
    uint8_t value = stream.read<uint8_t>();
    if (value != 0)
        value = (value * 8) - 1;

    return value;
}

Measure::Measure() : myIsDoubleBar(false), myIsRepeatBegin(false)
{
}

void Measure::load(InputStream &stream)
{
    const Flags flags = stream.read<uint8_t>();

    if (flags.test(MeasureHeader::Numerator) ||
        flags.test(MeasureHeader::Denominator))
    {
        std::pair<int, int> time;
        if (flags.test(MeasureHeader::Numerator))
            time.first = stream.read<int8_t>();

        if (flags.test(MeasureHeader::Denominator))
            time.second = stream.read<int8_t>();

        myTimeSignatureChange = time;
    }

    myIsDoubleBar = flags.test(MeasureHeader::DoubleBar);
    myIsRepeatBegin = flags.test(MeasureHeader::RepeatBegin);
    if (flags.test(MeasureHeader::RepeatEnd))
        myRepeatEnd = stream.read<int8_t>();

    if (flags.test(MeasureHeader::Marker) && stream.version == Version5_1)
        loadMarker(stream);

    if (flags.test(MeasureHeader::AltEnding))
        myAlternateEnding = stream.read<int8_t>();

    if (flags.test(MeasureHeader::Marker) && stream.version != Version5_1)
        loadMarker(stream);

    if (flags.test(MeasureHeader::KeySignatureChange))
    {
        myKeyChange = std::make_pair(stream.read<int8_t>(),
                                     static_cast<bool>(stream.read<int8_t>()));
    }

    // TODO - more unknown GP5 data ...
    if (stream.version > Version4)
    {
        if (flags.test(MeasureHeader::Numerator) ||
            flags.test(MeasureHeader::Denominator))
        {
            stream.skip(4);
        }

        if (!flags.test(MeasureHeader::AltEnding))
            stream.skip(1);

        stream.skip(1);
    }
}

void Measure::loadMarker(InputStream &stream)
{
    myMarker = stream.readString();
    // Ignore the marker's color.
    stream.skip(4);
}

Document::Document() : myStartTempo(0), myInitialKey(0), myOctave8va(false)
{
}

void Document::load(InputStream &stream)
{
    myHeader.load(stream);
    myStartTempo = stream.read<int32_t>();

    // TODO - figure out the meaning of this byte.
    if (stream.version == Version5_1)
        stream.skip(1);

    myInitialKey = stream.read<int32_t>();

    if (stream.version >= Version4)
        myOctave8va = stream.read<int8_t>();

    for (int i = 0; i < NUM_MIDI_CHANNELS; ++i)
    {
        Channel channel;
        channel.load(stream);
        myChannels.push_back(channel);
    }

    // TODO - is this RSE data?
    if (stream.version > Version4)
        stream.skip(42);

    const int numMeasures = stream.read<int32_t>();
    const int numTracks = stream.read<int32_t>();

    for (int i = 0; i < numMeasures; ++i)
    {
        // TODO - figure out what this byte is used for.
        if (stream.version > Version4 && i > 0)
            stream.skip(1);

        Measure measure;
        measure.load(stream);
        myMeasures.push_back(measure);
    }
}

}