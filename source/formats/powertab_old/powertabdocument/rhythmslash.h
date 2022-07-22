/////////////////////////////////////////////////////////////////////////////
// Name:            rhythmslash.h
// Purpose:         Stores and renders rhythm slashes
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 17, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef RHYTHMSLASH_H
#define RHYTHMSLASH_H

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders rhythm slashes
class RhythmSlash : public PowerTabObject
{
// Constants
public:
    // Default Constants
    static const uint8_t     DEFAULT_POSITION;                   ///< Default value for the position member variable
    static const uint8_t     DEFAULT_BEAMING;                    ///< Default value for the beaming member variable
    static const uint32_t   DEFAULT_DATA;                       ///< Default value for the data member variable
    static const uint8_t     DEFAULT_DURATION_TYPE;              ///< Default duration type
    
    // Position Constants
    static const uint32_t   MIN_POSITION;                       ///< Minimum allowed value for the position member variable
    static const uint32_t   MAX_POSITION;                       ///< Maximum allowed value for the position member variable
    
    // Single Note String Number Constants
    static const uint8_t     MIN_SINGLE_NOTE_STRING_NUMBER;      ///< Minimum allowed value for the single note string number
    static const uint8_t     MAX_SINGLE_NOTE_STRING_NUMBER;      ///< Maximum allowed value for the single note string number
    
    // Single Note Fret Number Constants
    static const uint8_t     MIN_SINGLE_NOTE_FRET_NUMBER;        ///< Minimum allowed value for the single note fret number
    static const uint8_t     MAX_SINGLE_NOTE_FRET_NUMBER;        ///< Maximum allowed value for the single note fret number
    
    enum beamingFlags
    {
        previousBeamDuration8th         = (uint8_t)0x01,         ///< Previous rhythm slash in beam group is an 8th note
        previousBeamDuration16th        = (uint8_t)0x02,         ///< Previous rhythm slash in beam group is a 16th note
        previousBeamDurationTypeMask    = (uint8_t)0x03,         ///< Mask that filters out mututally exclusive beam duration flags
        
        beamStart           = (uint8_t)0x04,                     ///< Start of a beamed group
        beamFractional      = (uint8_t)0x08,                     ///< Rhythm slash has a fractional beam
        beamEnd             = (uint8_t)0x10,                     ///< End of a beamed group
        beamTypeMask        = (uint8_t)0x1c,                     ///< Mask that filters out mutually exclusive beam type flags
        
        beamingFlagsMask    = (uint8_t)0x1f,                     ///< Mask that filters out all possible beaming flags (previous beam duration + beam type)
        
        tripletStart        = (uint8_t)0x20,                     ///< Start of a triplet group
        tripletMiddle       = (uint8_t)0x40,                     ///< Middle of a triplet group
        tripletEnd          = (uint8_t)0x80,                     ///< End of a triplet group
        tripletMask         = (uint8_t)0xe0                      ///< Mask that filters out all possible triplet flags
    };

    enum dataFlags
    {
        dotted                  = (uint32_t)0x01,               ///< Duration is dotted
        doubleDotted            = (uint32_t)0x02,               ///< Duration is double dotted
        dottedMask              = (uint32_t)0x03,               ///< Mask that filters out mutually exclusive possible dot flags
        
        rest                    = (uint32_t)0x04,               ///< Rhythm slash is a rest
        tied                    = (uint32_t)0x08,               ///< Rhythm slash is tied to previous slash
        muted                   = (uint32_t)0x10,               ///< Rhythm slash is muted "x"
        staccato                = (uint32_t)0x20,               ///< Rhythm slash is staccatoed
        singleNote              = (uint32_t)0x40,               ///< Rhythm slash is a single note
        
        pickStrokeUp            = (uint32_t)0x80,               ///< Pickstroke up symbol
        pickStrokeDown          = (uint32_t)0x100,              ///< Pickstroke down symbol
        pickStrokeMask          = (uint32_t)0x180,              ///< Mask that filters out mutually exclusive pickstroke flags
        
        arpeggioUp              = (uint32_t)0x200,              ///< Arpeggio up symbol
        arpeggioDown            = (uint32_t)0x400,              ///< Arpeggio down symbol
        arpeggioMask            = (uint32_t)0x600,              ///< Mask that filters out mutually exclusive arpeggio flags
        
        tripletFeel1st          = (uint32_t)0x800,              ///< Indicates the slash should be played as if it were the 1st note in a triplet feel duet
        tripletFeel2nd          = (uint32_t)0x1000,             ///< Indicates the slash should be played as if it were the 2nd note in a triplet feel duet
        tripletFeelMask         = (uint32_t)0x1800,             ///< Mask that filters out mutually exclusive triplet feel flags
        
        marcato                 = (uint32_t)0x2000,             ///< Standard accent
        sforzando               = (uint32_t)0x4000,             ///< Heavy accent
        accentMask              = (uint32_t)0x6000,             ///< Mask that filters out mutually exclusive accent flags
        
        slideIntoFromAbove      = (uint32_t)0x8000,             ///< Slide into from a higher fret position
        slideIntoFromBelow      = (uint32_t)0x10000,            ///< Slide into from a lower fret position
        slideIntoMask           = (uint32_t)0x18000,            ///< Mask that filters out mutually exclusive slide into flags
        
        slideOutOfDownwards     = (uint32_t)0x20000,            ///< Slide out of to a lower fret position
        slideOutOfUpwards       = (uint32_t)0x40000,            ///< Slide out of to a higher fret position
        slideOutOfMask          = (uint32_t)0x60000,            ///< Mask that filters out mutually exclusive slide out flags
        
        dataFlagsMask           = (uint32_t)0x7ffff,            ///< Mask that filters out all valid data flags
        
        durationTypeMask        = (uint32_t)0xe00000,           ///< Mask used to retrieve the duration type
        stringNumberMask        = (uint32_t)0x7000000,          ///< Mask used to retrieve the single note string number
        fretNumberMask          = (uint32_t)0xf8000000,         ///< Mask used to retrieve the single note fret number
        singleNoteDataMask      = (uint32_t)0xff000000          ///< Mask that filters out the single note data (string number + fret number)
    };

// Member Variables
private:
    uint8_t          m_position;         ///< Zero-based index of the position within the system where the rhythm slash is anchored
    uint8_t          m_beaming;          ///< Beaming and triplet data (top 3 bits = triplet flags, bottom 5 bits = beaming flags)
    uint32_t        m_data;             ///< Top byte = single note data, next 3 bits = duration type, remaining bits are flags

// Constructor/Destructor
public:
    RhythmSlash();
    RhythmSlash(uint32_t position, uint8_t durationType, uint8_t dotCount = 0);
    RhythmSlash(const RhythmSlash& rhythmSlash);
    ~RhythmSlash();

// Operators
    const RhythmSlash& operator=(const RhythmSlash& rhythmSlash);
    bool operator==(const RhythmSlash& rhythmSlash) const;
    bool operator!=(const RhythmSlash& rhythmSlash) const;

// Serialization functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

// MFC Class Functions
public:    
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CRhythmSlash";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}
    
// Position Functions
    /// Determines whether a position is valid
    /// @param position Position to validate
    /// @return True if the position is valid, false if not
    static bool IsValidPosition(uint32_t position)
        {return ((position >= MIN_POSITION) && (position <= MAX_POSITION));}
    /// Sets the position within the system where the rhythm slash is anchored
    /// @param position Zero-based index within the system where the rhythm
    /// slash is anchored
    /// @return True if the position was set, false if not
    bool SetPosition(uint32_t position)
    {
        PTB_CHECK_THAT(IsValidPosition(position), false);
        m_position = (uint8_t)position;
        return (true);
    }
    /// Gets the position within the system where the rhythm slash is anchored
    /// @return The position within the system where the rhythm slash is
    /// anchored
    uint32_t GetPosition() const                           
        {return (m_position);}
    
// Duration Type Functions
    /// Determines if a duration type is valid
    /// @param durationType Duration type to validate
    /// @return True if the duration type is valid, false if not
    static bool IsValidDurationType(uint8_t durationType)
    {
        return ((durationType == 1) || (durationType == 2) ||
            (durationType == 4) || (durationType == 8) || (durationType == 16));
    }
    bool SetDurationType(uint8_t durationType);
    uint8_t GetDurationType() const;
        
// Previous Beam Duration Functions
    /// Determines if a previous beam duration type is valid
    /// @param durationType Duration type to validate
    /// @return True if the duration type is valid, false if not
    static bool IsValidPreviousBeamDurationType(uint8_t durationType)
    {
        return ((durationType == 0) || (durationType == 8) ||
            (durationType == 16));
    }
    bool SetPreviousBeamDurationType(uint8_t durationType);
    uint8_t GetPreviousBeamDurationType() const;
    
// Beam Functions
    /// Sets or clears the beam start value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetBeamStart(bool set = true)
    {
        if (!set)
            return (ClearBeamingFlag(beamStart));
        return (SetBeamingFlag(beamStart));
    }
    /// Determines if the rhythm slash is the start of a beam grouping
    /// @return True if the rhythm slash is the start of a beam grouping, false
    /// if not
    bool IsBeamStart() const
        {return (IsBeamingFlagSet(beamStart));}
    /// Sets or clears the fractional beam value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetFractionalBeam(bool set = true)
    {
        if (!set)
            return (ClearBeamingFlag(beamFractional));
        return (SetBeamingFlag(beamFractional));
    }
    /// Determines if the rhythm slash has a fractional beam
    /// @return True if the rhythm slash has a fractional beam, false if not
    bool HasFractionalBeam() const
        {return (IsBeamingFlagSet(beamFractional));}
    /// Sets or clears the beam end value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetBeamEnd(bool set = true)
    {
        if (!set)
            return (ClearBeamingFlag(beamEnd));
        return (SetBeamingFlag(beamEnd));
    }
    /// Determines if the rhythm slash is the end of a beam grouping
    /// @return True if the rhythm slash is the end of a beam grouping, false if
    /// not
    bool IsBeamEnd() const
        {return (IsBeamingFlagSet(beamEnd));}
    /// Clears all beaming data
    /// @return True if the beaming data was cleared, false if not
    bool ClearBeam()
        {return (ClearBeamingFlag(beamingFlagsMask));}
                
private:
    /// Determines if a beaming flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidBeamingFlag(uint8_t flag)                 
    {
        return (((flag & beamingFlagsMask) != 0) &&
            ((flag & ~beamingFlagsMask) == 0));
    }
    bool SetBeamingFlag(uint8_t flag);
    /// Clears a beaming flag
    /// @param flag Flag to clear
    /// @return True if the beaming flag was cleared, false if not
    bool ClearBeamingFlag(uint8_t flag)                          
    {
        PTB_CHECK_THAT(IsValidBeamingFlag(flag), false);
        m_beaming &= ~flag;
        return (true);
    }
    /// Determines if a beaming flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsBeamingFlagSet(uint8_t flag) const                    
    {
        PTB_CHECK_THAT(IsValidBeamingFlag(flag), false);
        return ((m_beaming & flag) == flag);
    }

// Triplet Functions
public:
    /// Sets or clears the triplet start value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetTripletStart(bool set = true)
    {
        if (!set)
            return (ClearTripletFlag(tripletStart));
        return (SetTripletFlag(tripletStart));
    }
    /// Determines if the rhythm slash is the start of a triplet grouping
    /// @return True if the rhythm slash is the start of a triplet grouping,
    /// false if not
    bool IsTripletStart() const
        {return (IsTripletFlagSet(tripletStart));}
    /// Sets or clears the triplet middle value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetTripletMiddle(bool set = true)
    {
        if (!set)
            return (ClearTripletFlag(tripletMiddle));
        return (SetTripletFlag(tripletMiddle));
    }
    /// Determines if the rhythm slash is the middle of a triplet grouping
    /// @return True if the rhythm slash is the middle of a triplet grouping,
    /// false if not
    bool IsTripletMiddle() const
        {return (IsTripletFlagSet(tripletMiddle));}
    /// Sets or clears the triplet end value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetTripletEnd(bool set = true)
    {
        if (!set)
            return (ClearTripletFlag(tripletEnd));
        return (SetTripletFlag(tripletEnd));
    }
    /// Determines if the rhythm slash is the end of a triplet grouping
    /// @return True if the rhythm slash is the end of a triplet grouping, false
    /// if not
    bool IsTripletEnd() const
        {return (IsTripletFlagSet(tripletEnd));}
    /// Clears any triplet data
    /// @return True if triplet data were cleared, false if not
    bool ClearTriplet()
        {return (ClearTripletFlag(tripletMask));}
    
private:
    /// Determines if a triplet flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidTripletFlag(uint8_t flag)                 
        {return (((flag & tripletMask) != 0) && ((flag & ~tripletMask) == 0));}
    bool SetTripletFlag(uint8_t flag);
    /// Clears a triplet flag
    /// @param flag Flag to clear
    /// @return True if the triplet flag was cleared, false if not
    bool ClearTripletFlag(uint8_t flag)                          
    {
        PTB_CHECK_THAT(IsValidTripletFlag(flag), false);
        m_beaming &= ~flag;
        return (true);
    }
    /// Determines if a triplet flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsTripletFlagSet(uint8_t flag) const                    
    {
        PTB_CHECK_THAT(IsValidTripletFlag(flag), false);
        return ((m_beaming & flag) == flag);
    }

// Dotted Functions
public:
    /// Sets or clears the dotted duration state
    /// @param set True sets the dotted state, false clears it
    /// @return True if the dotted state was set or cleared, false if not
    bool SetDotted(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(dotted));
        return (SetDataFlag(dotted));
    }
    /// Determines if the rhythm slash's duration is dotted
    /// @return True if the rhythm slash's duration is dotted, false if not
    bool IsDotted() const                       
        {return (IsDataFlagSet(dotted));}
    
    // Double Dotted Functions
    /// Sets or clears the double dotted duration state
    /// @param set True sets the double dotted state, false clears it
    /// @return True if the double dotted state was set or cleared, false if not
    bool SetDoubleDotted(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(doubleDotted));
        return (SetDataFlag(doubleDotted));
    }
    /// Determines if the rhythm slash's duration is double dotted
    /// @return True if the rhythm slash's duration is double dotted, false if
    /// not
    bool IsDoubleDotted() const                 
        {return (IsDataFlagSet(doubleDotted));}
        
// Rest Functions
    /// Sets or clears the rest state
    /// @param set True sets the rest state, false clears it
    /// @return True if the rest state was set or cleared, false if not
    bool SetRest(bool set = true)
        {if (!set) return (ClearDataFlag(rest)); return (SetDataFlag(rest));}
    /// Determines if the rhythm slash is a rest
    /// @return True if the rhythm slash is a rest, false if not
    bool IsRest() const                 
        {return (IsDataFlagSet(rest));}

// Tied Functions
    /// Sets or clears the tied state
    /// @param set True sets the tied state, false clears it
    /// @return True if the tied state was set or cleared, false if not
    bool SetTied(bool set = true)
        {if (!set) return (ClearDataFlag(tied)); return (SetDataFlag(tied));}
    /// Determines if the rhythm slash is a tied (to the previous slash)
    /// @return True if the rhythm slash is a tied, false if not
    bool IsTied() const                 
        {return (IsDataFlagSet(tied));}

// Muted Functions
    /// Sets or clears the muted state
    /// @param set True sets the muted state, false clears it
    /// @return True if the muted state was set or cleared, false if not
    bool SetMuted(bool set = true)
        {if (!set) return (ClearDataFlag(muted)); return (SetDataFlag(muted));}
    /// Determines if the rhythm slash is a muted
    /// @return True if the rhythm slash is a muted, false if not
    bool IsMuted() const                 
        {return (IsDataFlagSet(muted));}
    
// Staccato Functions
    /// Sets or clears the staccato state
    /// @param set True sets the staccato state, false clears it
    /// @return True if the staccato state was set or cleared, false if not
    bool SetStaccato(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(staccato));
        return (SetDataFlag(staccato));
    }
    /// Determines if the rhythm slash is staccatoed
    /// @return True if the rhythm slash is staccatoed, false if not
    bool IsStaccato() const                 
        {return (IsDataFlagSet(staccato));}
                    
// Pick Stroke Up Functions
    /// Sets or clears the pick stroke up state
    /// @param set True sets the pick stroke up state, false clears it
    /// @return True if the pick stroke up state was set or cleared, false if
    /// not
    bool SetPickStrokeUp(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(pickStrokeUp));
        return (SetDataFlag(pickStrokeUp));
    }
    /// Determines if the rhythm slash has a pick stroke up symbol
    /// @return True if the rhythm slash has a pick stroke up symbol, false if
    /// not
    bool HasPickStrokeUp() const
        {return (IsDataFlagSet(pickStrokeUp));}

// Pick Stroke Down Functions
    /// Sets or clears the pick stroke down state
    /// @param set True sets the pick stroke down state, false clears it
    /// @return True if the pick stroke down state was set or cleared, false if
    /// not
    bool SetPickStrokeDown(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(pickStrokeDown));
        return (SetDataFlag(pickStrokeDown));
    }
    /// Determines if the rhythm slash has a pick stroke down symbol
    /// @return True if the rhythm slash has a pick stroke down symbol, false if
    /// not
    bool HasPickStrokeDown() const
        {return (IsDataFlagSet(pickStrokeDown));}
    
// Arpeggio Up Functions
    /// Sets or clears the arpeggio up symbol
    /// @param set True sets the arpeggio up symbol, false clears it
    /// @return True if the arpeggio up symbol was set or cleared, false if not
    bool SetArpeggioUp(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(arpeggioUp));
        return (SetDataFlag(arpeggioUp));
    }
    /// Determines if the rhythm slash has an arpeggio up symbol
    /// @return True if the rhythm slash has an arpeggio up symbol, false if not
    bool HasArpeggioUp() const
        {return (IsDataFlagSet(arpeggioUp));}

// Arpeggio Down Functions
    /// Sets or clears the arpeggio down symbol
    /// @param set True sets the arpeggio down symbol, false clears it
    /// @return True if the arpeggio down symbol was set or cleared, false if
    /// not
    bool SetArpeggioDown(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(arpeggioDown));
        return (SetDataFlag(arpeggioDown));
    }
    /// Determines if the rhythm slash has an arpeggio down symbol
    /// @return True if the rhythm slash has an arpeggio down symbol, false if
    /// not
    bool HasArpeggioDown() const
        {return (IsDataFlagSet(arpeggioDown));}

// Triplet Feel 1st Functions
    /// Sets or clears the 1st triplet feel effect (played as if it was the 1st
    /// note of two 'triplet feel' notes)
    /// @param set True sets the triplet feel effect, false clears it
    /// @return True if the triplet feel effect was set or cleared, false if not
    bool SetTripletFeel1st(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(tripletFeel1st));
        return (SetDataFlag(tripletFeel1st));
    }
    /// Determines if the rhythm slash uses the 1st triplet feel effect (played
    /// as if it was the 1st note of two 'triplet feel' notes)
    /// @return True if the rhythm slash uses the 1st triplet feel effect, false
    /// if not
    bool IsTripletFeel1st() const
        {return (IsDataFlagSet(tripletFeel1st));}

// Triplet Feel 2nd Functions
    /// Sets or clears the 2nd triplet feel effect (played as if it was the 2nd
    /// note of two 'triplet feel' notes)
    /// @param set True sets the triplet feel effect, false clears it
    /// @return True if the triplet feel effect was set or cleared, false if not
    bool SetTripletFeel2nd(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(tripletFeel2nd));
        return (SetDataFlag(tripletFeel2nd));
    }
    /// Determines if the rhythm slash uses the 2nd triplet feel effect (played
    /// as if it was the 1st note of two 'triplet feel' notes)
    /// @return True if the rhythm slash uses the 2nd triplet feel effect, false
    /// if not
    bool IsTripletFeel2nd() const
        {return (IsDataFlagSet(tripletFeel2nd));}
        
// Marcato Functions
    /// Sets or clears the marcato symbol (standard accent)
    /// @param set True sets the marcato symbol, false clears it
    /// @return True if the marcato symbol was set or cleared, false if not
    bool SetMarcato(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(marcato));
        return (SetDataFlag(marcato));
    }
    /// Determines if the rhythm slash has a marcato (standard accent)
    /// @return True if the rhythm slash has a marcato, false if not
    bool HasMarcato() const
        {return (IsDataFlagSet(marcato));}
    
// Sforzando Functions
    /// Sets or clears the sforzando symbol (heavy accent)
    /// @param set True sets the sforzando symbol, false clears it
    /// @return True if the sforzando symbol was set or cleared, false if not
    bool SetSforzando(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(sforzando));
        return (SetDataFlag(sforzando));
    }
    /// Determines if the rhythm slash has a sforzando (heavy accent)
    /// @return True if the rhythm slash has a sforzando, false if not
    bool HasSforzando() const
        {return (IsDataFlagSet(sforzando));}

// Slide Into From Above Functions
    /// Sets or clears the slide into from above symbol
    /// @param set True sets the slide into from above symbol, false clears it
    /// @return True if the slide into from above symbol was set or cleared,
    /// false if not
    bool SetSlideIntoFromAbove(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(slideIntoFromAbove));
        return (SetDataFlag(slideIntoFromAbove));
    }
    /// Determines if the rhythm slash has a slide into from above
    /// @return True if the rhythm slash has a slide into from above, false if
    /// not
    bool HasSlideIntoFromAbove() const
        {return (IsDataFlagSet(slideIntoFromAbove));}

// Slide Into From Below Functions
    /// Sets or clears the slide into from below symbol
    /// @param set True sets the slide into from below symbol, false clears it
    /// @return True if the slide into from below symbol was set or cleared,
    /// false if not
    bool SetSlideIntoFromBelow(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(slideIntoFromBelow));
        return (SetDataFlag(slideIntoFromBelow));
    }
    /// Determines if the rhythm slash has a slide into from below
    /// @return True if the rhythm slash has a slide into from below, false if
    /// not
    bool HasSlideIntoFromBelow() const
        {return (IsDataFlagSet(slideIntoFromBelow));}
        
// Slide Out Of Downwards Functions
    /// Sets or clears the slide out of downwards symbol
    /// @param set True sets the slide out of downwards symbol, false clears it
    /// @return True if the slide out of downwards symbol was set or cleared,
    /// false if not
    bool SetSlideOutOfDownwards(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(slideOutOfDownwards));
        return (SetDataFlag(slideOutOfDownwards));
    }
    /// Determines if the rhythm slash has a slide out of downwards
    /// @return True if the rhythm slash has a slide out of downwards, false if
    /// not
    bool HasSlideOutOfDownwards() const
        {return (IsDataFlagSet(slideOutOfDownwards));}

// Slide Out Of Upwards Functions
    /// Sets or clears the slide out of upwards symbol
    /// @param set True sets the slide out of upwards symbol, false clears it
    /// @return True if the slide out of upwards symbol was set or cleared,
    /// false if not
    bool SetSlideOutOfUpwards(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(slideOutOfUpwards));
        return (SetDataFlag(slideOutOfUpwards));
    }
    /// Determines if the rhythm slash has a slide out of upwards
    /// @return True if the rhythm slash has a slide out of upwards, false if
    /// not
    bool HasSlideOutOfUpwards() const
        {return (IsDataFlagSet(slideOutOfUpwards));}

// Flag Functions
private:
    /// Determines if a data flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidDataFlag(uint32_t flag)
    {
        return (((flag & dataFlagsMask) != 0) &&
            ((flag & ~dataFlagsMask) == 0));
    }
    bool SetDataFlag(uint32_t flag);
    /// Clears a data flag
    /// @param flag Flag to clear
    /// @return True if the flag was cleared, false if not
    bool ClearDataFlag(uint32_t flag)
        {PTB_CHECK_THAT(IsValidDataFlag(flag), false); m_data &= ~flag; return (true);}
    /// Determines if a data flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsDataFlagSet(uint32_t flag) const
    {
        PTB_CHECK_THAT(IsValidDataFlag(flag), false);
        return ((m_data & flag) == flag);
    }
    
// Single Note Functions
public:    
    /// Determines if a string number is valid
    /// @param stringNumber String number to validate
    /// @return True if the string number is valid, false if not
    static bool IsValidStringNumber(uint8_t stringNumber)
    {
        return ((stringNumber >= MIN_SINGLE_NOTE_STRING_NUMBER) &&
            (stringNumber <= MAX_SINGLE_NOTE_STRING_NUMBER));
    }
    /// Determines if a fret number is valid
    /// @param fretNumber Fret number to validate
    /// @return True if the fret number is valid, false if not
    static bool IsValidFretNumber(uint8_t fretNumber)
    {
        return ((fretNumber >= MIN_SINGLE_NOTE_FRET_NUMBER) &&
            (fretNumber <= MAX_SINGLE_NOTE_FRET_NUMBER));
    }
    bool IsSingleNote() const { return IsDataFlagSet(singleNote); }
    bool SetSingleNoteData(uint8_t stringNumber, uint8_t fretNumber);
    void GetSingleNoteData(uint8_t& stringNumber, uint8_t& fretNumber) const;
    bool ClearSingleNoteData();
};

}

#endif // RHYTHMSLASH_H
