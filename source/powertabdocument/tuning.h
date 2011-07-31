/////////////////////////////////////////////////////////////////////////////
// Name:            tuning.h
// Purpose:         Stores a tuning used by a stringed instrument
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 14, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef TUNING_H
#define TUNING_H

#include <vector>
#include <string>
#include <initializer_list>

#include "powertabobject.h"
#include "macros.h"

/// Stores a tuning used by a stringed instrument
class Tuning : public PowerTabObject
{
public:
    static const std::string    DEFAULT_NAME;                       ///< Default value for the name member variable
    static const uint8_t        DEFAULT_DATA;                       ///< Default value for the data member variable

    static const int8_t         MIN_MUSIC_NOTATION_OFFSET;          ///< Minimum allowed value for the music notation offset
    static const int8_t         MAX_MUSIC_NOTATION_OFFSET;          ///< Maximum allowed value for the music notation offset

    static const uint32_t       MIN_STRING_COUNT;                   ///< Minimum number of strings in a tuning
    static const uint32_t       MAX_STRING_COUNT;                   ///< Maximum number of strings in a tuning

    enum flags
    {
        sharpsMask                      = (uint8_t)0x01,             ///< Mask used to retrieve the sharp setting (display tuning name using sharps or flats)
        musicNotationOffsetMask         = (uint8_t)0xfe,             ///< Mask used to retrieve the music notation offset (sign + value)
        musicNotationOffsetSignMask     = (uint8_t)0x80,             ///< Mask used to retrieve the music notation offset sign (+/-)
        musicNotationOffsetValueMask    = (uint8_t)0x7e,             ///< Mask used to retrieve the music notation offset value
        notUsed                         = (uint8_t)0xff              ///< Marker that signifies an unused string
    };

    // Member Variables
protected:
    std::string             m_name;         ///< Name (or description) of the tuning
    uint8_t                 m_data;         ///< bit 7 = Music notation offset sign, bits 6 to 1 = Music notation offset value, bit 0 = display sharps or flats
    std::vector<uint8_t>    m_noteArray;   ///< Array of bytes representing the MIDI notes of the tuning, ordered from high string to low string

    // Constructor/Destructor
public:
    Tuning();
    Tuning(const Tuning& tuning);

    Tuning(const std::string& name, int8_t musicNotationOffset, bool sharps,
           const std::vector<uint8_t>& tuningNotes);

    Tuning(const std::string& name, int8_t musicNotationOffset, bool sharps,
           std::initializer_list<uint8_t> tuningNotes);

    // Operators
    bool operator==(const Tuning& tuning) const;
    bool operator!=(const Tuning& tuning) const;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
    {return "CTuning";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
    {return ((uint16_t)1);}

    // Serialize Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    // Tuning Functions
public:
    bool IsSameTuning(const Tuning& tuning) const;

    // Name Functions
    /// Sets the tuning name (i.e. "Open G")
    /// @param name Name to set
    /// @return True if the name was successfully set, false if not
    void SetName(const std::string& name)
    {
        m_name = name;
    }

    /// Gets the tuning name (i.e. "Open G")
    /// @return The tuning name
    std::string GetName() const
    {
        return m_name;
    }

    // Music Notation Offset Functions
    /// Determines if a music notation offset value is valid
    /// @param musicNotationOffset Music notation offset to validate
    /// @return True if the music notation offset is valid, false if not
    static bool IsValidMusicNotationOffset(int8_t musicNotationOffset)
    {
        return (musicNotationOffset >= MIN_MUSIC_NOTATION_OFFSET) &&
                (musicNotationOffset <= MAX_MUSIC_NOTATION_OFFSET);
    }
    bool SetMusicNotationOffset(int8_t musicNotationOffset);
    int8_t GetMusicNotationOffset() const;

    // Sharps Functions
    void SetSharps(bool set = true);

    /// Determines if the tuning notes are displayed using sharps, or flats
    /// @return True if the tuning notes are displayed using sharps, false if
    /// flats
    bool UsesSharps() const
    {
        return (m_data & sharpsMask) == sharpsMask;
    }

    // String Functions
    /// Determines if a string count is valid
    /// @param stringCount String count to validate
    /// @return True if the string count is valid, false if not
    static bool IsValidStringCount(size_t stringCount)
    {
        return (stringCount >= MIN_STRING_COUNT) && (stringCount <= MAX_STRING_COUNT);
    }

    /// Determines if a string is valid
    /// @param string String to validate
    /// @return True if the string is valid, false if not
    bool IsValidString(uint32_t string) const
    {
        return string < GetStringCount();
    }

    /// Gets the number of strings used by the tuning
    /// @return The number of strings used by the tuning
    size_t GetStringCount() const
    {
        return m_noteArray.size();
    }

    // Note Functions
    bool SetNote(uint32_t string, uint8_t note);
    uint8_t GetNote(uint32_t string,
                    bool includeMusicNotationOffset = false) const;

    std::string GetNoteText(uint32_t string) const;
    bool IsOpenStringNote(uint8_t note) const;
    std::pair<uint8_t, uint8_t> GetNoteRange(uint8_t capo) const;

    bool SetTuningNotes(const std::vector<uint8_t>& tuningNotes);
    bool SetTuningNotes(std::initializer_list<uint8_t> tuningNotes);

    bool IsValid() const;

    std::string GetSpelling() const;

    void SetToStandard();
};

#endif // TUNING_H
