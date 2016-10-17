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

#ifndef POSITION_H
#define POSITION_H

#include "powertabobject.h"
#include "macros.h"

#include <array>
#include <vector>

namespace PowerTabDocument {

class Note;

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

    static const size_t MAX_POSITION_COMPLEX_SYMBOLS = 2; ///< Maximum allowed number of complex symbols per position object

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
private:
    uint8_t	m_position; ///< Zero-based index of the position within the system where the position is anchored
    uint16_t	m_beaming;  ///< Beaming and irregular grouping timing data
    uint32_t	m_data;     ///< Duration and simple symbol flags
    std::array<uint32_t, MAX_POSITION_COMPLEX_SYMBOLS> m_complexSymbolArray; ///< Array of complex symbols

public:
    std::vector<Note*> m_noteArray;      ///< Array of notes

public:
    Position();
    ~Position();

    // Serialization Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {return "CPosition";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {return ((uint16_t)1);}

    /// Gets the position within the system where the position is anchored
    /// @return The position within the system where the position is anchored
    inline uint32_t GetPosition() const
    {
        return m_position;
    }

    uint8_t GetDurationType() const;

    // Irregular Grouping Functions
    void GetIrregularGroupingTiming(uint8_t& notesPlayed,
                                    uint8_t& notesPlayedOver) const;
    bool HasIrregularGroupingTiming() const;

public:
    /// Determines if the position's duration is dotted
    /// @return True if the position's duration is dotted, false if not
    bool IsDotted() const
    {return (IsDataFlagSet(dotted));}

    /// Determines if the position's duration is double dotted
    /// @return True if the position's duration is double dotted, false if not
    bool IsDoubleDotted() const
    {return (IsDataFlagSet(doubleDotted));}

    /// Determines if the position is a rest
    /// @return True if the position is a rest, false if not
    bool IsRest() const
    {return (IsDataFlagSet(rest));}

    /// Determines if the position is a vibrato
    /// @return True if the position is a vibrato, false if not
    bool HasVibrato() const
    {return (IsDataFlagSet(vibrato));}

    /// Determines if the position is a wideVibrato
    /// @return True if the position is a wideVibrato, false if not
    bool HasWideVibrato() const
    {return (IsDataFlagSet(wideVibrato));}

    /// Determines if the position has an arpeggio up symbol
    /// @return True if the position has an arpeggio up symbol, false if not
    bool HasArpeggioUp() const
    {return (IsDataFlagSet(arpeggioUp));}

    /// Determines if the position has an arpeggio down symbol
    /// @return True if the position has an arpeggio down symbol, false if not
    bool HasArpeggioDown() const
    {return (IsDataFlagSet(arpeggioDown));}

    /// Determines if the position has a pick stroke up symbol
    /// @return True if the position has a pick stroke up symbol, false if not
    bool HasPickStrokeUp() const
    {return (IsDataFlagSet(pickStrokeUp));}

    /// Determines if the position has a pick stroke down symbol
    /// @return True if the position has a pick stroke down symbol, false if not
    bool HasPickStrokeDown() const
    {return (IsDataFlagSet(pickStrokeDown));}

    /// Determines if the position is staccatoed
    /// @return True if the position is staccatoed, false if not
    bool IsStaccato() const
    {return (IsDataFlagSet(staccato));}

    /// Determines if the position has a marcato (standard accent)
    /// @return True if the position has a marcato, false if not
    bool HasMarcato() const
    {return (IsDataFlagSet(marcato));}

    /// Determines if the position has a sforzando (heavy accent)
    /// @return True if the position has a sforzando, false if not
    bool HasSforzando() const
    {return (IsDataFlagSet(sforzando));}

    /// Determines if the position has a tremolo picking
    /// @return True if the position has a tremolo picking, false if not
    bool HasTremoloPicking() const
    {return (IsDataFlagSet(tremoloPicking));}

    /// Determines if the position has a palm muting
    /// @return True if the position has a palm muting, false if not
    bool HasPalmMuting() const
    {return (IsDataFlagSet(palmMuting));}

    /// Determines if the position has a tap
    /// @return True if the position has a tap, false if not
    bool HasTap() const
    {return (IsDataFlagSet(tap));}

    /// Determines if the position has a acciaccatura
    /// @return True if the position has a acciaccatura, false if not
    bool IsAcciaccatura() const
    {return (IsDataFlagSet(acciaccatura));}

    /// Determines if the position uses the 1st triplet feel effect (played as
    /// if it was the 1st note of two 'triplet feel' notes)
    /// @return True if the position uses the 1st triplet feel effect, false if
    /// not
    bool IsTripletFeel1st() const
    {return (IsDataFlagSet(tripletFeel1st));}

    /// Determines if the position uses the 2nd triplet feel effect (played as
    /// if it was the 1st note of two 'triplet feel' notes)
    /// @return True if the position uses the 2nd triplet feel effect, false if
    /// not
    bool IsTripletFeel2nd() const
    {return (IsDataFlagSet(tripletFeel2nd));}

    /// Determines if the position has a let ring symbol
    /// @return True if the position has a let ring symbol, false if not
    bool HasLetRing() const
    {return (IsDataFlagSet(letRing));}

    /// Determines if the position has a fermata
    /// @return True if the position has a fermata, false if not
    bool HasFermata() const
    {return (IsDataFlagSet(fermata));}

    /// Determines if the position is the start of a irregular grouping grouping
    /// @return True if the position is the start of a irregular grouping
    /// grouping, false if not
    bool IsIrregularGroupingStart() const
    {return (IsDataFlagSet(irregularGroupingStart));}

    /// Determines if the position is the middle of a irregular grouping
    /// grouping
    /// @return True if the position is the middle of a irregular grouping
    /// grouping, false if not
    bool IsIrregularGroupingMiddle() const
    {return (IsDataFlagSet(irregularGroupingMiddle));}

    /// Determines if the position is the end of a irregular grouping
    /// @return True if the position is the end of a irregular grouping, false
    /// if not
    bool IsIrregularGroupingEnd() const
    {return (IsDataFlagSet(irregularGroupingEnd));}

private:
    /// Determines if a data flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidDataFlag(uint32_t flag)
    {
        return (((flag & dataFlagsMask) != 0) &&
                ((flag & ~dataFlagsMask) == 0));
    }

    /// Determines if a data flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsDataFlagSet(uint32_t flag) const
    {
        PTB_CHECK_THAT(IsValidDataFlag(flag), false);
        return ((m_data & flag) == flag);
    }

public:
    // Volume Swell Functions
    bool GetVolumeSwell(uint8_t& startVolume, uint8_t& endVolume,
                        uint8_t& duration) const;
    bool HasVolumeSwell() const;

    // Tremolo Bar Functions
    bool GetTremoloBar(uint8_t& type, uint8_t& duration, uint8_t& pitch) const;
    bool HasTremoloBar() const;

    // Multibar Rest Functions.
    bool GetMultibarRest(uint8_t& measureCount) const;
    bool HasMultibarRest() const;

    /// Determines if a note index is valid
    /// @param index note index to validate
    /// @return True if the note index is valid, false if not
    bool IsValidNoteIndex(size_t index) const
    {return (index < GetNoteCount());}
    /// Gets the number of notes in the position
    /// @return The number of notes in the position
    size_t GetNoteCount() const
    {return (m_noteArray.size());}
    /// Gets the nth note in the position
    /// @param index Index of the note to get
    /// @return The nth note in the position
    Note* GetNote(size_t index) const
    {
        PTB_CHECK_THAT(IsValidNoteIndex(index), nullptr);
        return (m_noteArray[index]);
    }
};

}

#endif // POSITION_H
