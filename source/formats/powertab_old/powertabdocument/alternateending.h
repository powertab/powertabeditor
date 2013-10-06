/////////////////////////////////////////////////////////////////////////////
// Name:            alternateending.h
// Purpose:         Stores and renders alternate ending symbols
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 3, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef ALTERNATEENDING_H
#define ALTERNATEENDING_H

#include <vector>
#include "systemsymbol.h"

namespace PowerTabDocument {

/// Stores and renders alternate ending symbols
class AlternateEnding : public SystemSymbol
{
// Constants
public:
    enum alternateEndingFlags
    {
        daCapo              =   (uint16_t)9,
        dalSegno            =   (uint16_t)10,
        dalSegnoSegno       =   (uint16_t)11,
        numbersMask         =   (uint16_t)0x7ff           ///< Mask used to retrieve all allowable numbers
    };
    
public:
    AlternateEnding();
    
    // Serialization Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {
        return "CSectionSymbol";
    }
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {
        return 1;
    }
    
    std::vector<uint8_t> GetListOfNumbers() const;
    bool IsNumberSet(uint32_t number) const;
    
    bool IsDaCapoSet() const                                    
        {return (IsNumberSet(daCapo));}
    bool IsDalSegnoSet() const                                  
        {return (IsNumberSet(dalSegno));}
    bool IsDalSegnoSegnoSet() const
        {return (IsNumberSet(dalSegnoSegno));}
};

}

#endif // ALTERNATEENDING_H
