/////////////////////////////////////////////////////////////////////////////
// Name:            systemsymbol.h
// Purpose:         Base class for symbols residing on a system scope
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 13, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEMSYMBOL_H
#define SYSTEMSYMBOL_H

#include <cstdint>

#include "powertabobject.h"
#include "macros.h"

namespace PowerTabDocument {

/// Base class for symbols residing on a system scope
class SystemSymbol : public PowerTabObject
{
// Constants
public:
    // Default constants
    static const uint16_t         DEFAULT_SYSTEM;             ///< Default value for the system member variable
    static const uint8_t         DEFAULT_POSITION;           ///< Default value for the position member variable
    static const uint32_t       DEFAULT_DATA;               ///< Default value for the data member variable

    // System Constants
    static const uint32_t       MIN_SYSTEM;                 ///< Minimum allowed value for the system member variable
    static const uint32_t       MAX_SYSTEM;                 ///< Maximum allowed value for the system member variable

    // Position Constants
    static const uint32_t       MIN_POSITION;               ///< Minimum allowed value for the position member variable
    static const uint32_t       MAX_POSITION;               ///< Maximum allowed value for the position member variable

// Member Variables
protected:
    uint16_t      m_system;           ///< Zero-based index of the system the symbol is anchored
    uint8_t      m_position;         ///< Zero-based index of the position within the system where the symbol is anchored
    uint32_t    m_data;             ///< Data used by the symbol (different for each symbol)

// Constructor/Destructor
public:
    SystemSymbol();
    SystemSymbol(uint32_t system, uint32_t position, uint32_t symbol);
    SystemSymbol(const SystemSymbol& systemSymbol);
    virtual ~SystemSymbol();
    
// Operators
    const SystemSymbol& operator=(const SystemSymbol& systemSymbol);
    bool operator==(const SystemSymbol& systemSymbol) const;
    bool operator!=(const SystemSymbol& systemSymbol) const;
    bool operator<(const SystemSymbol& systemSymbol) const;

// Serialization Functions
    virtual bool Serialize(PowerTabOutputStream &stream) const override;
    virtual bool Deserialize(PowerTabInputStream &stream,
                             uint16_t version) override;

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CSectionSymbol";}
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
    /// Sets the system within the system where the system symbol is anchored
    /// @param system Zero-based index of the system where the system symbol is
    /// anchored
    /// @return True if the system was set, false if not
    bool SetSystem(uint32_t system)                         
    {
        PTB_CHECK_THAT(IsValidSystem(system), false);
        m_system = (uint16_t)system;
        return (true);
    }
    /// Gets the system within the system where the system symbol is anchored
    /// @return The system within the system where the system symbol is anchored
    uint32_t GetSystem() const                                
        {return (m_system);}

// Position Functions
    /// Determines whether a position is valid
    /// @param position Position to validate
    /// @return True if the position is valid, false if not
    static bool IsValidPosition(uint32_t position)              
        {return ((position >= MIN_POSITION) && (position <= MAX_POSITION));}
    /// Sets the position within the system where the system symbol is anchored
    /// @param position Zero-based index within the system where the system
    /// symbol is anchored
    /// @return True if the position was set, false if not
    bool SetPosition(uint32_t position)                         
    {
        PTB_CHECK_THAT(IsValidPosition(position), false);
        m_position = (uint8_t)position;
        return (true);
    }
    /// Gets the position within the system where the system symbol is anchored
    /// @return The position within the system where the system symbol is
    /// anchored
    uint32_t GetPosition() const                                
        {return (m_position);}
    
// Data Functions
    /// Sets the data associated with the object
    /// @param data Data to set
    void SetData(uint32_t data)
        {m_data = data;}
    /// Gets the data associated with the object
    /// @return The data associated with the object
    uint32_t GetData() const
        {return (m_data);}
};

}

#endif // SYSTEMSYMBOL_H
