/////////////////////////////////////////////////////////////////////////////
// Name:            powertabinputstream.h
// Purpose:         Input stream used to deserialize MFC based Power Tab data
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 19, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __POWERTABINPUTSTREAM_H__
#define __POWERTABINPUTSTREAM_H__

#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>

#include "macros.h"

class Rect;
class Colour;

/// Input stream used to deserialize MFC based Power Tab data
class PowerTabInputStream
{
    friend class PowerTabFileHeader;

    // Member Variables
protected:
    std::ifstream    m_stream;

    // Constructor/Destructor
public:
    PowerTabInputStream(const std::string& filename);
    ~PowerTabInputStream();

    // Read Functions
    uint32_t ReadCount();
    bool ReadMFCString(std::string& string);
    bool ReadWin32ColorRef(Colour& color);
    bool ReadMFCRect(Rect& rect);

protected:
    void ReadClassInformation();
    uint32_t ReadMFCStringLength();

    // Error Checking Functions
public:
    /// Checks the current state of the stream
    /// @return True if the stream is OK, false if an error has occurred
    inline bool CheckState()
    {
        return !m_stream.fail();
    }

    inline bool fail()
    {
        return m_stream.fail();
    }

    template <class T>
    bool ReadVector(std::vector<T*>& vect, uint16_t version)
    {
        const uint32_t count = ReadCount();
        CHECK_THAT(CheckState(), false);

        vect.clear();
        if (count > 0)
        {
            vect.reserve(count);
        }

        for (uint32_t i = 0; i < count; i++)
        {
            ReadClassInformation();
            T* temp = new T();
            temp->Deserialize(*this, version);
            CHECK_THAT(CheckState(), false);
            vect.push_back(temp);
        }
        return true;
    }

    template<class T>
    inline PowerTabInputStream& operator>>(T& data)
    {
        m_stream.read(reinterpret_cast<char *>(&data), sizeof(data));
        return *this;
    }
};

#endif
