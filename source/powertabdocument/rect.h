#ifndef __RECT_H__
#define __RECT_H__

#include <stdint.h>

class Rect
{
public:
    Rect(void);
    Rect(int32_t x, int32_t y, int32_t width, int32_t height);

    inline bool operator==(const Rect& rhs) const
    {
        return (
            _x == rhs._x &&
            _y == rhs._y &&
            _width == rhs._width &&
            _height == rhs._height);
    }

    inline int32_t GetLeft() const { return GetX(); }
    inline void SetLeft(int32_t x) { SetX(x); }

    inline int32_t GetX() const
    {
        return _x;
    }

    inline void SetX(int32_t x)
    {
        _x = x;
    }

    inline int32_t GetTop() const { return GetY(); }
    inline void SetTop(int32_t top) { SetY(top); }

    inline int32_t GetY() const
    {
        return _y;
    }

    inline void SetY(int32_t y)
    {
        _y = y;
    }

    inline int32_t GetBottom() const
    {
        return _y + _height - 1;
    }

    inline void SetBottom(int32_t bottom)
    {
        _height = bottom - _y - 1;
    }

    inline int32_t GetWidth() const
    {
        return _width;
    }

    inline void SetWidth(int32_t width)
    {
        _width = width;
    }

    inline int32_t GetHeight() const
    {
        return _height;
    }

    inline void SetHeight(int32_t height)
    {
        _height = height;
    }

    inline int32_t GetRight() const
    {
        return _x + _width - 1;
    }

    inline void SetRight(int32_t right)
    {
        _width = right - _x - 1;
    }

protected:
    int32_t _x, _y, _width, _height;
};

#endif
