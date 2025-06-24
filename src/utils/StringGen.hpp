#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include <random>
#include <cmath>
#include <fmt/core.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <filesystem>
#include <map>
#include <vector>
#include <string>
#include <array>
#include <utility>

using namespace geode::prelude;

namespace JFPGen {

extern std::map<std::string, int> portalOddsMap;
extern std::map<std::string, int> speedOddsMap;

struct Segment {
    std::pair<int, int> coords;
    int y_swing;
};

struct Biome {
    int x_initial;
    std::string type;
    std::string theme;
    std::string options;
    std::vector<Segment> segments;
};

struct LevelData {
    std::string name;
    std::vector<Biome> biomes;
};

bool orientationMatch(int prevO[11], const std::vector<int> pattern);
void orientationShift(int prevO[11], int newO);
void exportLvlStringGMD(std::filesystem::path const& path, std::string ld1);
int convertSpeed(const std::string& speed);
float convertSpeedToFloat(const std::string& speed);
int convertFloatSpeed(float speed);

enum class AutoJFP : int {
    NotInAutoJFP = 0,
    JustStarted = 1,
    JustRestarted = 2,
    PlayingLevelAtt1 = 3,
    PlayingLevel = 4,
};

extern AutoJFP state;

std::string jfpMainStringGen(bool compress, AutoJFP state = AutoJFP::NotInAutoJFP);

} // namespace JFPGen
