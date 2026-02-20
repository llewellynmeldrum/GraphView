#pragma once
#include "imgui.h"
#include <array>
// clang-format off
struct Vec2 {
    float x, y;

    // convinience accesors
    constexpr float&        width()     { return x; }
    constexpr float&        height()    { return y; }
              float*        data()      { return &x; }


    Vec2()                      : x(0), y(0)            {}
    Vec2(auto _x, auto _y)      : x(_x), y(_y)          {}
    Vec2(float _x, float _y)    : x(_x), y(_y)          {}
    Vec2(ImVec2& rhs)           : x(rhs.x), y(rhs.y)    {}

    // implicit casts from ImVecs
    operator ImVec2() { return ImVec2{x, y}; }

    // WARNING: this is NOT a dot product, this is element wise mult
    friend Vec2 operator*(const Vec2& lhs, const Vec2& rhs) {
        return Vec2{lhs.x * rhs.x, lhs.y * rhs.y};
    }
    friend Vec2 operator*(const Vec2& lhs, const float& scalar) {
        return Vec2{lhs.x * scalar, lhs.y * scalar};
    }
    friend Vec2& operator*=(Vec2& lhs, const float scalar) {
        lhs.x*=scalar;
        lhs.y*=scalar;
        return lhs;
    }
    friend Vec2 operator+(const Vec2& lhs, const float& scalar) {
        return Vec2{lhs.x + scalar, lhs.y + scalar};
    }
    friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
        return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
    }
    friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
        return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
    }
    Vec2 operator*=(Vec2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        return Vec2{x * rhs.x, y * rhs.y};
    }
    Vec2 operator*=(float& scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

};

struct Vec3 {
    float x, y, z;
    constexpr float&        r()     { return x; }
    constexpr float&        g()     { return y; }
    constexpr float&        b()     { return z; }
    float*                  data()  { return &x; }
    

    Vec3(auto _x, auto _y, auto _z) : x(_x), y(_y), z(_z) { }

    // explicit cast because we are dropping off 1 float
    explicit Vec3(ImVec4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
    explicit operator ImVec4() { return ImVec4{x, y, z, 0.0f}; }
};

struct Vec4 {
    float                   r, g, b, a;
    constexpr float&        x() { return r; }
    constexpr float&        y() { return g; }
    constexpr float&        z() { return b; }
    constexpr float&        w() { return a; }
    float*                  data(){return &r;}

    Vec4(Vec2 v2, auto z, auto w) : r(v2.x), g(v2.y), b(z), a(w) {}
    Vec4(auto x, auto y, auto z, auto w) : r(x), g(y), b(z), a(w) {}
    Vec4(ImVec4& rhs) : r(rhs.x), g(rhs.y), b(rhs.z), a(rhs.w) {}

    operator ImVec4() { return ImVec4{r, g, b, a}; }
};
