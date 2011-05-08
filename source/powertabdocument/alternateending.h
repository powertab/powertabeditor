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

#ifndef __ALTERNATEENDING_H__
#define __ALTERNATEENDING_H__

#include <vector>
#include "systemsymbol.h"

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
    
// Constructor/Destructor
public:
    AlternateEnding();
    AlternateEnding(uint32_t system, uint32_t position, uint16_t numbers);
   
// Operators
    const AlternateEnding& operator=(const AlternateEnding& alternateEnding);
    bool operator==(const AlternateEnding& alternateEnding) const;
    bool operator!=(const AlternateEnding& alternateEnding) const;
    
// Serialization Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

// MFC Class Functions
public:    
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
    {
        return "CSectionSymbol";
    }
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const                            
    {
        return 1;
    }
    
// Number Functions
    static bool IsValidNumbers(uint16_t numbers);
    static bool IsValidNumber(uint32_t number);
    bool SetNumbers(uint16_t numbers);
    uint16_t GetNumbers() const;
    std::vector<uint8_t> GetListOfNumbers() const;
    bool SetNumber(uint32_t number);
    bool IsNumberSet(uint32_t number) const;
    bool ClearNumber(uint32_t number);
    
// Da Capo Functions
    void SetDaCapo()                                            
        {SetNumber(daCapo);}
    bool IsDaCapoSet() const                                    
        {return (IsNumberSet(daCapo));}
    void ClearDaCapo()                                          
        {ClearNumber(daCapo);}
    
// Dal Segno Functions
    void SetDalSegno()                                          
        {SetNumber(dalSegno);}
    bool IsDalSegnoSet() const                                  
        {return (IsNumberSet(dalSegno));}
    void ClearDalSegno()                                        
        {ClearNumber(dalSegno);}
    
// Dal Segno Segno Functions
    void SetDalSegnoSegno()
        {SetNumber(dalSegnoSegno);}
    bool IsDalSegnoSegnoSet() const
        {return (IsNumberSet(dalSegnoSegno));}
    void ClearDalSegnoSegno()
        {ClearNumber(dalSegnoSegno);}
    
// Operations
    std::string GetText() const;
    static std::string GetNumberText(uint32_t number);
};

#endif
