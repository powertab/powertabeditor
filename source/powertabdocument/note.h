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

#define MAX_NOTE_COMPLEX_SYMBOLS     3  ///< Maximum allowed number of complex symbols per note object
                                        // Note: needs to be #define so array works properly

#include "chordname.h"      // Needed for IsValidKeyAndVariation (Artificial Harmonics)

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

    enum flags
    {
        stringMask          = (uint8_t)0xe0,             ///< Mask used to retrieve the string
        fretNumberMask      = (uint8_t)0x1f              ///< Mask used to retrieve the fret number
    };

    enum simpleFlags
    {
        tied                        = (uint16_t)0x01,
        muted                       = (uint16_t)0x02,
        tieWrap                     = (uint16_t)0x04,     ///< Used to wrap a tie around systems
        hammerOn                    = (uint16_t)0x08,
        pullOff                     = (uint16_t)0x10,
        hammerPullFromToNowhere     = (uint16_t)0x20,
        hammerPullMask              = (uint16_t)0x38,     ///< Mask that filters out mutually exclusive hammer on/pull off flags
        naturalHarmonic             = (uint16_t)0x40,
        ghostNote                   = (uint16_t)0x80,
        octave8va                   = (uint16_t)0x100,
        octave15ma                  = (uint16_t)0x200,
        octave8vb                   = (uint16_t)0x400,
        octave15mb                  = (uint16_t)0x800,
        octaveMask                  = (uint16_t)0xf00,    ///< Mask that filters out mutually exclusive octave flags
        simpleFlagsMask             = (uint16_t)0xfff     ///< Mask that filters out all possible simple flags
    };

    enum complexSymbolTypes
    {
        slide                   = (uint8_t)'d',
        bend                    = (uint8_t)'e',
        tappedHarmonic          = (uint8_t)'f',
        trill                   = (uint8_t)'g',
        artificialHarmonic      = (uint8_t)'h',
        notUsed                 = (uint32_t)0
    };

    enum slideIntoTypes
    {
        slideIntoNone                       = (uint8_t)0x00,
        slideIntoFromBelow                  = (uint8_t)0x01,
        slideIntoFromAbove                  = (uint8_t)0x02,
        slideIntoShiftSlideUpwards          = (uint8_t)0x03,     ///< Used for wrapping shift slides around systems
        slideIntoShiftSlideDownwards        = (uint8_t)0x04,     ///< Used for wrapping shift slides around systems
        slideIntoLegatoSlideUpwards         = (uint8_t)0x05,     ///< Used for wrapping legato slides around systems
        slideIntoLegatoSlideDownwards       = (uint8_t)0x06      ///< Used for wrapping legato slides around systems
    };

    enum slideOutOfTypes
    {
        slideOutOfNone                      = (uint8_t)0x00,
        slideOutOfShiftSlide                = (uint8_t)0x01,
        slideOutOfLegatoSlide               = (uint8_t)0x02,
        slideOutOfDownwards                 = (uint8_t)0x03,
        slideOutOfUpwards                   = (uint8_t)0x04
    };

    enum slideFlags
    {
        slideIntoTypeMask       = (uint32_t)0xff0000,               ///< Mask used to retrieve the slide into type
        slideOutOfTypeMask      = (uint32_t)0xff00,                 ///< Mask used to retrieve the slide out of type
        slideOutOfStepsMask     = (uint32_t)0xff,                   ///< Mask used to retrieve the slide out of steps
    };

    enum bendTypes
    {
        normalBend              = (uint8_t)0x00,
        bendAndRelease          = (uint8_t)0x01,
        bendAndHold             = (uint8_t)0x02,
        preBend                 = (uint8_t)0x03,
        preBendAndRelease       = (uint8_t)0x04,
        preBendAndHold          = (uint8_t)0x05,
        gradualRelease          = (uint8_t)0x06,
        immediateRelease        = (uint8_t)0x07
    };

    enum bendDrawingPoints
    {
        lowPoint                = (uint8_t)0x00,
        midPoint                = (uint8_t)0x01,
        highPoint               = (uint8_t)0x02
    };

    enum bendFlags
    {
        bendTypeMask            = (uint32_t)0xf00000,               ///< Mask used to retrieve the bend type
        drawStartMask           = (uint32_t)0xc0000,                ///< Mask used to retrieve the draw start value
        drawEndMask             = (uint32_t)0x30000,                ///< Mask used to retrieve the draw end value
        bendDurationMask        = (uint32_t)0xff00,                 ///< Mask used to retrieve the bend duration
        bentPitchMask           = (uint32_t)0xf0,                   ///< Mask used to retrieve the bent pitch value
        releasePitchMask        = (uint32_t)0xf                     ///< Mask used to retrieve the release pitch value
    };

    enum artificialHarmonicOctaves
    {
        artificialHarmonicOctaveLoco            = (uint8_t)0x00,
        artificialHarmonicOctave8va             = (uint8_t)0x01,
        artificialHarmonicOctave15ma            = (uint8_t)0x02
    };

// Member Variables
protected:
    uint8_t      m_stringData;                                       ///< Top 3 bits = string, bottom 5 bits = fret number
    uint16_t      m_simpleData;						                ///< Contains simple symbol flags
    uint32_t    m_complexSymbolArray[MAX_NOTE_COMPLEX_SYMBOLS];	    ///< Complex symbol data (1 symbol per element)

// Constructor/Destructor
public:
    Note();
    Note(uint32_t string, uint8_t fretNumber);
    Note(const Note& note);
    ~Note();

// Operators
    const Note& operator=(const Note& note);
    bool operator==(const Note& note) const;
    bool operator!=(const Note& note) const;

// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream);
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    Note* CloneObject() const
            {return (new Note(*this));}

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    string GetMFCClassName() const
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

// 8va Octave Functions
    /// Sets or clears an 8va octave marker
    /// @param set True to set the 8va octave marker, false to clear it
    /// @return True if the 8va octave marker was set or cleared, false if not
    bool SetOctave8va(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(octave8va));
        return (SetSimpleFlag(octave8va));
    }
    /// Determines if the note is an 8va
    /// @return True if the note is an 8va, false if not
    bool IsOctave8va() const
        {return (IsSimpleFlagSet(octave8va));}

// 15ma Octave Functions
    /// Sets or clears an 15ma octave marker
    /// @param set True to set the 15ma octave marker, false to clear it
    /// @return True if the 15ma octave marker was set or cleared, false if not
    bool SetOctave15ma(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(octave15ma));
        return (SetSimpleFlag(octave15ma));
    }
    /// Determines if the note is an 15ma
    /// @return True if the note is an 15ma, false if not
    bool IsOctave15ma() const
        {return (IsSimpleFlagSet(octave15ma));}

// 8vb Octave Functions
    /// Sets or clears an 8vb octave marker
    /// @param set True to set the 8vb octave marker, false to clear it
    /// @return True if the 8vb octave marker was set or cleared, false if not
    bool SetOctave8vb(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(octave8vb));
        return (SetSimpleFlag(octave8vb));
    }
    /// Determines if the note is an 8vb
    /// @return True if the note is an 8vb, false if not
    bool IsOctave8vb() const
        {return (IsSimpleFlagSet(octave8vb));}

// 15mb Octave Functions
    /// Sets or clears an 15mb octave marker
    /// @param set True to set the 15mb octave marker, false to clear it
    /// @return True if the 15mb octave marker was set or cleared, false if not
    bool SetOctave15mb(bool set = true)
    {
        if (!set)
            return (ClearSimpleFlag(octave15mb));
        return (SetSimpleFlag(octave15mb));
    }
    /// Determines if the note is an 15mb
    /// @return True if the note is an 15mb, false if not
    bool IsOctave15mb() const
        {return (IsSimpleFlagSet(octave15mb));}

// Simple Flag Functions
protected:
    // Determines if a simple flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidSimpleFlag(uint16_t flag)
    {
        return (((flag & simpleFlagsMask) != 0) &&
            ((flag & ~simpleFlagsMask) == 0));
    }
    bool SetSimpleFlag(uint16_t flag);
    /// Clears a simple flag
    /// @param flag Flag to clear
    /// @return True if the flag was cleared, false if not
    bool ClearSimpleFlag(uint16_t flag)
    {
        CHECK_THAT(IsValidSimpleFlag(flag), false);
        m_simpleData &= ~flag;
        return (true);
    }
    /// Determines if a simple flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsSimpleFlagSet(uint16_t flag) const
    {
        CHECK_THAT(IsValidSimpleFlag(flag), false);
        return ((m_simpleData & flag) == flag);
    }

// Slide Functions
public:
    /// Determines if the note has a slide (either in or out)
    /// @return True if the note has a slide, false if not
    bool HasSlide() const
        {return (HasSlideInto() || HasSlideOutOf());}

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
    bool ClearSlideInto();

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
    bool ClearSlideOutOf();

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
    bool ClearBend();

// Tapped Harmonic Functions
    /// Determines if a tapped fret number is valid
    /// @param tappedFretNumber Tapped fret number to validate
    /// @return True if the tapped fret number is valid, false if not
    static bool IsValidTappedFretNumber(uint8_t tappedFretNumber)
    {
        return ((tappedFretNumber >= MIN_FRET_NUMBER) &&
            (tappedFretNumber <= MAX_FRET_NUMBER));
    }
    /// Determines if tapped harmonic data is valid
    /// @param tappedFretNumber Tapped fret number to validate
    /// @return True if the tapped harmonic data is valid, false if not
    static bool IsValidTappedHarmonic(uint8_t tappedFretNumber)
        {return (IsValidTappedFretNumber(tappedFretNumber));}
    bool SetTappedHarmonic(uint8_t tappedFretNumber);
    bool GetTappedHarmonic(uint8_t& tappedFretNumber) const;
    bool HasTappedHarmonic() const;
    bool ClearTappedHarmonic();

// Trill Functions
    /// Determines if a trilled fret number is valid
    /// @param trilledFretNumber Trilled fret number to validate
    /// @return True if the trilled fret number is valid, false if not
    static bool IsValidTrilledFretNumber(uint8_t trilledFretNumber)
    {
        return ((trilledFretNumber >= MIN_FRET_NUMBER) &&
            (trilledFretNumber <= MAX_FRET_NUMBER));
    }
    /// Determines if trill data is valid
    /// @param trilledFretNumber Trilled fret number to validate
    /// @return True if the trill data is valid, false if not
    static bool IsValidTrill(uint8_t trilledFretNumber)
        {return (IsValidTrilledFretNumber(trilledFretNumber));}
    bool SetTrill(uint8_t trilledFretNumber);
    bool GetTrill(uint8_t& trilledFretNumber) const;
    bool HasTrill() const;
    bool ClearTrill();

// Artificial Harmonic Functions
    /// Determines if a artificial octave is valid
    /// @param octave Octave to validate
    /// @return True if the artificial harmonic octave is valid, false if not
    static bool IsValidArtificialHarmonicOctave(uint8_t octave)
        {return (octave <= artificialHarmonicOctave15ma);}
    /// Determines if artificial harmonic data is valid
    /// @param key Key to validate
    /// @param keyVariation Key variation to validate
    /// @param octave Octave to validate
    /// @return True if the artificial harmonic data is valid, false if not
    static bool IsValidArtificialHarmonic(uint8_t key, uint8_t keyVariation,
        uint8_t octave)
    {
        return (ChordName::IsValidKeyAndVariation(key, keyVariation) &&
            IsValidArtificialHarmonicOctave(octave));
    }
    bool SetArtificialHarmonic(uint8_t key, uint8_t keyVariation, uint8_t octave);
    bool GetArtificialHarmonic(uint8_t& key, uint8_t& keyVariation,
        uint8_t& octave) const;
    bool HasArtificialHarmonic() const;
    bool ClearArtificialHarmonic();

// Complex Symbol Array Functions
protected:
    /// Determines if a complex symbol type is valid
    /// @param type Symbol type to validate
    /// @return True if the symbol type is valid, false if not
    static bool IsValidComplexSymbolType(uint8_t type)
    {
        return ((type == slide) || (type == bend) || (type == tappedHarmonic) ||
            (type == tappedHarmonic) || (type == trill) ||
            (type == artificialHarmonic));
    }
    bool AddComplexSymbol(uint32_t symbolData);
    size_t GetComplexSymbolCount() const;
    uint32_t FindComplexSymbol(uint8_t type) const;
    bool RemoveComplexSymbol(uint8_t type);
    void ClearComplexSymbolArrayContents();
};

#endif
