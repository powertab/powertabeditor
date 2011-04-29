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

#include <cstdint>
#include <istream>
#include <vector>
#include <array>

#include <boost/pointee.hpp>

#include "macros.h"

class Rect;
class Colour;

/// Input stream used to deserialize MFC based Power Tab data
class PowerTabInputStream
{
    // Member Variables
private:
    std::istream& m_stream;

public:
    PowerTabInputStream(std::istream& stream);

    // Read Functions
    uint32_t ReadCount();
    bool ReadMFCString(std::string& string);
    bool ReadWin32ColorRef(Colour& color);
    bool ReadMFCRect(Rect& rect);

private:
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
    bool ReadVector(std::vector<T>& vect, uint16_t version)
    {
        // Get the type that T points to, regardless of whether T is a raw pointer, shared_ptr, etc
        typedef typename boost::pointee<T>::type PointeeType;

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
            T temp(new PointeeType);
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

    template <class T>
    bool ReadSmallVector(std::vector<T>& vect)
    {
        uint8_t size = 0;
        *this >> size;

        CHECK_THAT(CheckState(), false);

        vect.clear();
        vect.resize(size);

        m_stream.read((char*)&vect[0], size * sizeof(T));

        return CheckState();
    }

    template <class T, size_t N>
    inline bool ReadSmallVector(std::array<T, N>& array)
    {
        uint8_t size = 0;
        *this >> size;

        CHECK_THAT(CheckState(), false);

        m_stream.read((char*)&array[0], size * sizeof(T));
        return CheckState();
    }
};

#endif
