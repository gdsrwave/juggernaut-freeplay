#pragma once

#include <string>
#include <unordered_map>
#include "Ninja.hpp"

namespace ThemeGen {


// Represents a theme with a name and a set of colors
struct Theme {
    std::string name;
};

struct ThemeMetadata {
    std::string name;
    std::string author;
    std::string version;
    std::string pack;
    std::string type;
    std::string description;
    std::string date;
};

struct ThemeMatch {
    std::vector<int> pattern; // TODO: make this a list of list of ints
    std::vector<std::vector<int>> notPatterns;
    int offset;
    std::vector<int> notOffsets;
    std::vector<std::string> commands;
};

// Generates a theme based on a seed or preset and level data
std::string parseAddBlock(std::string addBlockLine, float X = 465.f, float Y = 195.f,
    int maxHeight = 195, int minHeight = 45);
std::string parseTheme(const std::string& name, const JFPGen::LevelData& leveldata);

// Utility to convert hex color string to Color
std::array<int, 3> hexToColor(const std::string& hex);

} // namespace ThemeGen