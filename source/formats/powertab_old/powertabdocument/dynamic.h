/////////////////////////////////////////////////////////////////////////////
// Name:            dynamic.h
// Purpose:         Stores and renders a dynamic
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 13, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Stores and renders a dynamic
class Dynamic : public PowerTabObject
{
// Constants
public:
    // Default constants
    static const uint16_t         DEFAULT_SYSTEM;                     ///< Default value for the system member variable
    static const uint8_t         DEFAULT_STAFF;                      ///< Default value for the staff member variable
    static const uint8_t         DEFAULT_POSITION;                   ///< Default value for the position member variable
    static const uint16_t         DEFAULT_DATA;                       ///< Default value for the data member variable
    static const uint8_t         DEFAULT_STAFF_VOLUME;               ///< Default value for the staff volume
    static const uint8_t         DEFAULT_RHYTHM_SLASH_VOLUME;        ///< Default value for the dynamic volume
    
    // System Constants
    static const uint32_t       MIN_SYSTEM;                         ///< Minimum allowed value for the system member variable
    static const uint32_t       MAX_SYSTEM;                         ///< Maximum allowed value for the system member variable

    // Staff Constants
    static const uint32_t       MIN_STAFF;                          ///< Minimum allowed value for the staff member variable
    static const uint32_t       MAX_STAFF;                          ///< Maximum allowed value for the staff member variable

    // Position Constants
    static const uint32_t       MIN_POSITION;                       ///< Minimum allowed value for the position member variable
    static const uint32_t       MAX_POSITION;                       ///< Maximum allowed value for the position member variable

    // Volume constants
    enum VolumeLevel
    {
        notSet  = (uint8_t)0xff,
        fff     = (uint8_t)104,
        ff      = (uint8_t)91,
        f       = (uint8_t)78,
        mf      = (uint8_t)65,
        mp      = (uint8_t)52,
        p       = (uint8_t)39,
        pp      = (uint8_t)26,
        ppp     = (uint8_t)13,
        off     = (uint8_t)0
    };

// Member Variables
private:
    uint16_t      m_system;       ///< Zero-based index of the system where the dynamic is anchored
    uint8_t      m_staff;        ///< Zero-based index of the staff within the system where the dynamic is anchored
    uint8_t      m_position;     ///< Zero-based index of the position within the system where the dynamic is anchored
    uint16_t      m_data;         ///< Volume level (see volumeLevels enum for values; top byte = staff volume, bottom byte = dynamic volume)

// Constructors/Destructors
public:
    Dynamic();
    Dynamic(uint32_t system, uint32_t staff, uint32_t position,
        uint8_t staffVolume, uint8_t rhythmSlashVolume);
    Dynamic(const Dynamic& dynamic);
    ~Dynamic();
    
// Operators
    const Dynamic& operator=(const Dynamic& dynamic);
    bool operator==(const Dynamic& dynamic) const;
    bool operator!=(const Dynamic& dynamic) const;
    
// Serialization functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CDynamic";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}
    
// System Functions
    /// Determines whether a system is valid
    /// @param system System to validate
    /// @return True if the system is valid, false if not
    static bool IsValidSystem(uint32_t system)
        {return ((system >= MIN_SYSTEM) && (system <= MAX_SYSTEM));}
    /// Sets the system within the system where the dynamic is anchored
    /// @param system Zero-based index of the system where the dynamic is
    /// anchored
    /// @return True if the system was set, false if not
    bool SetSystem(uint32_t system)
    {
        PTB_CHECK_THAT(IsValidSystem(system), false);
        m_system = (uint16_t)system;
        return (true);
    }
    /// Gets the system within the system where the dynamic is anchored
    /// @return The system within the system where the dynamic is anchored
    uint32_t GetSystem() const                           
        {return (m_system);}
        
// Staff Functions
    /// Determines whether a staff is valid
    /// @param staff Staff to validate
    /// @return True if the staff is valid, false if not
    static bool IsValidStaff(uint32_t staff)
        {return ((staff >= MIN_STAFF) && (staff <= MAX_STAFF));}
    /// Sets the staff within the system where the dynamic is anchored
    /// @param staff Zero-based index of the staff where the dynamic is anchored
    /// @return True if the staff was set, false if not
    bool SetStaff(uint32_t staff)
    {
        PTB_CHECK_THAT(IsValidStaff(staff), false);
        m_staff = (uint8_t)staff;
        return (true);
    }
    /// Gets the staff within the system where the dynamic is anchored
    /// @return The staff within the system where the dynamic is anchored
    uint32_t GetStaff() const                           
        {return (m_staff);}
        
// Position Functions
    /// Determines whether a position is valid
    /// @param position Position to validate
    /// @return True if the position is valid, false if not
    static bool IsValidPosition(uint32_t position)
        {return ((position >= MIN_POSITION) && (position <= MAX_POSITION));}
    /// Sets the position within the system where the dynamic is anchored
    /// @param position Zero-based index of the position within the system where
    /// the dynamic is anchored
    /// @return True if the position was set, false if not
    bool SetPosition(uint32_t position)
    {
        PTB_CHECK_THAT(IsValidPosition(position), false);
        m_position = (uint8_t)position;
        return (true);
    }
    /// Gets the position within the system where the dynamic is anchored
    /// @return The position within the system where the dynamic is anchored
    uint32_t GetPosition() const                           
        {return (m_position);}
    
// Volume Functions
    /// Determines if a volume is valid
    /// @param volume Volume to validate
    /// @return True if the volume is valid, false if not
    static bool IsValidVolume(uint8_t volume)
    {
        return ((volume == notSet) || (volume == fff) || (volume == ff) ||
            (volume == f) || (volume == mf) || (volume == mp) ||
            (volume == p) || (volume == pp) || (volume == ppp) ||
            (volume == off));
    }
// Staff Volume Functions
    /// Sets the staff volume
    /// @param volume Volume to set
    /// @return True if the volume was set, false if not
    bool SetStaffVolume(uint8_t volume)
        {return (SetVolume(false, volume));}
    /// Gets the staff volume
    /// @return The staff volume
    uint8_t GetStaffVolume() const
        {return (GetVolume(false));}
    /// Determines if the staff volume is set
    /// @return True if the staff volume is set, false if not
    bool IsStaffVolumeSet() const
        {return (IsVolumeSet(false));}

// Rhythm Slash Functions
    bool SetRhythmSlashVolume(uint8_t volume)
        {return (SetVolume(true, volume));}
    /// Gets the dynamic volume
    /// @return The dynamic volume
    uint8_t GetRhythmSlashVolume() const
        {return (GetVolume(true));}
    /// Determines if the dynamic volume is set
    /// @return True if the dynamic volume is set, false if not
    bool IsRhythmSlashVolumeSet() const
        {return (IsVolumeSet(true));}
private:
    bool SetVolume(bool rhythmSlashes, uint8_t volume);
    uint8_t GetVolume(bool rhythmSlashes) const;
    bool IsVolumeSet(bool rhythmSlashes) const;

// Operations
public:
    std::string GetText(bool rhythmSlashes) const;
};

}

#endif // DYNAMIC_H
