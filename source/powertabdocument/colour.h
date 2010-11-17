#ifndef __COLOUR_H__
#define __COLOUR_H__

#include <stdint.h>

class Colour
{
public:
	Colour(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);
	~Colour(void);

	bool operator==(const Colour& rhs) const
	{
		return (
			_red == rhs._red &&
			_green == rhs._green &&
			_blue == rhs._blue &&
			_alpha == rhs._alpha
			);
	}

	uint8_t Red() { return _red; }
	uint8_t Green() { return _green; }
	uint8_t Blue() { return _blue; }
	uint8_t Alpha() { return _alpha; }

	void Set(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);

private:
	uint8_t _red, _green, _blue, _alpha;
};

#endif