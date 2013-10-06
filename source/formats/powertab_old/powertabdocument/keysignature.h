/////////////////////////////////////////////////////////////////////////////
// Name:            keysignature.h
// Purpose:         Stores and renders a key signature
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 10, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef KEYSIGNATURE_H
#define KEYSIGNATURE_H

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders a key signature
class KeySignature : public PowerTabObject
{
// Constants
public:
    // Default constants
    static const uint8_t         DEFAULT_DATA;       ///< Default value for the data member variable
    static const uint8_t         ACCIDENTAL_WIDTH;   ///< Display width of an accidental

    enum flags
    {
        keyAccidentalsMask      = (uint8_t)0x0f,     ///< Bit mask used to retrieve the key accidentals value
        show                    = (uint8_t)0x10,     ///< Key signature is shown
        cancellation            = (uint8_t)0x20,     ///< Key signature is a cancellation
        keyTypeMask             = (uint8_t)0x40      ///< Bit mask used to retrieve the key type value
    };

    enum keyTypes
    {
        majorKey                = (uint8_t)0,        ///< Key signature is a major key type (i.e. C major)
        minorKey                = (uint8_t)1         ///< Key signature is a minor key type (i.e. A minor)
    };

    enum keyAccidentals
    {
        noAccidentals           = (uint8_t)0,        ///< Key signature uses no accidentals
        oneSharp                = (uint8_t)1,        ///< Key signature uses 1 sharp
        twoSharps               = (uint8_t)2,        ///< Key signature uses 2 sharps
        threeSharps             = (uint8_t)3,        ///< Key signature uses 3 sharps
        fourSharps              = (uint8_t)4,        ///< Key signature uses 4 sharps
        fiveSharps              = (uint8_t)5,        ///< Key signature uses 5 sharps
        sixSharps               = (uint8_t)6,        ///< Key signature uses 6 sharps
        sevenSharps             = (uint8_t)7,        ///< Key signature uses 7 sharps
        oneFlat                 = (uint8_t)8,        ///< Key signature uses 1 flat
        twoFlats                = (uint8_t)9,        ///< Key signature uses 2 flats
        threeFlats              = (uint8_t)10,       ///< Key signature uses 3 flats
        fourFlats               = (uint8_t)11,       ///< Key signature uses 4 flats
        fiveFlats               = (uint8_t)12,       ///< Key signature uses 5 flats
        sixFlats                = (uint8_t)13,       ///< Key signature uses 6 flats
        sevenFlats              = (uint8_t)14        ///< Key signature uses 7 flats
    };

// Member Variables
private:
    uint8_t m_data;      ///< Stores all data required by the key signature (see the flags enum for the meaning of the individual bits)

// Constructor/Destructor
public:
    KeySignature();
    KeySignature(uint8_t keyType, uint8_t keyAccidentals);
    KeySignature(const KeySignature& keySignature);
    ~KeySignature();

// Operators
    const KeySignature& operator=(const KeySignature& keySignature);
    bool operator==(const KeySignature& keySignature) const;
    bool operator!=(const KeySignature& keySignature) const;

// MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CKeySignature";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}

// Serialization Functions
        bool Serialize(PowerTabOutputStream &stream) const override;
        bool Deserialize(PowerTabInputStream &stream,
                         uint16_t version) override;

// Key Functions
public:
    bool SetKey(uint8_t keyType, uint8_t keyAccidentals);
    void GetKey(uint8_t & keyType, uint8_t & keyAccidentals) const;
    /// Determines if the key (type + accidentals) is the same as that of
    /// another KeySignature object
    /// @param keySignature KeySignature object to compare with
    /// @return True if the key signatures have the same key, false if not
    bool IsSameKey(const KeySignature& keySignature) const
    {
        return ((GetKeyType() == keySignature.GetKeyType()) &&
            (GetKeyAccidentals() == keySignature.GetKeyAccidentals()));
    }
    bool GetDrawKey(uint8_t& keyType, uint8_t& keyAccidentals) const;

// Type Functions
    /// Deteremines if a key type is valid
    /// @param keyType The key type to validate
    /// @return True if the key type is valid, false if not
    static bool IsValidKeyType(uint8_t keyType)
        {return ((keyType == majorKey) || (keyType == minorKey));}
    bool SetKeyType(uint8_t keyType);
    /// Gets the key type for the key signature (majorKey vs minorKey)
    /// @return majorKey (see enum) if the key signature is a majorKey key,
    /// minorKey (see enum) if the key signature is a minorKey key
    uint8_t GetKeyType() const
        {return (uint8_t)((m_data & keyTypeMask) >> 6);}
    /// Determines if the key signature is a majorKey key
    /// @return True if the key signature is a majorKey key, false if not
    bool IsMajorKey() const
        {return (GetKeyType() == majorKey);}
    /// Determines if the key signature is a minorKey key
    /// @return True if the key signature is a minorKey key, false if not
    bool IsMinorKey() const
        {return (GetKeyType() == minorKey);}

// Accidentals Functions
    /// Determines if a key accidentals value is valid
    /// @param keyAccidentals Key accidentals value to validate
    /// @return True if the key accidentals value is validate, false if not
    static bool IsValidKeyAccidentals(uint8_t keyAccidentals)
        {return (keyAccidentals <= sevenFlats);}
    bool SetKeyAccidentals(uint8_t keyAccidentals);
    uint8_t GetKeyAccidentals() const;
    uint8_t GetKeyAccidentalsIncludingCancel() const;
    /// Determines if the key signature uses no accidentals
    /// @return True if the key signature uses no accidentals, false if not
    bool HasNoKeyAccidentals() const
        {return (GetKeyAccidentals() == noAccidentals);}

    /// Determines if the key signature uses flats
    /// @return True if the key signature uses flats, false if not
    bool UsesFlats() const
    {
        uint8_t key = 0, accidentals = 0;
        GetDrawKey(key, accidentals);
        return (accidentals >= oneFlat && accidentals <= sevenFlats);
    }
    /// Determines if the key signature uses sharps
    /// @return True if the key signature uses sharps, false if not
    bool UsesSharps() const
    {
        uint8_t key = 0, accidentals = 0;
        GetDrawKey(key, accidentals);
        return (accidentals >= oneSharp && accidentals <= sevenSharps);
    }

    /// Determines the number of accidentals in the key signature
    /// @return The number of accidentals in the key signature
    int NumberOfAccidentals() const
    {
        if (UsesFlats())
        {
            return GetKeyAccidentals() - oneFlat + 1;
        }
        else if (UsesSharps())
        {
            return GetKeyAccidentals() - oneSharp + 1;
        }
        else
        {
            return 0;
        }
    }

// Show/Hide Functions
    /// Makes the key signature visible
    void Show()
        {SetFlag(show);}
    /// Makes the key signature invisible
    void Hide()
        {ClearFlag(show);}
    /// Determines if the key signature is shown
    /// @return True if the key signature is shown, false if not
    bool IsShown() const
        {return (IsFlagSet(show));}

    void SetShown(bool set)
    {
        if (set)
        {
            Show();
        }
        else
        {
            Hide();
        }
    }

// Cancellation Functions
    /// Sets or clears the cancellation on a key signature
    /// @param set If true, makes the key signature a cancellation, otherwise
    /// clears the cancellation
    void SetCancellation(bool set = true)
        {if (set) SetFlag(cancellation); else ClearFlag(cancellation);}
    /// Determines if the key signature is a cancellation
    /// @return True if the key signature is a cancellation, false if not
    bool IsCancellation() const
        {return (IsFlagSet(cancellation));}

// Operations
    int GetWidth() const;

// Flag Functions
private:
    /// Determines if a flag is valid
    /// @param flag Flag to validate
    /// @return True if the flag is valid, false if not
    static bool IsValidFlag(uint8_t flag)
        {return ((flag == show) || (flag == cancellation));}
    /// Sets a flag used by the KeySignature object
    /// @param flag The flag to set
    bool SetFlag(uint8_t flag)
        {PTB_CHECK_THAT(IsValidFlag(flag), false); m_data |= flag; return (true);}
    /// Clears a flag used by the KeySignature object
    /// @param flag The flag to clear
    bool ClearFlag(uint8_t flag)
        {PTB_CHECK_THAT(IsValidFlag(flag), false); m_data &= ~flag; return (true);}
    /// Determines if a flag used by the KeySignature object is set
    /// @param flag The flag to test
    /// @return True if the flag is set, false if not
    bool IsFlagSet(uint8_t flag) const
        {PTB_CHECK_THAT(IsValidFlag(flag), false); return ((m_data & flag) == flag);}
};

}

#endif // KEYSIGNATURE_H
