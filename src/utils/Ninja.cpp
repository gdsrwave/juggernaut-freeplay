#include <Geode/Geode.hpp>
#include "Ninja.hpp"

using namespace geode::prelude;

namespace JFPGen {

LevelData generateJFPLevel() {
    const int64_t length = Mod::get()->getSettingValue<int64_t>("length");

    std::vector<Segment> segments(length);
    
    LevelData levelData = {
        .name = "JFP Level",
        .biomes = {
            {
                .x_initial = 435,
                .type = "Default",
                .theme = "Classic",
                .options = {
                    .length = static_cast<int>(length),
                    .corridorHeight = static_cast<int>(Mod::get()->getSettingValue<double>("corridor-height")),
                    .maxHeight = 195,
                    .visibility = Mod::get()->getSettingValue<bool>("low-vis") ? Visibility::Low : Visibility::Standard,
                    .startingSpeed = SpeedChange::Speed3x,
                    .colorMode = []{
                        auto cm = Mod::get()->getSettingValue<std::string>("color-mode");
                        if (cm == "Classic Mode") return ColorMode::ClassicMode;
                        if (cm == "All Colors") return ColorMode::AllColors;
                        if (cm == "Night Mode") return ColorMode::NightMode;
                        return ColorMode::Washed;
                    }()
                },
                .segments = segments
            }
        }
    };
};

}