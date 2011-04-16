#include "colour.h"

Colour::Colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	Set(red, green, blue, alpha);
}

void Colour::Set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	red_ = red;
	green_ = green;
	blue_ = blue;
	alpha_ = alpha;
}

bool Colour::operator==(const Colour& rhs) const
{
    return (red_ == rhs.red_ &&
            green_ == rhs.green_ &&
            blue_ == rhs.blue_ &&
            alpha_ == rhs.alpha_
            );
}

uint8_t Colour::Red() const
{
    return red_;
}

uint8_t Colour::Green() const
{
    return green_;
}

uint8_t Colour::Blue() const
{
    return blue_;
}

uint8_t Colour::Alpha() const
{
    return alpha_;
}
