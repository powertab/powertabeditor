/////////////////////////////////////////////////////////////////////////////
// Name:            fontsetting.h
// Purpose:         Stores information about a font format (facename, point size, weight, style, etc.)
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 5, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "fontsetting.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default constants
const char*   FontSetting::DEFAULT_FACENAME           = "Times New Roman";
const int32_t   FontSetting::DEFAULT_POINTSIZE          = 8;
const int32_t   FontSetting::DEFAULT_WEIGHT             = FontSetting::weightNormal;
const bool      FontSetting::DEFAULT_ITALIC             = false;
const bool      FontSetting::DEFAULT_UNDERLINE          = false;
const bool      FontSetting::DEFAULT_STRIKEOUT          = false;
const Colour   FontSetting::DEFAULT_COLOR              = Colour(0,0,0);

// Point Size constants
const int32_t   FontSetting::MIN_POINTSIZE              = 1;
const int32_t   FontSetting::MAX_POINTSIZE              = 72;
    
/// Default Constructor
FontSetting::FontSetting() : m_faceName(DEFAULT_FACENAME),
    m_pointSize(DEFAULT_POINTSIZE), m_weight(DEFAULT_WEIGHT),
    m_italic(DEFAULT_ITALIC), m_underline(DEFAULT_UNDERLINE),
    m_strikeOut(DEFAULT_STRIKEOUT), m_color(DEFAULT_COLOR)
{
    //------Last Checked------//
    // - Dec 6, 2004
}

/// Primary Constructor
/// @param faceName Face name of the font
/// @param pointSize Height of the font, in points
/// @param weight Weight of the font (see .h for values)
/// @param italic Make the font italic style
/// @param underline Make the font underline style
/// @param strikeOut Make the font strikeout style
/// @param color Color to draw the font with
FontSetting::FontSetting(const char* faceName, int32_t pointSize,
    int32_t weight, bool italic, bool underline, bool strikeOut, Colour color) :
    m_faceName(faceName), m_pointSize(pointSize), m_weight(weight),
    m_italic(italic), m_underline(underline), m_strikeOut(strikeOut),
    m_color(color)
{
    //------Last Checked------//
    // - Dec 6, 2004
    assert(faceName != nullptr);
    assert(IsValidPointSize(pointSize));
    assert(IsValidWeight(weight));
}

/// Copy Constructor
FontSetting::FontSetting(const FontSetting& fontSetting) :
    PowerTabObject(),
    m_faceName(DEFAULT_FACENAME), m_pointSize(DEFAULT_POINTSIZE),
    m_weight(DEFAULT_WEIGHT), m_italic(DEFAULT_ITALIC),
    m_underline(DEFAULT_UNDERLINE), m_strikeOut(DEFAULT_STRIKEOUT),
    m_color(DEFAULT_COLOR)
{
    //------Last Checked------//
    // - Dec 6, 2004
    *this = fontSetting;
}

/// Destructor
FontSetting::~FontSetting()
{
    //------Last Checked------//
    // - Dec 6, 2004
}

/// Assignment operator
const FontSetting& FontSetting::operator=(const FontSetting& fontSetting)
{
    //------Last Checked------//
    // - Dec 6, 2004

    // Check for assignment to self
    if (this != &fontSetting)
    {
        m_faceName = fontSetting.m_faceName;
        m_pointSize = fontSetting.m_pointSize;
        m_weight = fontSetting.m_weight;
        m_italic = fontSetting.m_italic;
        m_underline = fontSetting.m_underline;
        m_strikeOut = fontSetting.m_strikeOut;
        m_color = fontSetting.m_color;
    }
    return (*this);
}

/// Equality Operator
bool FontSetting::operator==(const FontSetting& fontSetting) const
{
    //------Last Checked------//
    // - Dec 6, 2004
    return (
        (m_faceName == fontSetting.m_faceName) &&
        (m_pointSize == fontSetting.m_pointSize) &&
        (m_weight == fontSetting.m_weight) &&
        (m_italic == fontSetting.m_italic) &&
        (m_underline == fontSetting.m_underline) &&
        (m_strikeOut == fontSetting.m_strikeOut) &&
        (m_color == fontSetting.m_color)
    );
}

/// Inequality Operator
bool FontSetting::operator!=(const FontSetting& fontSetting) const
{
    //------Last Checked------//
    // - Dec 6, 2004
    return (!operator==(fontSetting));
}

/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool FontSetting::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Dec 5, 2004
    stream.WriteMFCString(m_faceName);
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    stream << m_pointSize << m_weight << m_italic << m_underline << m_strikeOut;
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    stream.WriteWin32ColorRef(m_color);
    PTB_CHECK_THAT(stream.CheckState(), false);
    
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool FontSetting::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream.ReadMFCString(m_faceName);
    
    stream >> m_pointSize >> m_weight >> m_italic >> m_underline >> m_strikeOut;
    
    stream.ReadWin32ColorRef(m_color);
    
    return true;
}

/// Updates the contents of the FontSetting object
/// @param faceName Face name of the font
/// @param pointSize Height of the font, in points
/// @param weight Weight of the font (see .h for values)
/// @param italic Make the font italic style
/// @param underline Make the font underline style
/// @param strikeOut Make the font strikeout style
/// @param color Color to draw the font with
/// @return success or failure
bool FontSetting::SetFontSetting(const char* faceName, int32_t pointSize,
    int32_t weight, bool italic, bool underline, bool strikeOut, Colour color)
{
    //------Last Checked------//
    // - Dec 6, 2004
    if (!SetFaceName(faceName))
        return (false);
    if (!SetPointSize(pointSize))
        return (false);
    if (!SetWeight(weight))
        return (false);
    SetItalic(italic);
    SetUnderline(underline);
    SetStrikeOut(strikeOut);
    SetColor(color);
    
    return (true);
}

}
