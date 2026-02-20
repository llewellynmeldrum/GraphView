#pragma once
#include "imgui.h"
#include <array>
// clang-format off
struct Vec2 {
    float x, y;

    // convinience accesors
    constexpr float&        width()     { return x; }
    constexpr float&        height()    { return y; }
              float*        data()      { updateArray(); return arr.data(); }


    Vec2()                      : x(0), y(0)            { updateArray(); }
    Vec2(auto _x, auto _y)      : x(_x), y(_y)          { updateArray(); }
    Vec2(float _x, float _y)    : x(_x), y(_y)          { updateArray(); }
    Vec2(ImVec2& rhs)           : x(rhs.x), y(rhs.y)    { updateArray(); }

    // implicit casts from ImVecs
    operator ImVec2() { return ImVec2{x, y}; }

    // WARNING: this is NOT a dot product, this is element wise mult
    friend Vec2 operator*(const Vec2& lhs, const Vec2& rhs) {
        return Vec2{lhs.x * rhs.x, lhs.y * rhs.y};
    }
    Vec2 operator*=(Vec2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        return Vec2{x * rhs.x, y * rhs.y};
    }

 private:
    std::array<float, 2> arr;
    inline void updateArray() { arr = {x, y}; }
};

struct Vec3 {
    float x, y, z;
    constexpr float&        r()     { return x; }
    constexpr float&        g()     { return y; }
    constexpr float&        b()     { return z; }
    float*                  data()  { updateArray(); return arr.data(); }
    

    Vec3(auto _x, auto _y, auto _z) : x(_x), y(_y), z(_z) { updateArray(); }

    // explicit cast because we are dropping off 1 float
    explicit Vec3(ImVec4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
    explicit operator ImVec4() { return ImVec4{x, y, z, 0.0f}; }

 private:
    std::array<float, 3> arr;
    inline void updateArray() { arr = {x, y, z}; }
};

struct Vec4 {
    float                   r, g, b, a;
    constexpr float&        x() { return r; }
    constexpr float&        y() { return g; }
    constexpr float&        z() { return b; }
    constexpr float&        w() { return a; }
    float*                  data(){updateArray(); return arr.data(); }

    Vec4(auto x, auto y, auto z, auto w) : r(x), g(y), b(z), a(w) {updateArray();}
    Vec4(ImVec4& rhs) : r(rhs.x), g(rhs.y), b(rhs.z), a(rhs.w) {}

    operator ImVec4() { return ImVec4{r, g, b, a}; }

 private:
    std::array<float, 4> arr;
    inline void updateArray() { arr = {r, g, b, a}; }
};
