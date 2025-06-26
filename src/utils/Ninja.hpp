#pragma once
#include <utility>
#include <string>
#include <vector>

namespace JFPGen {

enum class SpeedChange : int {
    None = 0,
    Speed1x = 1,
    Speed2x = 2,
    Speed3x = 3,
    Speed4x = 4,
    Speed05x = 5
};

enum class Visibility : int {
    Standard = 0,
    Low = 1,
    Hidden = 2
};

enum class ColorMode : int {
    Washed = 0,
    AllColors = 1,
    ClassicMode = 2,
    NightMode = 3
};

struct SegmentOptions {
    bool gravity = false;
    bool isSpikeM = false;
    bool cornerPieces = false;
    std::string isPortal = "None";
    SpeedChange speedChange = SpeedChange::None;
    bool isFakePortal = false;
    bool isFuzzy = false;
};

struct Segment {
    std::pair<int, int> coords;
    int y_swing;
    SegmentOptions options;
};

struct BiomeOptions {
    int length;
    int corridorHeight = 60;
    int maxHeight;
    Visibility visibility = Visibility::Standard;
    SpeedChange startingSpeed;
    ColorMode colorMode = ColorMode::Washed;
};

struct Biome {
    int x_initial;
    std::string type;
    std::string theme;
    BiomeOptions options;
    std::vector<Segment> segments;
};

struct LevelData {
    std::string name;
    std::vector<Biome> biomes;
};

LevelData generateJFPLevel();

}
