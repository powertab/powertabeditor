#include "colour.h"


Colour::Colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	Set(red, green, blue, alpha);
}

void Colour::Set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	_red = red;
	_green = green;
	_blue = blue;
	_alpha = alpha;
}


Colour::~Colour(void)
{
}
