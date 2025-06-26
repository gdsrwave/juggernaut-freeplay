#pragma once

#include <string>
#include <unordered_map>
#include "Ninja.hpp"

namespace ThemeGen {

// Represents a color in RGBA format
struct Color {
    float r, g, b, a;
    Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
};

// Represents a theme with a name and a set of colors
struct Theme {
    std::string name;
    std::unordered_map<std::string, Color> colors;
};

// Generates a theme based on a seed or preset and level data
std::string parseTheme(const std::string& name, const JFPGen::LevelData& leveldata);

// Utility to convert hex color string to Color
Color hexToColor(const std::string& hex);

} // namespace ThemeGen