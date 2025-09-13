// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "./Ninja.hpp"

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

enum class OMType: int {
    Corridor = 0,
    Floor = 1,
    Ceiling = 2,
    None = 3
};

struct ThemeMatch {
    OMType omType;
    std::vector<OMType> notTypes;
    std::vector<int> pattern;
    std::vector<std::vector<int>> notPatterns;
    int offset;
    std::vector<int> notOffsets;
    std::vector<std::string> commands;
    bool _else = false;
};

struct RepeatingPattern {
    int id;
    std::vector<std::string> data;
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
    Speed = 8,
    Start = 9,
    CorridorBlock = 10
};

std::array<int, 3> hexToColor(const std::string& hex);
bool strictOM(const std::vector<JFPGen::Segment>& segments, int idx,
    const std::vector<int>& pattern, OMType omType = OMType::Corridor,
    bool typeA = false);
std::string handleRawBlock(std::string addBlockLine, OMType omType = OMType::None);
std::string parseAddBlock(std::string addBlockLine, float X = 465.f, float Y = 195.f,
    int maxHeight = 195, int minHeight = 45, int corridorHeight = 60);
std::string parseTheme(const std::string& name, const JFPGen::LevelData& leveldata);
ThemeMetadata parseThemeMeta(const std::string& name);
std::vector<std::string> tagConflicts(ThemeMetadata tmd);

}  // namespace ThemeGen
