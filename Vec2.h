#pragma once
#include <cmath>

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    float Length()   const { return std::sqrt(x * x + y * y); }
    float LengthSq() const { return x * x + y * y; }

    Vec2 Normalized() const {
        float len = Length();
        return len > 0.0f ? Vec2(x / len, y / len) : Vec2(0.0f, 0.0f);
    }

    static float Dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
    static float Cross(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }
    static float Distance(Vec2 a, Vec2 b) { return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2)); }
    static float DistanceSq(Vec2 a, Vec2 b) { return pow((a.x - b.x), 2) + pow((a.y-b.y), 2); }

    Vec2 operator+(Vec2 b)  const { return { x + b.x, y + b.y }; }
    Vec2 operator-(Vec2 b)  const { return { x - b.x, y - b.y }; }
    Vec2 operator*(float b) const { return { x * b, y * b }; }
    Vec2 operator/(float b) const { return { x / b, y / b }; }

    Vec2& operator+=(Vec2 b) { x += b.x; y += b.y; return *this; }
    Vec2& operator-=(Vec2 b) { x -= b.x; y -= b.y; return *this; }
    Vec2& operator*=(float b) { x *= b;   y *= b;   return *this; }
};