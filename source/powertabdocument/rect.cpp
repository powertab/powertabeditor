#include "rect.h"


Rect::Rect(void)
{
    _x = _y = _width = _height = 0;
}

Rect::Rect(int32_t x = 0, int32_t y = 0, int32_t width = 0, int32_t height = 0)
{
    _x = x;
    _y = y;
    _width = width;
    _height = height;
}
