/////////////////////////////////////////////////////////////////////////////
// Name:            chordname.h
// Purpose:         Stores and renders a chord name
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef CHORDNAME_H
#define CHORDNAME_H

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders a chord name
class ChordName : public PowerTabObject
{
// Constants
public:
    static const uint16_t DEFAULT_KEY;                        ///< Default value for the key member variable
    static const uint8_t DEFAULT_FORMULA;                    ///< Default value for the formula member variable
    static const uint16_t DEFAULT_FORMULA_MODIFICATIONS;      ///< Default value for the formula modifications member variable
    static const uint8_t DEFAULT_EXTRA;                      ///< Default value for the extra member variable

    static const uint8_t MIN_TYPE;                           ///< Minimum allowed value for the type
    static const uint8_t MAX_TYPE;                           ///< Maximum allowed value for the type

    static const uint8_t MIN_FRET_POSITION;                  ///< Minimum allowed value for the fret position
    static const uint8_t MAX_FRET_POSITION;                  ///< Maximum allowed value for the fret position

    enum Key
    {
        C                   = (uint8_t)0x00,
        CSharp              = (uint8_t)0x01,
        D                   = (uint8_t)0x02,
        EFlat               = (uint8_t)0x03,
        E                   = (uint8_t)0x04,
        F                   = (uint8_t)0x05,
        FSharp              = (uint8_t)0x06,
        G                   = (uint8_t)0x07,
        AFlat               = (uint8_t)0x08,
        A                   = (uint8_t)0x09,
        BFlat               = (uint8_t)0x0a,
        B                   = (uint8_t)0x0b
    };

    // Key variations (homonyms)
    enum keyVariations
    {
        variationDown       = (uint8_t)0x00,                 ///< B#, Bx, Cx, D#, Dx, E#, Ex, Fx, G#, Gx, A#, Ax
        variationDefault    = (uint8_t)0x01,                 ///< C, C#, D, Eb, E, F, F#, G, Ab, A, Bb, B
        variationUp         = (uint8_t)0x02                  ///< Dbb, Db, Ebb, Fbb, Fb, Gbb, Gb, Abb, , Bbb, Cbb, Cb
    };

    enum keyFlags
    {
        tonicKeyMask                = (uint16_t)0xf00,        ///< Mask used to retrieve the tonic key
        tonicKeyVariationMask       = (uint16_t)0x3000,       ///< Mask used to retrieve the tonic key variation
        tonicKeyAndVariationMask    = (uint16_t)0x3f00,       ///< Mask used to retrieve the tonic key and variation
        bassNoteKeyMask             = (uint16_t)0xf,          ///< Mask used to retrieve the bass note key
        bassNoteKeyVariationMask    = (uint16_t)0x30,         ///< Mask used to retrieve the bass note key variation
        bassNoteKeyAndVariationMask = (uint16_t)0x3f          ///< Mask used to retrieve the bass note key and variation
    };

    enum formulaFlags
    {
        noChord             = (uint8_t)0x10,                 ///< No Chord symbol
        brackets            = (uint8_t)0x20,                 ///< Places brackets around the chord name
        bassNoteSharps      = (uint8_t)0x40,                 ///< Bass note uses sharps vs flats (used for compatibility only)
        tonicSharps         = (uint8_t)0x80,                 ///< Tonic uses sharps vs flats (used for compatibility only)
        formulaFlagsMask    = (uint8_t)0xf0,                 ///< Mask used to retrieve all formula flags
        formulaMask         = (uint8_t)0x0f                  ///< Mask used to retrieve the formula
    };

    enum formula
    {
        major                       = (uint8_t)0x00,
        minor                       = (uint8_t)0x01,
        augmented                   = (uint8_t)0x02,
        diminished                  = (uint8_t)0x03,
        powerChord                  = (uint8_t)0x04,
        major6th                    = (uint8_t)0x05,
        minor6th                    = (uint8_t)0x06,
        dominant7th                 = (uint8_t)0x07,
        major7th                    = (uint8_t)0x08,
        minor7th                    = (uint8_t)0x09,
        augmented7th                = (uint8_t)0x0a,
        diminished7th               = (uint8_t)0x0b,
        minorMajor7th               = (uint8_t)0x0c,
        minor7thFlatted5th          = (uint8_t)0x0d
    };

    enum formulaModifications
    {
        extended9th	    = (uint16_t)0x01,
        extended11th	= (uint16_t)0x02,
        extended13th	= (uint16_t)0x04,
        added2nd	    = (uint16_t)0x08,
        added4th	    = (uint16_t)0x10,
        added6th	    = (uint16_t)0x20,
        added9th	    = (uint16_t)0x40,
        added11th	    = (uint16_t)0x80,
        flatted5th	    = (uint16_t)0x100,
        raised5th	    = (uint16_t)0x200,
        flatted9th	    = (uint16_t)0x400,
        raised9th	    = (uint16_t)0x800,
        raised11th	    = (uint16_t)0x1000,
        flatted13th	    = (uint16_t)0x2000,
        suspended2nd	= (uint16_t)0x4000,
        suspended4th	= (uint16_t)0x8000
    };

    enum extraFlags
    {
        fretPositionNotUsed     = (uint8_t)0xff,             ///< Value to indicate the fret position is not used
        fretPositionMask        = (uint8_t)0x1f,             ///< Mask used to retrieve the fret position
        typeNotUsed             = (uint8_t)0xff,             ///< Value to indicate the type is not used
        typeMask                = (uint8_t)0xe0              ///< Mask used to retrieve the type
    };

// Member Variables
private:
    uint16_t m_key;                                           ///< Chord key and bass note (hiword = tonic, loword = bass note; bits 5-6 = variation, bottom 4 bits = key)
    uint8_t m_formula;                                       ///< Core formula + flags (see formula and formulaFlags enums for values)
    uint16_t m_formulaModifications;                          ///< Stores the formula modifications (see formulaModifications enum for values)
    uint8_t m_extra;                                         ///< Stores the type and fret position data (top 3 bits = type, bottom 5 bits = position)

// Constructor/Destructor
public:
    ChordName();
    ChordName(uint8_t tonicKey, uint8_t tonicKeyVariation, uint8_t formula,
        uint16_t formulaAdditions, uint8_t fretPosition = fretPositionNotUsed,
        uint8_t type = typeNotUsed);
    ChordName(const ChordName& chordName);
    ~ChordName();

// Operators
    const ChordName& operator=(const ChordName& chordName);
    bool operator==(const ChordName& chordName) const;
    bool operator!=(const ChordName& chordName) const;

// Serialize Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {
        return "CChordName";
    }

    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}

// Key Functions
    /// Determines if a key is valid
    /// @param key Key to validate
    /// @return True if the key is valid, false if not
    static bool IsValidKey(uint8_t key)
        {return (key <= B);}
    /// Determines if a key variation is valid
    /// @param keyVariation Key variation to validate
    /// @return True if the key variation is valid, false if not
    static bool IsValidKeyVariation(uint8_t keyVariation)
        {return (keyVariation <= variationUp);}
    /// Determines if a key and key variation are valid
    /// @param key Key to validate
    /// @param keyVariation Key variation to validate
    /// @return True if the key and key variation are valid, false if not
    static bool IsValidKeyAndVariation(uint8_t key, uint8_t keyVariation)
    {
        return ((IsValidKey(key) && IsValidKeyVariation(keyVariation)) &&
            !(key == AFlat && keyVariation == variationUp));
    }

    std::string GetKeyText(bool getBassNote) const;

// Tonic Functions
    bool SetTonic(uint8_t key, uint8_t keyVariation);
    void GetTonic(uint8_t& key, uint8_t& keyVariation) const;
    bool IsSameTonic(uint8_t key, uint8_t keyVariation) const;

// Bass Note Functions
    bool SetBassNote(uint8_t key, uint8_t keyVariation);
    void GetBassNote(uint8_t& key, uint8_t& keyVariation) const;
    bool IsSameBassNote(uint8_t key, uint8_t keyVariation) const;

    bool TonicMatchesBassNote() const;

// Formula Functions
    /// Determines if a formula is valid
    /// @param formula Formula to validate
    /// @return True if the formula is valid, false if not
    static bool IsValidFormula(uint8_t formula)
        {return (formula <= minor7thFlatted5th);}
    bool SetFormula(uint8_t formula);
    uint8_t GetFormula() const;
    std::string GetFormulaText() const;

// Brackets Functions
    /// Sets or clears the brackets
    /// @return True if the brackets were set or cleared, false if not
    bool SetBrackets(bool set = true)
    {
        if (!set)
            return (ClearFormulaFlag(brackets));
        return (SetFormulaFlag(brackets));
    }
    /// Determines if the chord name is bracketed
    /// @return True if the chord name is bracketed, false if not
    bool HasBrackets() const
        {return (IsFormulaFlagSet(brackets));}

// No Chord Functions
    /// Sets or clears the no chord (N.C.) type
    /// @param set True to set the no chord type, false to clear it
    /// @return True if the no chord type was set or cleared, false if not
    bool SetNoChord(bool set = true)
    {
        if (!set)
            return (ClearFormulaFlag(noChord));
        return (SetFormulaFlag(noChord));
    }
    /// Determines if the chord name is the "no chord" (N.C.)
    /// @return True if the chord name is the "no chord"
    bool IsNoChord() const
        {return (IsFormulaFlagSet(noChord));}

// Formula Flag Functions
private:
    /// Determines if a formula flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidFormulaFlag(uint8_t flag)
    {
        return (((flag & formulaFlagsMask) != 0) &&
            ((flag & ~formulaFlagsMask) == 0));
    }
    /// Sets a formula flag
    /// @param flag Flag to set
    /// @return True if the flag was set, false if not
    bool SetFormulaFlag(uint8_t flag)
    {
        PTB_CHECK_THAT(IsValidFormulaFlag(flag), false);
        m_formula |= flag;
        return (true);
    }
    /// Clears a formula flag
    /// @param flag Flag to clear
    /// @return True if the flag was cleared, false if not
    bool ClearFormulaFlag(uint8_t flag)
    {
        PTB_CHECK_THAT(IsValidFormulaFlag(flag), false);
        m_formula &= ~flag;
        return (true);
    }
    /// Determines if a formula flag is set
    /// @param flag Flag to test
    /// @return True if the flag is set, false if not
    bool IsFormulaFlagSet(uint8_t flag) const
        {return ((m_formula & flag) == flag);}

// Formula Modifications Functions
public:
    /// Sets the Formula Modifications (see formulaModifications enum for
    /// values)
    /// @param formulaModifications Formula modifications to set
    void SetFormulaModifications(uint16_t formulaModifications)
        {m_formulaModifications = formulaModifications;}
    uint32_t GetFormulaModificationsCount() const;
    /// Clears all formula modifications
    void ClearFormulaModifications()
        {m_formulaModifications = 0;}
    /// Determines if a formula modification flag is set
    /// @param flag Formula modification flag to test
    /// @return True if the formula modification flag is set, false if not
    bool IsFormulaModificationFlagSet(uint16_t flag) const
        {return ((m_formulaModifications & flag) == flag);}

    /// Sets a formula modifications flag
    /// @param flag Flag to set
    void SetFormulaModificationFlag(uint16_t flag)
        {m_formulaModifications |= flag;}
    /// Clears a formula modifications flag
    /// @param flag Flag to clear
    void ClearFormulaModificationFlag(uint16_t flag)
        {m_formulaModifications &= ~flag;}

// Fret Position Functions
public:
    /// Determines if a fret position is valid
    /// @param fretPosition Fret position to validate
    /// @return True if the fret position is valid, false is not
    static bool IsValidFretPosition(uint8_t fretPosition)
    {
        return (((fretPosition >= MIN_FRET_POSITION) &&
            (fretPosition <= MAX_FRET_POSITION)) ||
            (fretPosition == fretPositionNotUsed));
    }
    bool SetFretPosition(uint8_t fretPosition);
    uint8_t GetFretPosition() const;
    /// Determines if the fret position is being used
    /// @return True if the fret position is being used, false if not
    bool IsFretPositionUsed() const
        {return (GetFretPosition() != fretPositionNotUsed);}

// Type Functions
    /// Determines if a type is valid
    /// @param type Type to validate
    /// @return True if the type is valid, false if not
    static bool IsValidType(uint8_t type)
    {
        return (((type >= MIN_TYPE) && (type <= MAX_TYPE)) ||
            (type == typeNotUsed));
    }
    bool SetType(uint8_t type);
    uint8_t GetType() const;
    /// Determines if the type is being used
    /// @return True if the type is being used, false if not
    bool IsTypeUsed() const
        {return (GetType() != typeNotUsed);}

    std::string GetText() const;

    static void ComputeKeyAndVariation(uint8_t &key, uint8_t &variation,
            uint8_t currentKey, bool sharp, bool doubleSharp, bool flat,
            bool doubleFlat);
};

}

#endif // CHORDNAME_H
