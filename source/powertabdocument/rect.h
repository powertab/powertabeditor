#ifndef __RECT_H__
#define __RECT_H__

class Rect
{
public:
	Rect(void);
	Rect(int x, int y, int width, int height);
	~Rect(void);

	bool operator==(const Rect& rhs) const 
	{
		return (
			_x == rhs._x &&
			_y == rhs._y &&
			_width == rhs._width &&
			_height == rhs._height);
	}

	int GetLeft() const { return GetX(); }
	void SetLeft(int x) { SetX(x); }

	int GetX() const
	{
		return _x;
	}

	void SetX(int x)
	{
		_x = x;
	}

	int GetTop() const { return GetY(); }
	void SetTop(int top) { SetY(top); }

	int GetY() const
	{
		return _y;
	}

	void SetY(int y)
	{
		_y = y;
	}

	int GetBottom() const
	{
		return _y + _height - 1;
	}

	void SetBottom(int bottom)
	{
		_height = bottom - _y - 1;
	}

	int GetWidth() const
	{
		return _width;
	}

	void SetWidth(int width)
	{
		_width = width;
	}

	int GetHeight() const
	{
		return _height;
	}

	void SetHeight(int height)
	{
		_height = height;
	}

	int GetRight() const
	{
		return _x + _width - 1;
	}

	void SetRight(int right)
	{
		_width = right - _x - 1;
	}


private:
	int _x, _y, _width, _height;
};

#endif