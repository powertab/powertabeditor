/////////////////////////////////////////////////////////////////////////////
// Name:            guitar.h
// Purpose:         Stores information about a guitar (tuning, effects, etc.)
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 8, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef POWERTABDOCUMENT_GUITAR_H
#define POWERTABDOCUMENT_GUITAR_H

#include "powertabobject.h"
#include "tuning.h"

namespace PowerTabDocument {

/// Stores information about a guitar (tuning, effects, etc.)
class Guitar : public PowerTabObject
{
    // Constants
public:
    // Default constants
    static const uint8_t         DEFAULT_NUMBER;         ///< Default value for the number member variable
    static const std::string     DEFAULT_DESCRIPTION;    ///< Default value for the description member variable
    static const uint8_t         DEFAULT_PRESET;         ///< Default value for the preset member variable
    static const uint8_t         DEFAULT_INITIAL_VOLUME; ///< Default value for the intial volume member variable
    static const uint8_t         DEFAULT_PAN;            ///< Default value for the pan member variable
    static const uint8_t         DEFAULT_REVERB;         ///< Default value for the reverb member variable
    static const uint8_t         DEFAULT_CHORUS;         ///< Default value for the chorus member variable
    static const uint8_t         DEFAULT_TREMOLO;        ///< Default value for the tremolo member variable
    static const uint8_t         DEFAULT_PHASER;         ///< Default value for the phaser member variable
    static const uint8_t         DEFAULT_CAPO;           ///< Default value for the capo member variable

    static const uint8_t         MIN_PRESET;             ///< Minimum allowed value for the preset
    static const uint8_t         MAX_PRESET;             ///< Maximum allowed value for the preset
    
    static const uint8_t         MIN_INITIAL_VOLUME;     ///< Minimum allowed value for the initial volume
    static const uint8_t         MAX_INITIAL_VOLUME;     ///< Maximum allowed value for the initial volume
    
    static const uint8_t         MIN_PAN;                ///< Minimum allowed value for the pan level
    static const uint8_t         MAX_PAN;                ///< Maximum allowed value for the pan level
    static const uint8_t         PAN_HARD_LEFT;          ///< Value for the hard left pan setting
    static const uint8_t         PAN_CENTER;             ///< Value for the center/middle pan setting
    static const uint8_t         PAN_HARD_RIGHT;         ///< Value for the hard right pan setting
    
    static const uint8_t         MIN_REVERB;             ///< Minimum allowed value for the reverb level
    static const uint8_t         MAX_REVERB;             ///< Maximum allowed value for the reverb level

    static const uint8_t         MIN_CHORUS;             ///< Minimum allowed value for the chorus level
    static const uint8_t         MAX_CHORUS;             ///< Maximum allowed value for the chorus level

    static const uint8_t         MIN_TREMOLO;            ///< Minimum allowed value for the tremolo level
    static const uint8_t         MAX_TREMOLO;            ///< Maximum allowed value for the tremolo level
    
    static const uint8_t         MIN_PHASER;             ///< Minimum allowed value for the phaser level
    static const uint8_t         MAX_PHASER;             ///< Maximum allowed value for the phaser level

    static const uint8_t         MIN_CAPO;               ///< Minimum allowed value for the capo number
    static const uint8_t         MAX_CAPO;               ///< Maximum allowed value for the capo number

    // Member Variables
private:
    uint8_t      m_number;                   ///< Unique number used to identify the guitar (zero-based)
    std::string  m_description;              ///< Description of the guitar
    uint8_t      m_preset;                   ///< MIDI preset to use during playback
    uint8_t      m_initialVolume;            ///< Initial MIDI channel volume level for the guitar
    uint8_t      m_pan;                      ///< Channel pan setting for the guitar
    uint8_t      m_reverb;                   ///< Amount of reverb effect used by the guitar
    uint8_t      m_chorus;                   ///< Amount of chorus effect used by the guitar
    uint8_t      m_tremolo;                  ///< Amount of tremolo effect used by the guitar
    uint8_t      m_phaser;                   ///< Amount of phaser effect used by the guitar
    uint8_t      m_capo;                     ///< Capo placement (0 - no capo)
    Tuning       m_tuning;                   ///< Guitar tuning
    bool         m_isShown;

    // Constructor/Destructor
public:
    Guitar();
    
    // Serialization Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {
        return "CGuitar";
    }
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {
        return 1;
    }

    // Number Functions
    void SetNumber(uint8_t number);
    uint8_t GetNumber() const;

    // Description Functions
    void SetDescription(const std::string& description);
    std::string GetDescription() const;

    // Preset Functions
    static bool IsValidPreset(uint8_t preset);
    bool SetPreset(uint8_t preset);
    uint8_t GetPreset() const;

    // Initial Volume Functions
    static bool IsValidInitialVolume(uint8_t initialVolume);
    bool SetInitialVolume(uint8_t initialVolume);
    uint8_t GetInitialVolume() const;

    // Pan Functions
    static bool IsValidPan(uint8_t pan);
    bool SetPan(uint8_t pan);
    uint8_t GetPan() const;

    // Reverb Functions
    static bool IsValidReverb(uint8_t reverb);
    bool SetReverb(uint8_t reverb);
    uint8_t GetReverb() const;

    // Chorus Functions
    static bool IsValidChorus(uint8_t chorus);
    bool SetChorus(uint8_t chorus);
    uint8_t GetChorus() const;

    // Tremolo Functions
    static bool IsValidTremolo(uint8_t tremolo);
    bool SetTremolo(uint8_t tremolo);
    uint8_t GetTremolo() const;

    // Phaser Functions
    static bool IsValidPhaser(uint8_t phaser);
    bool SetPhaser(uint8_t phaser);
    uint8_t GetPhaser() const;

    // Capo Functions
    static bool IsValidCapo(uint8_t capo);
    bool SetCapo(uint8_t capo);
    uint8_t GetCapo() const;
    bool UsesCapo() const;

    // Tuning Functions
    Tuning& GetTuning();
    const Tuning& GetTuning() const;

    size_t GetStringCount() const;

    bool IsShown() const;
    void SetShown(bool set = true);
};

}

#endif // POWERTABDOCUMENT_GUITAR_H
