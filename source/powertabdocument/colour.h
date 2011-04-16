#ifndef __COLOUR_H__
#define __COLOUR_H__

#include <cstdint>

class Colour
{
public:
	Colour(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);

	bool operator==(const Colour& rhs) const;

    uint8_t Red() const;
    uint8_t Green() const;
    uint8_t Blue() const;
    uint8_t Alpha() const;
    
	void Set(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);

private:
	uint8_t red_, green_, blue_, alpha_;
};

#endif
