#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>

class Vec2{
    private:
        float x, y;

    public:
        Vec2() : x(0.0f), y(0.0f) {}
        Vec2(float x, float y) : x(x), y(y) {}

        Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
        Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
        Vec2 operator*(const float scalar) const { return Vec2(x * scalar, y * scalar); }
        Vec2 operator/(const float scalar) const { return Vec2(x / scalar, y / scalar); }

        Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
        Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
        Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
        Vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

        float getX() const { return x; }
        float getY() const { return y; }
        void setX(float val) { x = val; }
        void setY(float val) { y = val; }

        float length() const { return std::sqrt(x * x + y * y); }

        void clear() { x = 0.0f; y = 0.0f; }
        static Vec2 zero() { return Vec2(0.0f, 0.0f); }
};

#endif