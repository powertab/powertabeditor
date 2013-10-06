/////////////////////////////////////////////////////////////////////////////
// Name:            floatingtext.h
// Purpose:         Stores and renders text annotations
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 6, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef FLOATINGTEXT_H
#define FLOATINGTEXT_H

#include "fontsetting.h"
#include "powertabobject.h"
#include "macros.h"
#include "rect.h"

namespace PowerTabDocument {

/// Stores and renders text annotations
class FloatingText : public PowerTabObject
{
// Constants
public:
    // Default constants
    static const char*        DEFAULT_TEXT;               ///< Default value for the text member variable
    static const Rect         DEFAULT_RECT;               ///< Default value for the rect member variable
    static const uint8_t         DEFAULT_FLAGS;              ///< Default value for the flags member variable
    static const uint8_t         DEFAULT_ALIGNMENT;          ///< Default alignment value

    enum flags
    {
        alignLeft   = (uint8_t)0x01,             ///< Text aligned left
        alignCenter = (uint8_t)0x02,             ///< Text aligned center
        alignRight  = (uint8_t)0x04,             ///< Text aligned right
        alignMask   = (uint8_t)0x07,             ///< Mask used to extract alignment value
        border      = (uint8_t)0x08              ///< Text surrounded by border
    };
    
// Member Variables
private:
    std::string        m_text;                     ///< Text to be output
    Rect          m_rect;                     ///< Bounding rectangle for the text
    uint8_t          m_flags;                    ///< Flags representing alignment and borders (see flags below)
    FontSetting     m_fontSetting;              ///< Font setting (format) to use when drawing the text

// Constructor/Destructor
public:
    FloatingText();
    FloatingText(const char* text, Rect rect, uint8_t flags,
        const FontSetting& fontSetting);
    FloatingText(const FloatingText& floatingText);
    ~FloatingText();

// Operators
    const FloatingText& operator=(const FloatingText& floatingText);
    bool operator==(const FloatingText& floatingText) const;
    bool operator!=(const FloatingText& floatingText) const;

// Serialize Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

// MFC Class Functions
public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
        {return "CFloatingText";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
        uint16_t GetMFCClassSchema() const override
        {return ((uint16_t)1);}
    
// Text Functions
    /// Sets the text used by the FloatingText object
    /// @param text Text to set
    /// @return True if the text was set, false if not
    bool SetText(const char* text)                                
        {
            PTB_CHECK_THAT(text != nullptr, false);
            m_text = text;
            return (true);
        }
    /// Gets the text used by the FloatingText object
    /// @return The text used by the FloatingText object
    std::string GetText() const
        {return (m_text);}

// Rect Functions
    /// Sets the bounding rect for the text in the FloatingText object
    /// @param rect Rect to set
    void SetRect(Rect rect)                                       
        {m_rect = rect;}
    /// Gets the bounding rect for the text in the FloatingText object
    /// @return The rect that bounds the text in the FloatingText object
    Rect GetRect() const                                          
        {return (m_rect);}
    /// Determines if the bounding rect is empty
    bool IsEmpty() const                                            
        {return ((m_rect.GetWidth() == 0) && (m_rect.GetHeight() == 0));}

// Flag Functions
private:
    void SetFlag(uint8_t flag);
    /// Clears a flag used by the FloatingText object
    /// @param flag The flag to clear
    void ClearFlag(uint8_t flag)                                     
        {m_flags &= ~flag;}
    /// Determines if a flag is set in the FloatingText object
    /// @param flag The flag to test
    /// @return True if the flag is set, false if not
    bool IsFlagSet(uint8_t flag) const                               
        {return ((m_flags & flag) == flag);}

// Alignment Functions
public:
    /// Determines if a alignment value is valid
    /// @param alignment Text alignment to validate
    static bool IsValidAlignment(uint8_t alignment)                  
    {
        return ((alignment == alignLeft) || (alignment == alignCenter) ||
            (alignment == alignRight));
    }
    /// Sets the alignment used by the FloatingText object
    /// @param alignment Text alignment to set
    /// @return True if the alignment was successfully set, false if not
    bool SetAlignment(uint8_t alignment)                             
    {
        PTB_CHECK_THAT(IsValidAlignment(alignment), false);
        ClearFlag(alignMask);
        SetFlag(alignment);
        return (true);
    }
    /// Gets the alignment used by the FloatingText object
    /// @return The alignment used by the FloatingText object
    uint8_t GetAlignment() const                                     
        {return (uint8_t)(m_flags & FloatingText::alignMask);}
    /// Determines if the alignment used by the FloatingText object is in a
    /// particular alignment
    /// @param alignment Alignment to test
    /// @return True if the alignment used by the FloatingText object matches
    /// the one given by alignment, false if not
    bool IsAligned(uint8_t alignment) const                          
    {
        PTB_CHECK_THAT(IsValidAlignment(alignment), false);
        return (IsFlagSet(alignment));
    }
    /// Determines if the alignment used by the FloatingText object is left
    /// aligned
    /// @return True if the alignment used by the FloatingText object is left
    /// aligned, false if not
    bool IsAlignedLeft() const                                      
        {return (IsAligned(FloatingText::alignLeft));}
    /// Determines if the alignment used by the FloatingText object is centered
    /// @return True if the alignment used by the FloatingText object is
    /// centered, false if not
    bool IsAlignedCenter() const                                    
        {return (IsAligned(FloatingText::alignCenter));}
    /// Determines if the alignment used by the FloatingText object is right
    /// aligned
    /// @return True if the alignment used by the FloatingText object is right
    /// aligned, false if not
    bool IsAlignedRight() const                                     
        {return (IsAligned(FloatingText::alignRight));}

// Border Functions
    /// Sets the border
    /// @param set True sets the border, false clears it
    void SetBorder(bool set = true)                                 
        {if (set) SetFlag(border); else ClearFlag(border);}
    /// Determines if a border is drawn around the text
    /// @return True if a border is drawn around the text, false if not
    bool HasBorder() const                                          
        {return (IsFlagSet(FloatingText::border));}
    Rect GetBorderRect() const;
        
// Font Setting Functions
    /// Sets the font setting used by the FloatingText object
    /// @param fontSetting FontSetting object to set
    void SetFontSetting(const FontSetting& fontSetting)             
        {m_fontSetting = fontSetting;}
    /// Gets the font setting used by the FloatingText object
    /// @return The font setting used by the FloatingText object
    FontSetting GetFontSetting() const                              
        {return (m_fontSetting);}
    /// Gets a reference to the FontSetting object used by the FloatingText
    /// object
    /// @return Reference to the FontSetting object used by the FloatingText
    /// object
    FontSetting& GetFontSettingRef()                                
        {return (m_fontSetting);}
    /// Gets a constant reference to the FontSetting object used by the
    /// FloatingText object
    /// @return Constant reference to the FontSetting object used by the
    /// FloatingText object
    const FontSetting& GetFontSettingConstRef() const               
        {return (m_fontSetting);}
};

}

#endif // FLOATINGTEXT_H
