#pragma once

#include <string>
#include <unordered_map>
#include "Ninja.hpp"

namespace ThemeGen {

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
    std::string biome;
    std::vector<std::string> tags;
};

struct ThemeMatch {
    std::vector<int> pattern;
    std::vector<std::vector<int>> notPatterns;
    int offset;
    std::vector<int> notOffsets;
    std::vector<std::string> commands;
    bool _else = false;
};

enum class OMType: int {
    Corridor = 0,
    Floor = 1,
    Ceiling = 2
};

struct RepeatingPattern {
    int id;
    std::string data;
    int start = 195;
    int repeat = 300;
};

enum class InOverride : int {
    None = 0,
    Base = 1,
    EndDown = 2,
    EndUp = 3,
    Slope = 4,
    Fuzz = 5,
    Spike = 6,
    Portal = 7,
    Speed = 8
};

std::string nuke155(std::string addBlockLine);
std::string parseAddBlock(std::string addBlockLine, float X = 465.f, float Y = 195.f,
    int maxHeight = 195, int minHeight = 45, int corridorHeight = 60);
std::string parseTheme(const std::string& name, const JFPGen::LevelData& leveldata);

std::array<int, 3> hexToColor(const std::string& hex);

}