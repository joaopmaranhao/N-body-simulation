#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>

//might add something here

class Vec3{
    private:
        float x, y, z; //3d cartesian coordinates

    public:
        //Initialization methods
        Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        //Operation overload 
        Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(const float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
        Vec3 operator/(const float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

        Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
        Vec3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

        //getters and setters 
        float getX() const { return x; }
        float getY() const { return y; }
        float getZ() const { return z; }
        void setX(float val) { x = val; }
        void setY(float val) { y = val; }
        void setZ(float val) { z = val; }
        float length() const { return std::sqrt(x * x + y * y + z * z); }

        //clearing
        void clear() { x = 0.0f; y = 0.0f; z = 0.0f; }
        static Vec3 zero() { return Vec3(0.0f, 0.0f, 0.0f); }
};

#endif