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

#ifndef __GUITAR_H__
#define __GUITAR_H__

#include "powertabobject.h"
#include "tuning.h"
#include "macros.h"

/// Stores information about a guitar (tuning, effects, etc.)
class Guitar : public PowerTabObject
{
// Constants
public:
    // Default constants
    static const uint8_t         DEFAULT_NUMBER;         ///< Default value for the number member variable
    static const char*        DEFAULT_DESCRIPTION;    ///< Default value for the description member variable
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
protected:
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

// Constructor/Destructor
public:
    Guitar();
    Guitar(uint8_t number, const char* description = DEFAULT_DESCRIPTION,
        uint8_t preset = DEFAULT_PRESET,
        uint8_t initialVolume = DEFAULT_INITIAL_VOLUME,
        uint8_t pan = DEFAULT_PAN, uint8_t reverb = DEFAULT_REVERB,
        uint8_t chorus = DEFAULT_CHORUS, uint8_t tremolo = DEFAULT_TREMOLO,
        uint8_t phaser = DEFAULT_PHASER, uint8_t capo = DEFAULT_CAPO);
    Guitar(const Guitar& guitar);
    ~Guitar();
    
// Operators
    const Guitar& operator=(const Guitar& guitar);
    bool operator==(const Guitar& guitar) const;
    bool operator!=(const Guitar& guitar) const;
    
// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
        {return "CGuitar";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const                        
        {return ((uint16_t)1);}
        
// Number Functions
    /// Sets the number (id) used by the guitar
    /// @param number Number to set (zero based)
    void SetNumber(uint8_t number)                           
        {m_number = number;}
    /// Gets the number (id) used by the guitar
    /// @return The number used by the guitar
    uint8_t GetNumber() const                                
        {return (m_number);}

// Description Functions
    /// Sets the description for the guitar
    /// @param description Description to set
    void SetDescription(const std::string& description)
    {
        m_description = description;
    }
    /// Gets the description for the guitar (i.e. Acoustic w/Chorus)
    /// @return The description for the guitar
    std::string GetDescription() const
    {
        return m_description;
    }

// Preset Functions
    /// Determines if a preset is valid
    /// @param preset Preset to validate
    /// @return True if the preset is validate, false if not
    static bool IsValidPreset(uint8_t preset)                
        {return ((preset >= MIN_PRESET) && (preset <= MAX_PRESET));}
    /// Sets the preset (MIDI) used by the guitar
    /// @param preset Preset to set
    /// @return True if the preset was set, false if not
    bool SetPreset(uint8_t preset)                           
    {
        CHECK_THAT(IsValidPreset(preset), false);
        m_preset = preset;
        return (true);
    }
    /// Gets the preset (MIDI) used by the guitar
    /// @return The preset used by the guitar
    uint8_t GetPreset() const                                
        {return (m_preset);}

// Initial Volume Functions
    /// Determines if an initial volume is valid
    /// @param initialVolume Initial volume to validate
    /// @return True if the initial volume is valid, false if not
    static bool IsValidInitialVolume(uint8_t initialVolume)  
    {
        return ((initialVolume >= MIN_INITIAL_VOLUME) &&
            (initialVolume <= MAX_INITIAL_VOLUME));
    }
    /// Sets the initial volume level of the guitar
    /// @param initialVolume Initial volume to set
    /// @return True if the volume was set, false if not
    bool SetInitialVolume(uint8_t initialVolume)             
    {
        CHECK_THAT(IsValidInitialVolume(initialVolume), false);
        m_initialVolume = initialVolume;
        return (true);
    }
    /// Gets the initial volume level of the guitar
    /// @return The initial volume level of the guitar
    uint8_t GetInitialVolume() const                         
        {return (m_initialVolume);}

// Pan Functions
    /// Determines if a pan level is valid
    /// @param pan Pan level to validate
    /// @return True if the pan level is valid, false if not
    static bool IsValidPan(uint8_t pan)                      
        {return ((pan >= MIN_PAN) && (pan <= MAX_PAN));}
    /// Sets the pan level for the guitar
    /// @param pan Pan level to set (0 - hard left, 64 - center, 127 - hard
    /// right)
    /// @return True if the pan level was set, false if not
    bool SetPan(uint8_t pan)                                 
        {CHECK_THAT(IsValidPan(pan), false); m_pan = pan; return (true);}
    /// Gets the pan level for the guitar
    /// @return The pan level for the guitar
    uint8_t GetPan() const                                   
        {return (m_pan);}

// Reverb Functions
    /// Determines if a reverb level is valid
    /// @param reverb Reverb level to validate
    /// @return True if the reverb level is valid, false if not
    static bool IsValidReverb(uint8_t reverb)                
        {return ((reverb >= MIN_REVERB) && (reverb <= MAX_REVERB));}
    /// Sets the reverb level for the guitar
    /// @param reverb Reverb level to set (0 - none, 127 - max)
    /// @return True if the reverb level was set, false if not
    bool SetReverb(uint8_t reverb)                           
    {
        CHECK_THAT(IsValidReverb(reverb), false);
        m_reverb = reverb;
        return (true);
    }
    /// Gets the reverb level for the guitar
    /// @return The reverb level for the guitar
    uint8_t GetReverb() const                                
        {return (m_reverb);}

// Chorus Functions
    /// Determines if a chorus level is valid
    /// @param chorus Chorus level to validate
    /// @return True if the chorus level is valid, false if not
    static bool IsValidChorus(uint8_t chorus)                
        {return ((chorus >= MIN_CHORUS) && (chorus <= MAX_CHORUS));}
    /// Sets the chorus level for the guitar (0 - none, 127 - maximum chorus)
    /// @param chorus Chorus level to set
    /// @return True if the chorus level was set, false if not
    bool SetChorus(uint8_t chorus)                           
    {
        CHECK_THAT(IsValidChorus(chorus), false);
        m_chorus = chorus;
        return (true);
    }
    /// Gets the chorus level for the guitar
    /// @return The chorus level for the guitar
    uint8_t GetChorus() const                                
        {return (m_chorus);}

// Tremolo Functions
    /// Determines if a tremolo level is valid
    /// @param tremolo Tremolo level to validate
    /// @return True if the tremolo level is valid, false if not
    static bool IsValidTremolo(uint8_t tremolo)              
        {return ((tremolo >= MIN_TREMOLO) && (tremolo <= MAX_TREMOLO));}
    /// Sets the tremolo level for the guitar (0 - none, 127 - maximum tremolo)
    /// @param tremolo Tremolo level to set
    /// @return True if the tremolo level was set, false is not
    bool SetTremolo(uint8_t tremolo)                         
    {
        CHECK_THAT(IsValidTremolo(tremolo), false);
        m_tremolo = tremolo;
        return (true);
    }
    /// Gets the tremolo level for the guitar
    /// @return The tremolo level for the guitar
    uint8_t GetTremolo() const                               
        {return (m_tremolo);}

// Phaser Functions
    /// Determines if a phaser level is valid
    /// @param phaser Phaser level to validate
    /// @return True if the phaser level is valid, false if not
    static bool IsValidPhaser(uint8_t phaser)                
        {return ((phaser >= MIN_PHASER) && (phaser <= MAX_PHASER));}
    /// Sets the phaser level for the guitar (0 - none, 127 - maximum phaser)
    /// @param phaser Phaser level to set
    /// @return True if the phaser level was set, false is not
    bool SetPhaser(uint8_t phaser)                           
    {
        CHECK_THAT(IsValidPhaser(phaser), false);
        m_phaser = phaser;
        return (true);
    }
    /// Gets the phaser level for the guitar
    /// @return The phaser level for the guitar
    uint8_t GetPhaser() const                                
        {return (m_phaser);}

// Capo Functions
    /// Determines if a capo value is valid
    /// @param capo Capo value to validate
    /// @return True if the capo value is valid, false if not
    static bool IsValidCapo(uint8_t capo)                    
        {return ((capo >= MIN_CAPO) && (capo <= MAX_CAPO));}
    /// Sets the capo used by the guitar (0 - no capo, all other values
    /// represent the fret where the capo is placed)
    /// @param capo Capo value to set
    /// @return True if the capo was set, false if not
    bool SetCapo(uint8_t capo)                               
        {CHECK_THAT(IsValidCapo(capo), false); m_capo = capo; return (true);}
    /// Gets the capo setting used by the guitar
    /// @return The capo used by the guitar
    uint8_t GetCapo() const                                  
        {return (m_capo);}
    /// Determines if the guitar uses a capo
    /// @return True if the guitar uses a capo, false if not
    bool UsesCapo() const                                   
        {return (GetCapo() != MIN_CAPO);}

// Tuning Functions
    bool SetTuning(const Tuning& tuning);
    Tuning& GetTuning();
    const Tuning& GetTuning() const;

    size_t GetStringCount() const;
    std::string GetTuningSpelling() const;

// Operations
    std::string GetLegendText() const;
};

#endif
