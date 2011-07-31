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

#ifndef POWERTABINPUTSTREAM_H
#define POWERTABINPUTSTREAM_H

#include <cstdint>
#include <istream>
#include <vector>
#include <array>

#include <boost/pointee.hpp>

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
    void ReadMFCString(std::string& string);
    void ReadWin32ColorRef(Colour& color);
    void ReadMFCRect(Rect& rect);

private:
    void ReadClassInformation();
    uint32_t ReadMFCStringLength();

public:

    template <class T>
    void ReadVector(std::vector<T>& vect, uint16_t version)
    {
        // Get the type that T points to, regardless of whether T is a raw pointer, shared_ptr, etc
        typedef typename boost::pointee<T>::type PointeeType;

        const uint32_t count = ReadCount();

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
            vect.push_back(temp);
        }
    }

    /// Read data from the input stream
    /// @throw std::ifstream::failure if any errors occur
    template<class T>
    inline PowerTabInputStream& operator>>(T& data)
    {
        m_stream.read(reinterpret_cast<char *>(&data), sizeof(data));
        return *this;
    }

    template <class T>
    inline void ReadSmallVector(std::vector<T>& vect)
    {
        uint8_t size = 0;
        *this >> size;

        vect.clear();
        vect.resize(size);

        m_stream.read((char*)&vect[0], size * sizeof(T));
    }

    template <class T, size_t N>
    inline void ReadSmallVector(std::array<T, N>& array)
    {
        uint8_t size = 0;
        *this >> size;

        m_stream.read((char*)&array[0], size * sizeof(T));
    }
};

#endif // POWERTABINPUTSTREAM_H
