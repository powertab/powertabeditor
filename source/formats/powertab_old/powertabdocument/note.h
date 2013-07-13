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

#include <boost/array.hpp>
#include "powertabobject.h"

namespace PowerTabDocument {

class Tuning;

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
    boost::array<uint32_t, MAX_NOTE_COMPLEX_SYMBOLS> m_complexSymbolArray;    ///< Complex symbol data (1 symbol per element)

// Constructor/Destructor
public:
    Note();
    Note(uint32_t string, uint8_t fretNumber);
    Note(const Note& note);

// Operators
    const Note& operator=(const Note& note);
    bool operator==(const Note& note) const;
    bool operator!=(const Note& note) const;

// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    Note* CloneObject() const
    {
        return new Note(*this);
    }

// MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
        {return "CLineData";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
        {return ((uint16_t)1);}

// String Functions
    static bool IsValidString(uint32_t str);
    bool SetString(uint32_t str);
    uint32_t GetString() const;

// Fret Number Functions
    static bool IsValidFretNumber(uint8_t fretNumber);
    bool SetFretNumber(uint8_t fretNumber);
    uint8_t GetFretNumber() const;

// Tied Functions
    bool SetTied(bool set = true);
    bool IsTied() const;

// Muted Functions
    bool SetMuted(bool set = true);
    bool IsMuted() const;

// Tie Wrap Functions
    bool SetTieWrap(bool set = true);
    bool HasTieWrap() const;

// Hammer On Functions
    bool SetHammerOn(bool set = true);
    bool HasHammerOn() const;
    bool SetHammerOnFromNowhere(bool set = true);
    bool HasHammerOnFromNowhere() const;

// Pull Off Functions
    bool SetPullOff(bool set = true);
    bool HasPullOff() const;
    bool SetPullOffToNowhere(bool set = true);
    bool HasPullOffToNowhere() const;

    bool HasHammerOnOrPulloff() const;

// Natural Harmonic Functions
    bool SetNaturalHarmonic(bool set = true);
    bool IsNaturalHarmonic() const;

// Ghost Note Functions
    bool SetGhostNote(bool set = true);
    bool IsGhostNote() const;
    
    int GetOctaveOffset() const;
    
    bool SetOctave8va(bool set = true);
    bool IsOctave8va() const;

    bool SetOctave15ma(bool set = true);
    bool IsOctave15ma() const;

    bool SetOctave8vb(bool set = true);
    bool IsOctave8vb() const;

    bool SetOctave15mb(bool set = true);
    bool IsOctave15mb() const;

// Simple Flag Functions
private:
    static bool IsValidSimpleFlag(uint16_t flag);
    bool SetSimpleFlag(uint16_t flag);
    bool ClearSimpleFlag(uint16_t flag);
    bool IsSimpleFlagSet(uint16_t flag) const;

// Slide Functions
public:
    bool HasSlide() const;

private:
    bool HasSlideOutType(uint8_t slideType) const;

public:
    inline bool HasLegatoSlide() const { return HasSlideOutType(slideOutOfLegatoSlide); }
    inline bool HasShiftSlide() const { return HasSlideOutType(slideOutOfShiftSlide); }

// Slide Into Functions
    static bool IsValidSlideIntoType(uint8_t type);
    static bool IsValidSlideInto(uint8_t type);

    bool SetSlideInto(uint8_t type);
    bool GetSlideInto(uint8_t& type) const;
    bool HasSlideInto() const;
    void ClearSlideInto();
    bool HasSlideIntoFromAbove() const;
    bool HasSlideIntoFromBelow() const;

// Slide Out Of Functions
    static bool IsValidSlideOutOfType(uint8_t type);
    static bool IsValidSlideOutOf(uint8_t type);

    bool SetSlideOutOf(uint8_t type, int8_t steps);
    bool GetSlideOutOf(uint8_t& type, int8_t& steps) const;
    bool HasSlideOutOf() const;
    bool HasSlideOutOfDownwards() const;
    bool HasSlideOutOfUpwards() const;
    void ClearSlideOutOf();

// Bend Functions
    static bool IsValidBend(uint8_t type, uint8_t bentPitch, uint8_t releasePitch,
                            uint8_t duration, uint8_t drawStartPoint, uint8_t drawEndPoint);

    static bool IsValidBendType(uint8_t type);
    static bool IsValidBentPitch(uint8_t bentPitch);
    static bool IsValidReleasePitch(uint8_t releasePitch);
    static bool IsValidBendDuration(uint8_t duration);
    static bool IsValidDrawStartPoint(uint8_t drawStartPoint);
    static bool IsValidDrawEndPoint(uint8_t drawEndPoint);

    bool SetBend(uint8_t type, uint8_t bentPitch, uint8_t releasePitch,
                 uint8_t duration, uint8_t drawStartPoint, uint8_t drawEndPoint);
    bool GetBend(uint8_t& type, uint8_t& bentPitch, uint8_t& releasePitch,
                 uint8_t& duration, uint8_t& drawStartPoint, uint8_t& drawEndPoint) const;
    bool HasBend() const;
    void ClearBend();

    static std::string GetBendText(uint8_t pitch);

// Tapped Harmonic Functions
    static bool IsValidTappedFretNumber(uint8_t tappedFretNumber);
    static bool IsValidTappedHarmonic(uint8_t tappedFretNumber);
    
    bool SetTappedHarmonic(uint8_t tappedFretNumber);
    bool GetTappedHarmonic(uint8_t& tappedFretNumber) const;
    bool HasTappedHarmonic() const;
    void ClearTappedHarmonic();

// Trill Functions
    static bool IsValidTrilledFretNumber(uint8_t trilledFretNumber);
    bool IsValidTrill(uint8_t trilledFretNumber) const;
    
    bool SetTrill(uint8_t trilledFretNumber);
    bool GetTrill(uint8_t& trilledFretNumber) const;
    bool HasTrill() const;
    void ClearTrill();

// Artificial Harmonic Functions
    static bool IsValidArtificialHarmonicOctave(uint8_t octave);
    static bool IsValidArtificialHarmonic(uint8_t key, uint8_t keyVariation,
                                          uint8_t octave);
    
    bool SetArtificialHarmonic(uint8_t key, uint8_t keyVariation, uint8_t octave);
    bool GetArtificialHarmonic(uint8_t& key, uint8_t& keyVariation, uint8_t& octave) const;
    bool HasArtificialHarmonic() const;
    void ClearArtificialHarmonic();

    std::string GetText() const;

    uint8_t GetPitch(const Tuning& tuning, bool includeOffset) const;

// Complex Symbol Array Functions
private:
    static bool IsValidComplexSymbolType(uint8_t type);
};

}

#endif // NOTE_H
