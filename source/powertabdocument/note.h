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

#ifndef __NOTE_H__
#define __NOTE_H__

#include <array>
#include "powertabobject.h"

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
protected:
    uint8_t      m_stringData;                                       ///< Top 3 bits = string, bottom 5 bits = fret number
    uint16_t      m_simpleData;						                ///< Contains simple symbol flags
    std::array<uint32_t, MAX_NOTE_COMPLEX_SYMBOLS> m_complexSymbolArray;    ///< Complex symbol data (1 symbol per element)

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
            {return (new Note(*this));}

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
        {return "CLineData";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
        {return ((uint16_t)1);}

// String Functions
    /// Determines if a string is valid
    /// @param string String to validate
    /// @return True if the string is valid, false if not
        static bool IsValidString(uint32_t str)
                {return (str <= MAX_STRING);}
        bool SetString(uint32_t str);
    uint32_t GetString() const;

// Fret Number Functions
    /// Determines if a fret number is valid
    /// @param fretNumber Fret number to validate
    /// @return True if the fret number is valid, false if not
        static bool IsValidFretNumber(uint8_t fretNumber)
        {return (fretNumber <= MAX_FRET_NUMBER);}
    bool SetFretNumber(uint8_t fretNumber);
    uint8_t GetFretNumber() const;

// Tied Functions
    /// Sets or clears a tie
    /// @param set True to set the tie, false to clear it
    /// @return True if the tie was set or cleared, false if not
    bool SetTied(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(tied));
        return (SetSimpleFlag(tied));
    }
    /// Determines if the note is tied
    /// @return True if the note is tied, false if not
    bool IsTied() const
        {return (IsSimpleFlagSet(tied));}

// Muted Functions
    /// Sets or clears the muted effect
    /// @param set True to set the muted effect, false to clear it
    /// @return True if the muted effect was set or cleared, false if not
    bool SetMuted(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(muted));
        return (SetSimpleFlag(muted));
    }
    /// Determines if the note is muted
    /// @return True if the note is muted, false if not
    bool IsMuted() const
        {return (IsSimpleFlagSet(muted));}

// Tie Wrap Functions
    /// Sets or clears a tie wrap
    /// @param set True to set the tie wrap, false to clear it
    /// @return True if the tie wrap was set or cleared, false if not
    bool SetTieWrap(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(tieWrap));
        return (SetSimpleFlag(tieWrap));
    }
    /// Determines if the note has a tie wrap
    /// @return True if the note has a tie wrap, false if not
    bool HasTieWrap() const
        {return (IsSimpleFlagSet(tieWrap));}

// Hammer On Functions
    /// Sets or clears a hammer on
    /// @param set True to set the hammer on, false to clear it
    /// @return True if the hammer on was set or cleared, false if not
    bool SetHammerOn(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(hammerPullMask));
        return (SetSimpleFlag(hammerOn) &&
            ClearSimpleFlag(hammerPullFromToNowhere));
    }
    /// Determines if the note has a hammer on
    /// @return True if the note has a hammer on, false if not
    bool HasHammerOn() const
    {
        return (IsSimpleFlagSet(hammerOn) &&
            !IsSimpleFlagSet(hammerPullFromToNowhere));
    }
    /// Sets or clears a hammer on from nowhere
    /// @param set True to set the hammer on from nowhere, false to clear it
    /// @return True if the hammer on from nowhere was set or cleared, false if
    /// not
    bool SetHammerOnFromNowhere(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(hammerPullMask));
        return (SetSimpleFlag(hammerOn | hammerPullFromToNowhere));
    }
    /// Determines if the note has a hammer on from nowhere
    /// @return True if the note has a hammer on from nowhere, false if not
    bool HasHammerOnFromNowhere() const
        {return (IsSimpleFlagSet(hammerOn | hammerPullFromToNowhere));}

// Pull Off Functions
    /// Sets or clears a pull off
    /// @param set True to set the pull off, false to clear it
    /// @return True if the pull off was set or cleared, false if not
    bool SetPullOff(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(hammerPullMask));
        return (SetSimpleFlag(pullOff) &&
            ClearSimpleFlag(hammerPullFromToNowhere));
    }
    /// Determines if the note has a pull off
    /// @return True if the note has a pull off, false if not
    bool HasPullOff() const
    {
        return (IsSimpleFlagSet(pullOff) &&
            !IsSimpleFlagSet(hammerPullFromToNowhere));
    }
    /// Sets or clears a pull off to nowhere
    /// @param set True to set the pull off to nowhere, false to clear it
    /// @return True if the pull off to nowhere was set or cleared, false if not
    bool SetPullOffToNowhere(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(hammerPullMask));
        return (SetSimpleFlag(pullOff | hammerPullFromToNowhere));
    }
    /// Determines if the note has a pull off to nowhere
    /// @return True if the note has a pull off to nowhere, false if not
    bool HasPullOffToNowhere() const
    {
        return (IsSimpleFlagSet(pullOff) &&
            IsSimpleFlagSet(hammerPullFromToNowhere));
    }
    
    /// Determines if the note has either a hammer-on or a pull-off
    /// @return True if the note has either a hammer-on or a pull-off
    bool HasHammerOnOrPulloff() const
    {
        return (HasHammerOn() || HasPullOff());
    }

// Natural Harmonic Functions
    /// Sets or clears a natural harmonic
    /// @param set True to set the natural harmonic, false to clear it
    /// @return True if the natural harmonic was set or cleared, false if not
    bool SetNaturalHarmonic(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(naturalHarmonic));
        return (SetSimpleFlag(naturalHarmonic));
    }
    /// Determines if the note is a natural harmonic
    /// @return True if the note is a natural harmonic, false if not
    bool IsNaturalHarmonic() const
        {return (IsSimpleFlagSet(naturalHarmonic));}

// Ghost Note Functions
    /// Sets or clears a ghost note
    /// @param set True to set the ghost note, false to clear it
    /// @return True if the ghost note was set or cleared, false if not
    bool SetGhostNote(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(ghostNote));
        return (SetSimpleFlag(ghostNote));
    }
    /// Determines if the note is a ghost note
    /// @return True if the note is a ghost note, false if not
    bool IsGhostNote() const
        {return (IsSimpleFlagSet(ghostNote));}
    
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
protected:
    static bool IsValidSimpleFlag(uint16_t flag);
    bool SetSimpleFlag(uint16_t flag);
    bool ClearSimpleFlag(uint16_t flag);
    bool IsSimpleFlagSet(uint16_t flag) const;

// Slide Functions
public:
    /// Determines if the note has a slide (either in or out)
    /// @return True if the note has a slide, false if not
    bool HasSlide() const
        {return (HasSlideInto() || HasSlideOutOf());}

protected:
    bool HasSlideOutType(uint8_t slideType) const;

public:
    inline bool HasLegatoSlide() const { return HasSlideOutType(slideOutOfLegatoSlide); }
    inline bool HasShiftSlide() const { return HasSlideOutType(slideOutOfShiftSlide); }

// Slide Into Functions
    /// Determines if a slide into type is valid
    /// @param type Type to validate
    /// @return True if the slide into type is valid, false if not
    static bool IsValidSlideIntoType(uint8_t type)
        {return (type <= slideIntoLegatoSlideDownwards);}
    /// Determines if slide into data is valid
    /// @param type Type to validate
    /// @return True if slide into data is valid, false if not
    static bool IsValidSlideInto(uint8_t type)
        {return (IsValidSlideIntoType(type));}
    bool SetSlideInto(uint8_t type);
    bool GetSlideInto(uint8_t& type) const;
    bool HasSlideInto() const;
    void ClearSlideInto();
    bool HasSlideIntoFromAbove() const;
    bool HasSlideIntoFromBelow() const;

// Slide Out Of Functions
    /// Determines if a slide out of type is valid
    /// @param type Type to validate
    /// @return True if the slide out of type is valid, false if not
    static bool IsValidSlideOutOfType(uint8_t type)
        {return (type <= slideOutOfUpwards);}
    /// Determines if slide out of data is valid
    /// @param type Type to validate
    /// @return True if the slide out of data is valid, false if not
    static bool IsValidSlideOutOf(uint8_t type)
        {return (IsValidSlideOutOfType(type));}
    bool SetSlideOutOf(uint8_t type, int8_t steps);
    bool GetSlideOutOf(uint8_t& type, int8_t& steps) const;
    bool HasSlideOutOf() const;
    void ClearSlideOutOf();

    inline bool HasSlideOutOfDownwards() const
    {
        uint8_t type = 0;
        int8_t steps = 0;
        GetSlideOutOf(type, steps);
        return type == slideOutOfDownwards;
    }
    inline bool HasSlideOutOfUpwards() const
    {
        uint8_t type = 0;
        int8_t steps = 0;
        GetSlideOutOf(type, steps);
        return type == slideOutOfUpwards;
    }

// Bend Functions
    static bool IsValidBend(uint8_t type, uint8_t bentPitch, uint8_t releasePitch,
        uint8_t duration, uint8_t drawStartPoint, uint8_t drawEndPoint);
    /// Determines if a bend type is valid
    /// @param type Bend type to validate
    /// @return True if the bend type is valid, false if not
    static bool IsValidBendType(uint8_t type)
        {return (type <= immediateRelease);}
    /// Determines if a bent pitch is valid
    /// @param bentPitch Bent pitch to validate
    /// @return True if the bent pitch is valid, false if not
    static bool IsValidBentPitch(uint8_t bentPitch)
        {return (bentPitch <= MAX_BEND_PITCH);}
    /// Determines if a release pitch is valid
    /// @param releasePitch Release pitch to validate
    /// @return True if the release pitch is valid, false if not
    static bool IsValidReleasePitch(uint8_t releasePitch)
        {return (releasePitch <= MAX_BEND_PITCH);}
    /// Determines if a bend duration is valid
    /// @param duration Duration to validate
    /// @return True if the duration is valid, false if not
    static bool IsValidBendDuration(uint8_t duration)
        {return (duration <= MAX_BEND_DURATION);}
    /// Determines if a draw start point is valid
    /// @param drawStartPoint Draw start point to validate
    /// @return True if the draw start point is valid, false if not
    static bool IsValidDrawStartPoint(uint8_t drawStartPoint)
        {return (drawStartPoint <= highPoint);}
    /// Determines if a draw end point is valid
    /// @param drawEndPoint Draw end point to validate
    /// @return True if the draw end point is valid, false if not
    static bool IsValidDrawEndPoint(uint8_t drawEndPoint)
        {return (drawEndPoint <= highPoint);}
    bool SetBend(uint8_t type, uint8_t bentPitch, uint8_t releasePitch,
        uint8_t duration, uint8_t drawStartPoint, uint8_t drawEndPoint);
    bool GetBend(uint8_t& type, uint8_t& bentPitch, uint8_t& releasePitch,
        uint8_t& duration, uint8_t& drawStartPoint, uint8_t& drawEndPoint) const;
    bool HasBend() const;
    void ClearBend();

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
    bool GetArtificialHarmonic(uint8_t& key, uint8_t& keyVariation,
        uint8_t& octave) const;
    bool HasArtificialHarmonic() const;
    void ClearArtificialHarmonic();

    std::string GetText() const;

    uint8_t GetPitch(const Tuning& tuning) const;

// Complex Symbol Array Functions
private:
    static bool IsValidComplexSymbolType(uint8_t type);
};

#endif
