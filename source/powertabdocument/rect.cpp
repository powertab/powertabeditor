#include "rect.h"


Rect::Rect(void)
{
	_x = _y = _width = _height = 0;
}

Rect::Rect(int x = 0, int y = 0, int width = 0, int height = 0)
{
	_x = x;
	_y = y;
	_width = width;
	_height = height;
}


Rect::~Rect(void)
{
}
