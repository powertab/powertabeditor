/////////////////////////////////////////////////////////////////////////////
// Name:            FloatingText.cpp
// Purpose:         Stores and renders text annotations
// Author:          Brad Larsen
// Modified by:     
// Created:         Sat Nov 27 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// Licence:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "floatingtext.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default constants
const char*   FloatingText::DEFAULT_TEXT          = "Text";
const Rect    FloatingText::DEFAULT_RECT          = Rect(0,0,0,0);
const uint8_t    FloatingText::DEFAULT_FLAGS         = DEFAULT_ALIGNMENT;
const uint8_t    FloatingText::DEFAULT_ALIGNMENT     = FloatingText::alignLeft;

// Constructor/Destructor
/// Default Constructor
FloatingText::FloatingText() : 
    m_text(DEFAULT_TEXT), m_rect(DEFAULT_RECT), m_flags(DEFAULT_FLAGS)
{
    //------Last Checked------//
    // - Dec 7, 2004
}

/// Primary Constructor
/// @param text Text to set
/// @param rect Bounding rect for the text, in logical co-ordinates
/// @param flags Flags to set (see flags enum)
/// @param fontSetting FontSetting object to set
FloatingText::FloatingText(const char* text, Rect rect, uint8_t flags,
    const FontSetting& fontSetting) : m_text(text), m_rect(rect),
    m_flags(flags), m_fontSetting(fontSetting)
{
    //------Last Checked------//
    // - Dec 7, 2004
    assert(text != nullptr);
}

/// Copy Constructor
FloatingText::FloatingText(const FloatingText& floatingText) :
    PowerTabObject(),
    m_text(DEFAULT_TEXT), m_rect(DEFAULT_RECT), m_flags(DEFAULT_FLAGS)
{
    //------Last Checked------//
    // - Dec 7, 2004
    *this = floatingText;
}

/// Destructor    
FloatingText::~FloatingText()
{
    //------Last Checked------//
    // - Dec 7, 2004
}

// Operators
/// Assignment Operator
const FloatingText& FloatingText::operator=(const FloatingText& floatingText)
{
    //------Last Checked------//
    // - Dec 7, 2004
    
    // Check for assignment to self
    if (this != &floatingText)
    {
        m_text = floatingText.m_text;
        m_rect = floatingText.m_rect;
        m_flags = floatingText.m_flags;
        m_fontSetting = floatingText.m_fontSetting;
    }
    return (*this);
}

/// Equality Operator
bool FloatingText::operator==(const FloatingText& floatingText) const
{
    //------Last Checked------//
    // - Dec 6, 2004
    return (
        (m_text == floatingText.m_text) &&
        (m_rect == floatingText.m_rect) &&
        (m_flags == floatingText.m_flags) &&
        (m_fontSetting == floatingText.m_fontSetting)
    );
}

/// Inequality Operator
bool FloatingText::operator!=(const FloatingText& floatingText) const
{
    //------Last Checked------//
    // - Dec 6, 2004
    return (!operator==(floatingText));
}

/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool FloatingText::Serialize(PowerTabOutputStream & stream) const
{
    //------Last Checked------//
    // - Dec 7, 2004
    stream.WriteMFCString(m_text);
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    stream.WriteMFCRect(m_rect);
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    stream << m_flags;
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    m_fontSetting.Serialize(stream);
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool FloatingText::Deserialize(PowerTabInputStream & stream, uint16_t version)
{
    stream.ReadMFCString(m_text);
    
    stream.ReadMFCRect(m_rect);
    
    stream >> m_flags;
    
    m_fontSetting.Deserialize(stream, version);
    
    return true;
}

// Flag Functions
/// Sets a flag used by the FloatingText object
/// @param flag The flag to set
void FloatingText::SetFlag(uint8_t flag)
{
    //------Last Checked------//
    // - Dec 7, 2004
    
    // Clear old alignment flag if new flag is alignment flag
    if (((flag & alignLeft) == alignLeft) |
        ((flag & alignCenter) == alignCenter) |
        ((flag & alignRight) == alignRight))
    {
        ClearFlag(alignMask);
    }
    m_flags |= flag;
}

/// Gets the rect used to draw the border surrounding the text
/// @return The rect used to draw the border surrounding the text
Rect FloatingText::GetBorderRect() const
{
    //------Last Checked------//
    // - Dec 7, 2004
    Rect returnValue;
    returnValue.SetLeft(m_rect.GetLeft() - 2);
    returnValue.SetTop(m_rect.GetTop() - 1);
    returnValue.SetRight(m_rect.GetRight() + 1);
    returnValue.SetBottom(m_rect.GetBottom() + 1);
    return (returnValue);
}

}
