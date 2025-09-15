// Copyright 2025 GDSRWave
#pragma once
#include <map>
#include <string>
#include <vector>
#include <utility>
#include "./shared.hpp"

extern JFPGen::AutoJFP state;

extern bool att1;

extern int globalAtt;

namespace JFPGen {

enum class SpeedChange : int {
    None = -1,
    Random = 0,
    Speed05x = 1,
    Speed1x = 2,
    Speed2x = 3,
    Speed3x = 4,
    Speed4x = 5
};

enum class Visibility : int {
    Standard = 0,
    Low = 1,
    Hidden = 2
};

enum class ColorMode : int {
    Random = 0,
    Washed = 1,
    AllColors = 2,
    ClassicMode = 3,
    NightMode = 4
    // Random = 4
};

enum class CorridorRules : int {
    Random = 0,
    NoSpamNoZigzag = 1,
    NoSpam = 2,
    Juggernaut = 3,
    Unrestricted = 4,
    LRD = 5,
    Limp = 6
};

struct RawCR {
    bool NZ = false;
    bool NS = false;
    bool LRD = false;
    bool FD = false;
    bool SPECIAL = false;
    bool BURST = false;
    bool ND = false;
};

enum class Difficulties : int {
    None = 0,
    Light = 1,
    Balanced = 2,
    Aggressive = 3
};

enum class Portals : int {
    None = 0,
    Gravity = 1,
    Fake = 2,
    Teleportal = 3
};

enum class PortalInputs : int {
    Both = 0,
    Releases = 1,
    Holds = 2
};

enum class MusicSources : int {
    JFPSoundtrack = 0,
    LocalFiles = 1
};

enum class PlacementBySize : int {
    Both = 0,
    Big = 1,
    Mini = 2
};

enum class WaveSize : int {
    Big = 0,
    Mini = 1,
    Random = 2
};

enum class JFPBiome : int {
    Juggernaut = 0,
    DeathFactory = 1,
    SawField = 2,
    SpikeRain = 3,
    NeironExGaming = 4,
    Campfire = 5,
    DeathCWaveBuff = 6,
    RandomPart = 7
};

struct SegmentOptions {
    bool gravity = false;
    bool isSpikeM = false;
    bool spikeSide = false;
    Portals isPortal = Portals::None;
    SpeedChange speedChange = SpeedChange::None;
    bool isFuzzy = false;
    bool mini = false;
    bool isTransition = false;
};

struct Segment {
    std::pair<int, int> coords;
    int y_swing;
    SegmentOptions options;
};

struct BiomeOptions {
    int length;
    int corridorHeight;
    bool startingMini;
    int maxHeight;
    int minHeight;
    Visibility visibility = Visibility::Standard;
    bool startingGravity = false;
    SpeedChange startingSpeed;
    ColorMode colorMode = ColorMode::Washed;
    int bgColor[3] = {0, 0, 0};
    int lineColor[3] = {255, 255, 255};
    bool typeA = true;
};

struct Biome {
    int x_initial;
    int y_initial;
    JFPBiome type;
    std::string theme;
    int song;
    BiomeOptions options;
    std::vector<Segment> segments;
};

struct LevelData {
    std::string name;
    uint32_t seed;
    std::vector<Biome> biomes;
};

struct Color {
    int slot = 13;
    std::array<int, 3> rgb = {255, 255, 255};
    bool blending = false;
    float opacity = 1.f;
    int copyColor = -1;
    std::string special = "";
};

extern std::map<Difficulties, int> portalOddsMap;
extern std::map<Difficulties, int> speedOddsMap;

int convertFloatSpeed(float speed);
SpeedChange convertFloatSpeedEnum(float speed);
float convertSpeedToFloat(const std::string& speed);
float convertSpeedToFloat(SpeedChange speed);
bool orientationMatch(int prevO[11], const std::vector<int> pattern);
bool orientationMatch(const std::vector<Segment>& segments, int idx,
    const std::vector<int>& pattern, bool strictMini = false);

LevelData generateJFPLevel();

}  // namespace JFPGen

extern std::map<std::string, std::string> kBank;
extern std::map<std::string, bool> overrideBank;
extern std::vector<JFPGen::Color> colorBank;
extern const std::vector<JFPGen::Color> defaultColorBank;

void pushColor(const JFPGen::Color& color);
