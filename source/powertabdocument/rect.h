#ifndef RECT_H
#define RECT_H

#include <cstdint>

class Rect
{
public:
    Rect(void);
    Rect(int32_t x, int32_t y, int32_t width, int32_t height);

    bool operator==(const Rect& rhs) const;

    int32_t GetLeft() const;
    void SetLeft(int32_t x);

    int32_t GetX() const;
    void SetX(int32_t x);

    int32_t GetTop() const;
    void SetTop(int32_t top);

    int32_t GetY() const;
    void SetY(int32_t y);

    int32_t GetBottom() const;
    void SetBottom(int32_t bottom);

    int32_t GetWidth() const;
    void SetWidth(int32_t width);

    int32_t GetHeight() const;
    void SetHeight(int32_t height);

    int32_t GetRight() const;
    void SetRight(int32_t right);

private:
    int32_t x_, y_, width_, height_;
};

#endif // RECT_H
