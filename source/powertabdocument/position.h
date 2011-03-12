/////////////////////////////////////////////////////////////////////////////
// Name:            position.h
// Purpose:         Stores and renders a position (a group of notes, or a rest)
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 17, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __POSITION_H__
#define __POSITION_H__

class Note;
class Tuning;

#include "dynamic.h"        // Needed for volume swell volumes and validation

#define MAX_POSITION_COMPLEX_SYMBOLS    2   ///< Maximum allowed number of complex symbols per position object
// Note: needs to be #define so array works properly

/// Stores and renders a position (a group of notes, or a rest)
class Position : public PowerTabObject
{
    // Constants
public:
    // Default Constants
    static const uint8_t     DEFAULT_POSITION;                               ///< Default value for the position member variable
    static const uint8_t     DEFAULT_BEAMING;                                ///< Default value for the beaming member variable
    static const uint32_t   DEFAULT_DATA;                                   ///< Default value for the data member variable
    static const uint8_t     DEFAULT_DURATION_TYPE;                          ///< Default duration type

    // Position Constants
    static const uint32_t   MIN_POSITION;                                   ///< Minimum allowed value for the position member variable
    static const uint32_t   MAX_POSITION;                                   ///< Maximum allowed value for the position member variable

    // Irregular Grouping Constants
    static const uint8_t     MIN_IRREGULAR_GROUPING_NOTES_PLAYED;            ///< Minimum allowed value for the irregular grouping notes played value
    static const uint8_t     MAX_IRREGULAR_GROUPING_NOTES_PLAYED;            ///< Maximum allowed value for the irregular grouping notes played value
    static const uint8_t     MIN_IRREGULAR_GROUPING_NOTES_PLAYED_OVER;       ///< Minimum allowed value for the irregular grouping notes played over value
    static const uint8_t     MAX_IRREGULAR_GROUPING_NOTES_PLAYED_OVER;       ///< Maximum allowed value for the irregular grouping notes played over value

    // Volume Swell Constants
    static const uint8_t     MAX_VOLUME_SWELL_DURATION;                      ///< Maximum allowed value for the volume swell duration

    // Tremolo Bar Constants
    static const uint8_t     MAX_TREMOLO_BAR_DURATION;                       ///< Maximum allowed value for the tremolo bar duration
    static const uint8_t     MAX_TREMOLO_BAR_PITCH;                          ///< Maximum allowed value for the tremolo bar pitch (in quarter steps)

    // Multibar Rest Constants
    static const uint8_t     MIN_MULTIBAR_REST_MEASURE_COUNT;                ///< Minimum allowed value for a multibar rest measure count
    static const uint8_t     MAX_MULTIBAR_REST_MEASURE_COUNT;                ///< Maximum allowed value for a multibar rest measure count

    enum ShiftType
    {
        SHIFT_UP,
        SHIFT_DOWN,
    };

    enum beamingFlags
    {
        irregularGroupingNotesPlayedOverMask    = (uint16_t)0x07,             ///< Mask used to get notes played over value
        irregularGroupingNotesPlayedMask        = (uint16_t)0x78,             ///< Mask used to get notes played value
        irregularGroupingTimingMask             = (uint16_t)0x7f,             ///< Mask used to filter out the irregular grouping timing values

        previousBeamDuration8th         = (uint8_t)0x01,                     ///< Previous position in beam group is an 8th note
        previousBeamDuration16th        = (uint8_t)0x02,                     ///< Previous position in beam group is a 16th note
        previousBeamDuration32nd        = (uint8_t)0x03,                     ///< Previous position in beam group is a 32nd note
        previousBeamDuration64th        = (uint8_t)0x04,                     ///< Previous position in beam group is a 64th note
        previousBeamDurationTypeMask    = (uint16_t)0x0380,                   ///< Mask that filters out mututally exclusive beam duration flags

        beamStart		                = (uint16_t)0x400,		            ///< Note is the start of a group of beamed notes
        beamFractionalLeft	            = (uint16_t)0x800,		            ///< Note is half beam that draws left to right
        beamFractionalRight	            = (uint16_t)0x1000,		            ///< Note is half beam that draws right to left
        beamEnd			                = (uint16_t)0x2000,		            ///< Note is the end of a group of beamed notes

        beamingFlagsMask                = (uint16_t)0x3f80                    ///< Mask that filters out all possible beaming flags (previous beam duration + beam type)

                                      };

    // Simple flags
    enum simpleFlags
    {
        dotted			    = (uint32_t)0x01,		///< Duration is dotted
        doubleDotted		= (uint32_t)0x02,		///< Duration is double dotted
        dottedMask          = (uint32_t)0x03,       ///< Mask that filters out mutually exclusive dotted flags

        rest			    = (uint32_t)0x04,		///< Position is a rest

        vibrato			    = (uint32_t)0x08,		///< Notes are vibratoed
        wideVibrato		    = (uint32_t)0x10,		///< Notes are vibratoed rapidly
        vibratoMask         = (uint32_t)0x18,       ///< Mask that filters out mutually exclusive vibrato flags

        arpeggioUp		    = (uint32_t)0x20,		///< Arpeggio up symbol
        arpeggioDown	    = (uint32_t)0x40,		///< Arpeggio down symbol
        arpeggioMask        = (uint32_t)0x60,       ///< Mask that filters out mutually exclusive arpeggio flags

        pickStrokeUp	    = (uint32_t)0x80,		///< Pickstroke up symbol
        pickStrokeDown	    = (uint32_t)0x100,		///< Pickstroke down symbol
        pickStrokeMask      = (uint32_t)0x180,      ///< Mask that filters out mutually exclusive pickstroke flags

        staccato		    = (uint32_t)0x200,		///< Notes are staccatoed

        marcato			    = (uint32_t)0x400,		///< Standard accent
        sforzando		    = (uint32_t)0x800,		///< Heavy accent
        accentMask          = (uint32_t)0xc00,      ///< Mask that filters out mutually exclusive accent flags

        tremoloPicking	    = (uint32_t)0x1000,	    ///< Notes are plucked rapidly
        palmMuting		    = (uint32_t)0x2000,	    ///< Notes are muffled by the picking hand
        tap				    = (uint32_t)0x4000,	    ///< Notes are tapped
        appoggiatura	    = (uint32_t)0x8000,	    ///< Leaning note (NOT USED in v1.7)
        acciaccatura	    = (uint32_t)0x10000,	///< Crushing note

        tripletFeel1st	    = (uint32_t)0x20000,	///< Indicates the notes should be played as if it were the 1st note in a triplet feel duet
        tripletFeel2nd	    = (uint32_t)0x40000,	///< Indicates the slash should be played as if it were the 2nd note in a triplet feel duet
        tripletFeelMask     = (uint32_t)0x60000,    ///< Mask that filters out mutually exclusive triplet feel flags

        letRing			    = (uint32_t)0x80000,	            ///< Notes are held longer than the notated duration
        fermata		        = (uint32_t)0x100000,	            ///< Fermata sign
        irregularGroupingStart	    = (uint32_t)0x200000,	    ///< Start of an irregular grouping
        irregularGroupingMiddle     = (uint32_t)0x400000,	    ///< Middle of an irregular grouping
        irregularGroupingEnd	    = (uint32_t)0x800000,	    ///< End of an irregular grouping
        irregularGroupingFlagsMask   = (uint32_t)0xe00000,      ///< Masks that filters all possible irregular grouping types

        dataFlagsMask       = (uint32_t)0xffffff,               ///< Mask that filters out all valid data flags
        durationTypeMask    = (uint32_t)0xff000000              ///< Mask used to retrieve the duration type

                          };

    enum complexSymbolTypes
    {
        volumeSwell             = (uint8_t)'a',
        tremoloBar              = (uint8_t)'c',
        multibarRest            = (uint8_t)'j',
        notUsed                 = (uint32_t)0
                              };

    enum tremoloBarTypes
    {
        dip                     = (uint8_t)0x00,
        diveAndRelease          = (uint8_t)0x01,
        diveAndHold             = (uint8_t)0x02,
        release                 = (uint8_t)0x03,
        returnAndRelease        = (uint8_t)0x04,
        returnAndHold           = (uint8_t)0x05,
        invertedDip             = (uint8_t)0x06
                              };

    // Member Variables
protected:
    uint8_t					m_position;				                                ///< Zero-based index of the position within the system where the position is anchored
    uint16_t					m_beaming;				                                ///< Beaming and irregular grouping timing data
    uint32_t				m_data;			                                        ///< Duration and simple symbol flags
    uint32_t				m_complexSymbolArray[MAX_POSITION_COMPLEX_SYMBOLS];	    ///< Array of complex symbols

public:
    std::vector<Note*> m_noteArray;      ///< Array of notes

    // Constructor/Destructor
public:
    Position();
    Position(uint32_t position, uint8_t durationType, uint8_t dotCount);
    Position(const Position& position);
    ~Position();

    // Operators
    const Position& operator=(const Position& position);
    bool operator==(const Position& position) const;
    bool operator!=(const Position& position) const;

    // Serialization Functions
    bool Serialize(PowerTabOutputStream& stream);
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    Position* CloneObject() const;

    // MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    string GetMFCClassName() const
    {return "CPosition";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
    {return ((uint16_t)1);}

    // Position Functions
    /// Determines whether a position is valid
    /// @param position Position to validate
    /// @return True if the position is valid, false if not
    static bool IsValidPosition(uint32_t position)
    {return ((position >= MIN_POSITION) && (position <= MAX_POSITION));}
    /// Sets the position within the system where the position is anchored
    /// @param position Zero-based index within the system where the position is
    /// anchored
    /// @return True if the position was set, false if not
    bool SetPosition(uint32_t position)
    {
        CHECK_THAT(IsValidPosition(position), false);
        m_position = (uint8_t)position;
        return (true);
    }
    /// Gets the position within the system where the position is anchored
    /// @return The position within the system where the position is anchored
    uint32_t GetPosition() const
    {return (m_position);}

    // Duration Type Functions
    /// Determines if a duration type is valid
    /// @param durationType Duration type to validate
    /// @return True if the duration type is valid, false if not
    static bool IsValidDurationType(uint8_t durationType)
    {
        return ((durationType == 1) || (durationType == 2) ||
                (durationType == 4) || (durationType == 8) ||
                (durationType == 16) || (durationType == 32) ||
                (durationType == 64));
    }
    bool SetDurationType(uint8_t durationType);
    uint8_t GetDurationType() const;

    double GetDuration() const;

    // Irregular Grouping Functions
    /// Determines if an irregular grouping timing is valid
    /// @param notesPlayed Number of notes played
    /// @param notesPlayedOver Number of notes played over
    /// @return True if the irregular grouping timing is valid, false if not
    static bool IsValidIrregularGroupingTiming(uint8_t notesPlayed,
                                               uint8_t notesPlayedOver)
    {
        return
                ((notesPlayed >= MIN_IRREGULAR_GROUPING_NOTES_PLAYED) &&
                 (notesPlayed <= MAX_IRREGULAR_GROUPING_NOTES_PLAYED)) &&
                ((notesPlayedOver >= MIN_IRREGULAR_GROUPING_NOTES_PLAYED_OVER) &&
                 (notesPlayedOver <= MAX_IRREGULAR_GROUPING_NOTES_PLAYED_OVER));
    }
    bool SetIrregularGroupingTiming(uint8_t notesPlayed, uint8_t notesPlayedOver);
    void GetIrregularGroupingTiming(uint8_t& notesPlayed,
                                    uint8_t& notesPlayedOver) const;
    bool HasIrregularGroupingTiming() const;
    bool ClearIrregularGroupingTiming();

    // Previous Beam Duration Functions
    /// Determines if a previous beam duration type is valid
    /// @param durationType Duration type to validate
    /// @return True if the duration type is valid, false if not
    static bool IsValidPreviousBeamDurationType(uint8_t durationType)
    {
        return ((durationType == 0) || (durationType == 8) ||
        (durationType == 16) || (durationType == 32) || (durationType == 64));
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
    /// Determines if the position is the start of a beam grouping
    /// @return True if the position is the start of a beam grouping, false if
    /// not
    bool IsBeamStart() const
    {return (IsBeamingFlagSet(beamStart));}
    /// Sets or clears the fractional left beam value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetFractionalLeftBeam(bool set = true)
    {
        if (!set)
            return (ClearBeamingFlag(beamFractionalLeft));
        return (SetBeamingFlag(beamFractionalLeft));
    }
    /// Determines if the position has a fractional left beam
    /// @return True if the position has a fractional left beam, false if not
    bool HasFractionalLeftBeam() const
    {return (IsBeamingFlagSet(beamFractionalLeft));}
    /// Sets or clears the fractional right beam value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetFractionalRightBeam(bool set = true)
    {
        if (!set)
            return (ClearBeamingFlag(beamFractionalRight));
        return (SetBeamingFlag(beamFractionalRight));
    }
    /// Determines if the position has a fractional right beam
    /// @return True if the position has a fractional right beam, false if not
    bool HasFractionalRightBeam() const
    {return (IsBeamingFlagSet(beamFractionalRight));}
    /// Sets or clears the beam end value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetBeamEnd(bool set = true)
    {
        if (!set)
            return (ClearBeamingFlag(beamEnd));
        return (SetBeamingFlag(beamEnd));
    }
    /// Determines if the position is the end of a beam grouping
    /// @return True if the position is the end of a beam grouping, false if not
    bool IsBeamEnd() const
    {return (IsBeamingFlagSet(beamEnd));}
    /// Clears all beaming data
    /// @return True if the beaming data was cleared, false if not
    bool ClearBeam()
    {return (ClearBeamingFlag(beamingFlagsMask));}

    bool IsBeamable() const;

protected:
    /// Determines if a beaming flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidBeamingFlag(uint16_t flag)
    {
        return (((flag & beamingFlagsMask) != 0) &&
                ((flag & ~beamingFlagsMask) == 0));
    }
    bool SetBeamingFlag(uint16_t flag);
    /// Clears a beaming flag
    /// @param flag Flag to clear
    /// @return True if the beaming flag was cleared, false if not
    bool ClearBeamingFlag(uint16_t flag)
    {
        CHECK_THAT(IsValidBeamingFlag(flag), false);
        m_beaming &= ~flag;
        return (true);
    }
    /// Determines if a beaming flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsBeamingFlagSet(uint16_t flag) const
    {
        CHECK_THAT(IsValidBeamingFlag(flag), false);
        return ((m_beaming & flag) == flag);
    }

    // Dotted Functions
public:
    /// Sets or clears the dotted duration state
    /// @param set True sets the dotted state, false clears it
    /// @return True if the dotted state was set or cleared, false if not
    bool SetDotted(bool set = true)
    {
        if (!set) return (ClearDataFlag(dotted));
        return (SetDataFlag(dotted));
    }
    /// Determines if the position's duration is dotted
    /// @return True if the position's duration is dotted, false if not
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
    /// Determines if the position's duration is double dotted
    /// @return True if the position's duration is double dotted, false if not
    bool IsDoubleDotted() const
    {return (IsDataFlagSet(doubleDotted));}

    // Rest Functions
    /// Sets or clears the rest state
    /// @param set True sets the rest state, false clears it
    /// @return True if the rest state was set or cleared, false if not
    bool SetRest(bool set = true)
    {if (!set) return (ClearDataFlag(rest)); return (SetDataFlag(rest));}
    /// Determines if the position is a rest
    /// @return True if the position is a rest, false if not
    bool IsRest() const
    {return (IsDataFlagSet(rest));}

    // Vibrato Functions
    /// Sets or clears the vibrato state
    /// @param set True sets the vibrato state, false clears it
    /// @return True if the vibrato state was set or cleared, false if not
    bool SetVibrato(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(vibrato));
        return (SetDataFlag(vibrato));
    }
    /// Determines if the position is a vibrato
    /// @return True if the position is a vibrato, false if not
    bool HasVibrato() const
    {return (IsDataFlagSet(vibrato));}

    // Wide Vibrato Functions
    /// Sets or clears the wide vibrato state
    /// @param set True sets the wide vibrato state, false clears it
    /// @return True if the wide vibrato state was set or cleared, false if not
    bool SetWideVibrato(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(wideVibrato));
        return (SetDataFlag(wideVibrato));
    }
    /// Determines if the position is a wideVibrato
    /// @return True if the position is a wideVibrato, false if not
    bool HasWideVibrato() const
    {return (IsDataFlagSet(wideVibrato));}

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
    /// Determines if the position has an arpeggio up symbol
    /// @return True if the position has an arpeggio up symbol, false if not
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
    /// Determines if the position has an arpeggio down symbol
    /// @return True if the position has an arpeggio down symbol, false if not
    bool HasArpeggioDown() const
    {return (IsDataFlagSet(arpeggioDown));}

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
    /// Determines if the position has a pick stroke up symbol
    /// @return True if the position has a pick stroke up symbol, false if not
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
    /// Determines if the position has a pick stroke down symbol
    /// @return True if the position has a pick stroke down symbol, false if not
    bool HasPickStrokeDown() const
    {return (IsDataFlagSet(pickStrokeDown));}

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
    /// Determines if the position is staccatoed
    /// @return True if the position is staccatoed, false if not
    bool IsStaccato() const
    {return (IsDataFlagSet(staccato));}

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
    /// Determines if the position has a marcato (standard accent)
    /// @return True if the position has a marcato, false if not
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
    /// Determines if the position has a sforzando (heavy accent)
    /// @return True if the position has a sforzando, false if not
    bool HasSforzando() const
    {return (IsDataFlagSet(sforzando));}

    // Tremolo Picking Functions
    /// Sets or clears the tremolo picking symbol
    /// @param set True sets the tremolo picking symbol, false clears it
    /// @return True if the tremolo picking symbol was set or cleared, false if
    /// not
    bool SetTremoloPicking(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(tremoloPicking));
        return (SetDataFlag(tremoloPicking));
    }
    /// Determines if the position has a tremolo picking
    /// @return True if the position has a tremolo picking, false if not
    bool HasTremoloPicking() const
    {return (IsDataFlagSet(tremoloPicking));}

    // Palm Muting Functions
    /// Sets or clears the palm muting symbol
    /// @param set True sets the palm muting symbol, false clears it
    /// @return True if the palm muting symbol was set or cleared, false if not
    bool SetPalmMuting(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(palmMuting));
        return (SetDataFlag(palmMuting));
    }
    /// Determines if the position has a palm muting
    /// @return True if the position has a palm muting, false if not
    bool HasPalmMuting() const
    {return (IsDataFlagSet(palmMuting));}

    // Tap Functions
    /// Sets or clears the tap symbol
    /// @param set True sets the tap symbol, false clears it
    /// @return True if the tap symbol was set or cleared, false if not
    bool SetTap(bool set = true)
    {if (!set) return (ClearDataFlag(tap)); return (SetDataFlag(tap));}
    /// Determines if the position has a tap
    /// @return True if the position has a tap, false if not
    bool HasTap() const
    {return (IsDataFlagSet(tap));}

    // Acciaccatura Functions
    /// Sets or clears the acciaccatura symbol
    /// @param set True sets the acciaccatura symbol, false clears it
    /// @return True if the acciaccatura symbol was set or cleared, false if not
    bool SetAcciaccatura(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(acciaccatura));
        return (SetDataFlag(acciaccatura));
    }
    /// Determines if the position has a acciaccatura
    /// @return True if the position has a acciaccatura, false if not
    bool IsAcciaccatura() const
    {return (IsDataFlagSet(acciaccatura));}

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
    /// Determines if the position uses the 1st triplet feel effect (played as
    /// if it was the 1st note of two 'triplet feel' notes)
    /// @return True if the position uses the 1st triplet feel effect, false if
    /// not
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
    /// Determines if the position uses the 2nd triplet feel effect (played as
    /// if it was the 1st note of two 'triplet feel' notes)
    /// @return True if the position uses the 2nd triplet feel effect, false if
    /// not
    bool IsTripletFeel2nd() const
    {return (IsDataFlagSet(tripletFeel2nd));}

    // Let Ring Functions
    /// Sets or clears the let ring symbol
    /// @param set True sets the let ring symbol, false clears it
    /// @return True if the let ring symbol was set or cleared, false if not
    bool SetLetRing(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(letRing));
        return (SetDataFlag(letRing));
    }
    /// Determines if the position has a let ring symbol
    /// @return True if the position has a let ring symbol, false if not
    bool HasLetRing() const
    {return (IsDataFlagSet(letRing));}

    // Fermata Functions
    /// Sets or clears the fermata symbol
    /// @param set True sets the fermata symbol, false clears it
    /// @return True if the fermata symbol was set or cleared, false if not
    bool SetFermata(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(fermata));
        return (SetDataFlag(fermata));
    }
    /// Determines if the position has a fermata
    /// @return True if the position has a fermata, false if not
    bool HasFermata() const
    {return (IsDataFlagSet(fermata));}

    // Irregular Grouping Functions
    /// Sets or clears the irregular grouping start value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetIrregularGroupingStart(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(irregularGroupingStart));
        return (SetDataFlag(irregularGroupingStart));
    }
    /// Determines if the position is the start of a irregular grouping grouping
    /// @return True if the position is the start of a irregular grouping
    /// grouping, false if not
    bool IsIrregularGroupingStart() const
    {return (IsDataFlagSet(irregularGroupingStart));}

    /// Sets or clears the irregular grouping middle value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetIrregularGroupingMiddle(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(irregularGroupingMiddle));
        return (SetDataFlag(irregularGroupingMiddle));
    }
    /// Determines if the position is the middle of a irregular grouping
    /// grouping
    /// @return True if the position is the middle of a irregular grouping
    /// grouping, false if not
    bool IsIrregularGroupingMiddle() const
    {return (IsDataFlagSet(irregularGroupingMiddle));}

    /// Sets or clears the irregular grouping end value
    /// @param set True sets the value, false clears it
    /// @return True if the value was set or cleared, false if not
    bool SetIrregularGroupingEnd(bool set = true)
    {
        if (!set)
            return (ClearDataFlag(irregularGroupingEnd));
        return (SetDataFlag(irregularGroupingEnd));
    }
    /// Determines if the position is the end of a irregular grouping
    /// @return True if the position is the end of a irregular grouping, false
    /// if not
    bool IsIrregularGroupingEnd() const
    {return (IsDataFlagSet(irregularGroupingEnd));}

    // Flag Functions
protected:
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
    {
        CHECK_THAT(IsValidDataFlag(flag), false);
        m_data &= ~flag;
        return (true);
    }
    /// Determines if a data flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsDataFlagSet(uint32_t flag) const
    {
        CHECK_THAT(IsValidDataFlag(flag), false);
        return ((m_data & flag) == flag);
    }

    // Volume Swell Functions
public:
    static bool IsValidVolumeSwell(uint8_t startVolume, uint8_t endVolume, uint8_t duration)
    {
        return (
                (Dynamic::IsValidVolume(startVolume) && startVolume != Dynamic::notSet) &&
                (Dynamic::IsValidVolume(endVolume) && endVolume != Dynamic::notSet) &&
                (startVolume != endVolume) && (duration <= MAX_VOLUME_SWELL_DURATION)
                );
    }
    bool SetVolumeSwell(uint8_t startVolume, uint8_t endVolume, uint8_t duration);
    bool GetVolumeSwell(uint8_t& startVolume, uint8_t& endVolume,
                        uint8_t& duration) const;
    bool HasVolumeSwell() const;
    bool ClearVolumeSwell();

    // Tremolo Bar Functions
    /// Determines if a tremolo bar type is valid
    /// @param type Type to validate
    /// @return True if the tremolo bar type is valid, false if not
    static bool IsValidTremoloBarType(uint8_t type)
    {return (type <= invertedDip);}
    /// Determines if a tremolo bar duration is valid
    /// @param duration Duration to validate
    /// @return True if the tremolo bar duration is valid, false if not
    static bool IsValidTremoloBarDuration(uint8_t duration)
    {return (duration <= MAX_TREMOLO_BAR_DURATION);}
    /// Determines if a tremolo bar pitch is valid
    /// @param pitch Pitch to validate
    /// @return True if the tremolo bar pitch is valid, false if not
    static bool IsValidTremoloBarPitch(uint8_t pitch)
    {return (pitch <= MAX_TREMOLO_BAR_PITCH);}
    /// Determines if tremolo bar data is valid
    /// @param type Type to validate
    /// @param duration Duration to validate
    /// @param pitch Pitch to validate
    /// @return True if the tremolo bar is valid, false if not
    static bool IsValidTremoloBar(uint8_t type, uint8_t duration, uint8_t pitch)
    {return (
            (IsValidTremoloBarType(type)) &&
            (IsValidTremoloBarDuration(duration)) &&
            (IsValidTremoloBarPitch(pitch))
            );
    }
    bool SetTremoloBar(uint8_t type, uint8_t duration, uint8_t pitch);
    bool GetTremoloBar(uint8_t& type, uint8_t& duration, uint8_t& pitch) const;
    bool HasTremoloBar() const;
    bool ClearTremoloBar();

    // Multibar Rest Functions
    /// Determines if a multibar rest count is valid
    /// @param measureCount Measure count to validate
    /// @return True if the multibar rest measure count is valid, false if not
    static bool IsValidMultibarRestMeasureCount(uint8_t measureCount)
    {
        return ((measureCount >= MIN_MULTIBAR_REST_MEASURE_COUNT) &&
                (measureCount <= MAX_MULTIBAR_REST_MEASURE_COUNT));
    }
    /// Determines if multibar rest data is valid
    /// @param measureCount Measure count to validate
    /// @return True if the multibar rest data is valid, false if not
    static bool IsValidMultibarRest(uint8_t measureCount)
    {return (IsValidMultibarRestMeasureCount(measureCount));}
    bool SetMultibarRest(uint8_t measureCount);
    bool GetMultibarRest(uint8_t& measureCount) const;
    bool HasMultibarRest() const;
    bool ClearMultibarRest();

    // Complex Symbol Array Functions
protected:
    /// Determines if a complex symbol type is valid
    /// @param type Symbol type to validate
    /// @return True if the symbol type is valid, false if not
    static bool IsValidComplexSymbolType(uint8_t type)
    {
        return ((type == volumeSwell) || (type == tremoloBar) ||
                (type == multibarRest));
    }
    bool AddComplexSymbol(uint32_t symbolData);
    size_t GetComplexSymbolCount() const;
    uint32_t FindComplexSymbol(uint8_t type) const;
    bool RemoveComplexSymbol(uint8_t type);
    void ClearComplexSymbolArrayContents();

    // Note Functions
public:
    /// Determines if a note index is valid
    /// @param index note index to validate
    /// @return True if the note index is valid, false if not
    bool IsValidNoteIndex(uint32_t index) const
    {return (index < GetNoteCount());}
    /// Gets the number of notes in the position
    /// @return The number of notes in the position
    size_t GetNoteCount() const
    {return (m_noteArray.size());}
    /// Gets the nth note in the position
    /// @param index Index of the note to get
    /// @return The nth note in the position
    Note* GetNote(uint32_t index) const
    {
        CHECK_THAT(IsValidNoteIndex(index), NULL);
        return (m_noteArray[index]);
    }

    Note* GetNoteByString(uint8_t string) const;
    bool CanShiftTabNumber(Note* note, ShiftType type, uint8_t numStringsInStaff, const Tuning& tuning) const;
    bool ShiftTabNumber(Note* note, ShiftType type, uint8_t numStringsInStaff, const Tuning& tuning);

    void SortNotesUp();
    void SortNotesDown();

    std::pair<uint8_t, uint8_t> GetStringBounds() const;
    
    /// Gets a list of the notes at this position
    /// @param notes A reference to a list that will store the notes 
    void GetNotes(std::vector<Note*>& notes) const
    {
        notes = m_noteArray;
    }

protected:
    int GetShiftedStringNumber(Note* note, ShiftType type) const;
    int GetShiftedFretNumber(Note* note, int originalString, int newString, const Tuning& tuning) const;

public:
    // Functions for checking if at least one of the member notes has a property enabled
    bool HasNoteWithTrill() const;
    bool HasNoteWithNaturalHarmonic() const;
    bool HasNoteWithArtificialHarmonic() const;
    bool HasNoteWithHammeronOrPulloff() const;
    bool HasNoteWithHammeron() const;
    bool HasNoteWithHammeronFromNowhere() const;
    bool HasNoteWithPulloff() const;
    bool HasNoteWithPulloffToNowhere() const;
    bool HasNoteWithSlide() const;
    bool HasNoteWithTappedHarmonic() const;

protected:
    bool HasNoteWithProperty(bool (Note::*notePropertyPredicate)() const) const;
};

#endif
