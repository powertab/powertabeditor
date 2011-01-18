#ifndef __RECT_H__
#define __RECT_H__

class Rect
{
public:
    Rect(void);
    Rect(int x, int y, int width, int height);

    inline bool operator==(const Rect& rhs) const
    {
        return (
            _x == rhs._x &&
            _y == rhs._y &&
            _width == rhs._width &&
            _height == rhs._height);
    }

    inline int GetLeft() const { return GetX(); }
    inline void SetLeft(int x) { SetX(x); }

    inline int GetX() const
    {
        return _x;
    }

    inline void SetX(int x)
    {
        _x = x;
    }

    inline int GetTop() const { return GetY(); }
    inline void SetTop(int top) { SetY(top); }

    inline int GetY() const
    {
        return _y;
    }

    inline void SetY(int y)
    {
        _y = y;
    }

    inline int GetBottom() const
    {
        return _y + _height - 1;
    }

    inline void SetBottom(int bottom)
    {
        _height = bottom - _y - 1;
    }

    inline int GetWidth() const
    {
        return _width;
    }

    inline void SetWidth(int width)
    {
        _width = width;
    }

    inline int GetHeight() const
    {
        return _height;
    }

    inline void SetHeight(int height)
    {
        _height = height;
    }

    inline int GetRight() const
    {
        return _x + _width - 1;
    }

    inline void SetRight(int right)
    {
        _width = right - _x - 1;
    }

protected:
    int _x, _y, _width, _height;
};

#endif
