#pragma once
#include "imgui.h"
struct Vec2 {
    float            x, y;
    constexpr float& width() { return x; }
    constexpr float& height() { return y; }
    Vec2() : x(0), y(0) {}
    Vec2(auto _x, auto _y) : x(_x), y(_y) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}
    Vec2(ImVec2& rhs) : x(rhs.x), y(rhs.y) {}
    // implicit cast to ImVec2
    operator ImVec2() { return ImVec2{x, y}; }

    friend Vec2 operator*(const Vec2& lhs, const Vec2& rhs) {
        return Vec2{lhs.x * rhs.x, lhs.y * rhs.y};
    }
    Vec2 operator*=(Vec2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        return Vec2{x * rhs.x, y * rhs.y};
    }
};
struct Vec3 {
    float            x, y, z;
    constexpr float& r() { return x; }
    constexpr float& g() { return y; }
    constexpr float& b() { return z; }

    // loses 4th param, explicit
    explicit Vec3(ImVec4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
    operator ImVec4() { return ImVec4{x, y, z, 0.0f}; }
};
struct Vec4 {
    float            r, g, b, a;
    constexpr float& x() { return r; }
    constexpr float& y() { return g; }
    constexpr float& z() { return b; }
    constexpr float& w() { return a; }

    Vec4(int x, int y, int z, int w) : r(x), g(y), b(z), a(w) {}
    Vec4(ImVec4& rhs) : r(rhs.x), g(rhs.y), b(rhs.z), a(rhs.w) {}
    operator ImVec4() { return ImVec4{r, g, b, a}; }
};
