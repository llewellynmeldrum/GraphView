#pragma once
#include "imgui.h"
#define COLOR(c) c.r, c.g, c.b, c.a
struct GLColor {
    float r, g, b, a;
    GLColor(auto _r, auto _b, auto _g) : r(_r), g(_g), b(_b), a(1.0f) {}
    GLColor(auto _r, auto _b, auto _g, auto _a) : r(_r), g(_g), b(_b), a(_a) {}
    GLColor(float grey) : r(grey), g(grey), b(grey), a(1.0f) {}
    GLColor(float grey, float alpha) : r(grey), g(grey), b(grey), a(alpha) {}
    operator ImVec4() const { return ImVec4{r, g, b, a}; }
};

static inline const GLColor WHITE = {1.0f};
static inline const GLColor BLACK = {0.0f};
static inline const GLColor GREY = {0.5f};
static inline const GLColor DARKGREY = {0.25f};
static inline const GLColor DDARKGREY = {0.1f};
static inline const GLColor LIGREY = {0.75f};
static inline const GLColor LILIGREY = {0.85f};
static inline const GLColor RED = {1, 0, 0};
static inline const GLColor GREEN = {0, 1, 0};
static inline const GLColor BLUE = {0, 0, 1};
