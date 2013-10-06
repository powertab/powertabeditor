/////////////////////////////////////////////////////////////////////////////
// Name:            note.h
// Purpose:         Stores and renders a note
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 17, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef NOTE_H
#define NOTE_H

#include <array>
#include "powertabobject.h"

namespace PowerTabDocument {

/// Stores and renders a note
class Note : public PowerTabObject
{
// Constants
public:
    // Default Constants
    static const uint8_t DEFAULT_STRING_DATA;            ///< Default value for the string data member variable
    static const uint16_t DEFAULT_SIMPLE_DATA;            ///< Default value for the simple data member variable

    // String Constants
    static const uint8_t MIN_STRING;                     ///< Minimum allowed string
    static const uint8_t MAX_STRING;                     ///< Maximum allowed string

    // Fret Number Constants
    static const uint8_t MIN_FRET_NUMBER;                ///< Minimum allowed fret number
    static const uint8_t MAX_FRET_NUMBER;                ///< Maximum allowed fret number

    // Bend Constants
    static const uint8_t MAX_BEND_PITCH;                 ///< Maximum allowed pitch for a bend, in quarter steps
    static const uint8_t MAX_BEND_DURATION;              ///< Maximum allowed value for the bend duration

    static const size_t MAX_NOTE_COMPLEX_SYMBOLS = 3;      ///< Maximum allowed number of complex symbols per note object

    enum flags
    {
        stringMask          = 0xe0,             ///< Mask used to retrieve the string
        fretNumberMask      = 0x1f              ///< Mask used to retrieve the fret number
    };

    enum simpleFlags
    {
        tied                        = 0x01,
        muted                       = 0x02,
        tieWrap                     = 0x04,     ///< Used to wrap a tie around systems
        hammerOn                    = 0x08,
        pullOff                     = 0x10,
        hammerPullFromToNowhere     = 0x20,
        hammerPullMask              = 0x38,     ///< Mask that filters out mutually exclusive hammer on/pull off flags
        naturalHarmonic             = 0x40,
        ghostNote                   = 0x80,
        octave8va                   = 0x100,
        octave15ma                  = 0x200,
        octave8vb                   = 0x400,
        octave15mb                  = 0x800,
        octaveMask                  = 0xf00,    ///< Mask that filters out mutually exclusive octave flags
        simpleFlagsMask             = 0xfff     ///< Mask that filters out all possible simple flags
    };

    enum complexSymbolTypes
    {
        slide                   = 'd',
        bend                    = 'e',
        tappedHarmonic          = 'f',
        trill                   = 'g',
        artificialHarmonic      = 'h',
        notUsed                 = 0
    };

    enum slideIntoTypes
    {
        slideIntoNone                       = 0x00,
        slideIntoFromBelow                  = 0x01,
        slideIntoFromAbove                  = 0x02,
        slideIntoShiftSlideUpwards          = 0x03,     ///< Used for wrapping shift slides around systems
        slideIntoShiftSlideDownwards        = 0x04,     ///< Used for wrapping shift slides around systems
        slideIntoLegatoSlideUpwards         = 0x05,     ///< Used for wrapping legato slides around systems
        slideIntoLegatoSlideDownwards       = 0x06      ///< Used for wrapping legato slides around systems
    };

    enum slideOutOfTypes
    {
        slideOutOfNone                      = 0x00,
        slideOutOfShiftSlide                = 0x01,
        slideOutOfLegatoSlide               = 0x02,
        slideOutOfDownwards                 = 0x03,
        slideOutOfUpwards                   = 0x04
    };

    enum slideFlags
    {
        slideIntoTypeMask       = 0xff0000,               ///< Mask used to retrieve the slide into type
        slideOutOfTypeMask      = 0xff00,                 ///< Mask used to retrieve the slide out of type
        slideOutOfStepsMask     = 0xff                   ///< Mask used to retrieve the slide out of steps
    };

    enum bendTypes
    {
        normalBend              = 0x00,
        bendAndRelease          = 0x01,
        bendAndHold             = 0x02,
        preBend                 = 0x03,
        preBendAndRelease       = 0x04,
        preBendAndHold          = 0x05,
        gradualRelease          = 0x06,
        immediateRelease        = 0x07
    };

    enum bendDrawingPoints
    {
        lowPoint                = 0x00,
        midPoint                = 0x01,
        highPoint               = 0x02
    };

    enum bendFlags
    {
        bendTypeMask            = 0xf00000,               ///< Mask used to retrieve the bend type
        drawStartMask           = 0xc0000,                ///< Mask used to retrieve the draw start value
        drawEndMask             = 0x30000,                ///< Mask used to retrieve the draw end value
        bendDurationMask        = 0xff00,                 ///< Mask used to retrieve the bend duration
        bentPitchMask           = 0xf0,                   ///< Mask used to retrieve the bent pitch value
        releasePitchMask        = 0xf                     ///< Mask used to retrieve the release pitch value
    };

    enum artificialHarmonicOctaves
    {
        artificialHarmonicOctaveLoco            = 0x00,
        artificialHarmonicOctave8va             = 0x01,
        artificialHarmonicOctave15ma            = 0x02
    };

// Member Variables
private:
    uint8_t      m_stringData;                                       ///< Top 3 bits = string, bottom 5 bits = fret number
    uint16_t      m_simpleData;						                ///< Contains simple symbol flags
    std::array<uint32_t, MAX_NOTE_COMPLEX_SYMBOLS> m_complexSymbolArray;    ///< Complex symbol data (1 symbol per element)

public:
    Note();

    // Serialization Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CLineData";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}

    uint32_t GetString() const;
    uint8_t GetFretNumber() const;

    bool IsTied() const;
    bool IsMuted() const;
    bool HasTieWrap() const;

// Hammer On Functions
    bool HasHammerOn() const;
    bool HasHammerOnFromNowhere() const;
    bool HasPullOff() const;
    bool HasPullOffToNowhere() const;

    bool IsNaturalHarmonic() const;
    bool IsGhostNote() const;
    bool IsOctave8va() const;
    bool IsOctave15ma() const;
    bool IsOctave8vb() const;
    bool IsOctave15mb() const;

// Simple Flag Functions
private:
    static bool IsValidSimpleFlag(uint16_t flag);
    bool IsSimpleFlagSet(uint16_t flag) const;

private:
    bool HasSlideOutType(uint8_t slideType) const;

public:
    inline bool HasLegatoSlide() const { return HasSlideOutType(slideOutOfLegatoSlide); }
    inline bool HasShiftSlide() const { return HasSlideOutType(slideOutOfShiftSlide); }

// Slide Into Functions
    bool GetSlideInto(uint8_t& type) const;
    bool HasSlideIntoFromAbove() const;
    bool HasSlideIntoFromBelow() const;

// Slide Out Of Functions
    bool GetSlideOutOf(uint8_t& type, int8_t& steps) const;
    bool HasSlideOutOfDownwards() const;
    bool HasSlideOutOfUpwards() const;

// Bend Functions
    bool GetBend(uint8_t& type, uint8_t& bentPitch, uint8_t& releasePitch,
                 uint8_t& duration, uint8_t& drawStartPoint, uint8_t& drawEndPoint) const;
    bool HasBend() const;

// Tapped Harmonic Functions
    bool GetTappedHarmonic(uint8_t& tappedFretNumber) const;
    bool HasTappedHarmonic() const;

// Trill Functions
    bool GetTrill(uint8_t& trilledFretNumber) const;
    bool HasTrill() const;

// Artificial Harmonic Functions
    bool GetArtificialHarmonic(uint8_t& key, uint8_t& keyVariation,
                               uint8_t& octave) const;
    bool HasArtificialHarmonic() const;
};

}

#endif // NOTE_H
