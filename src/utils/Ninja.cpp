#include <Geode/Geode.hpp>
#include "Ninja.hpp"
#include <random>
#include "constants.hpp"

using namespace geode::prelude;
JFPGen::AutoJFP state = JFPGen::AutoJFP::NotInAutoJFP;

namespace JFPGen {

float convertSpeedToFloat(const std::string& speed) {
    if (speed == "0.5x") return 0.5f;
    else if (speed == "1x") return 1.0f;
    else if (speed == "2x") return 2.0f;
    else if (speed == "4x") return 4.0f;
    return 3.0f; // default speed
}

float convertSpeedToFloat(SpeedChange speed) {
    switch (speed) {
        case SpeedChange::Speed05x: return 0.5f;
        case SpeedChange::Speed1x:  return 1.0f;
        case SpeedChange::Speed2x:  return 2.0f;
        case SpeedChange::Speed3x:  return 3.0f;
        case SpeedChange::Speed4x:  return 4.0f;
        default: return 3.0f;
    }
}

bool orientationMatch(const std::vector<Segment>& segments, int idx, const std::vector<int>& pattern) {
    if (idx < static_cast<int>(pattern.size())) return false;
    for (int i = 0; i < pattern.size(); i++) {
        if (segments[idx - pattern.size() + i].y_swing != pattern[i]) {
            return false;
        }
    }
    return true;
}

bool orientationMatch(int prevO[11], const std::vector<int> pattern) {
    if(pattern.size()>11) return false;
    for(int i = 0; i < pattern.size(); i++) {
        if(pattern[i] != prevO[i+(11-pattern.size())]) {
            return false;
        }
    }
    return true;
}

std::map<std::string, int> portalOddsMap = {
	{"Light", 40},
	{"Balanced", 10},
	{"Aggressive", 3},
};

std::map<std::string, int> speedOddsMap = {
	{"Light", 60},
	{"Balanced", 10},
	{"Aggressive", 5},
};

LevelData generateJFPLevel() {
    const bool optChangingSpeed = Mod::get()->getSettingValue<bool>("changing-speed");
    const bool optSpikes = Mod::get()->getSettingValue<bool>("corridor-spikes");
    const bool optFakePortals = Mod::get()->getSettingValue<bool>("fake-gravity-portals");
    const bool optFuzz = Mod::get()->getSettingValue<bool>("fuzzy-spikes");
    const bool optLowvis = Mod::get()->getSettingValue<bool>("low-vis");
    const bool optTeleportals = Mod::get()->getSettingValue<bool>("teleportals");
    bool gravity = Mod::get()->getSettingValue<bool>("upside-start");
    std::string colorModeStr = Mod::get()->getSettingValue<std::string>("color-mode");
    ColorMode optColorMode = ColorMode::Washed;
    if (colorModeStr == "All Colors") optColorMode = ColorMode::AllColors;
    else if (colorModeStr == "Classic Mode") optColorMode = ColorMode::ClassicMode;
    else if (colorModeStr == "Night Mode") optColorMode = ColorMode::NightMode;
    log::info("{}", colorModeStr);

    std::string corridorRulesStr = Mod::get()->getSettingValue<std::string>("corridor-rules");
    CorridorRules optCorridorRules = CorridorRules::Unrestricted;
    if (corridorRulesStr == "No Spam") optCorridorRules = CorridorRules::NoSpam;
    else if (corridorRulesStr == "No Spam, No Zigzagging") optCorridorRules = CorridorRules::NoSpamNoZigzag;

    std::string portalsStr = Mod::get()->getSettingValue<std::string>("portals");
    Difficulties optPortals = Difficulties::None;
    if (portalsStr == "Light") optPortals = Difficulties::Light;
    else if (portalsStr == "Balanced") optPortals = Difficulties::Balanced;
    else if (portalsStr == "Aggressive") optPortals = Difficulties::Aggressive;

    auto getSpeedChange = [](const std::string& speedStr) -> SpeedChange {
        if (speedStr == "0.5x") return SpeedChange::Speed05x;
        else if (speedStr == "1x") return SpeedChange::Speed1x;
        else if (speedStr == "2x") return SpeedChange::Speed2x;
        else if (speedStr == "3x") return SpeedChange::Speed3x;
        else if (speedStr == "4x") return SpeedChange::Speed4x;
        return SpeedChange::None;
    };

    std::string maxSpeedStr = Mod::get()->getSettingValue<std::string>("max-speed");
    std::string minSpeedStr = Mod::get()->getSettingValue<std::string>("min-speed");
    std::string speedStr = Mod::get()->getSettingValue<std::string>("speed");
    SpeedChange optMaxSpeed = getSpeedChange(maxSpeedStr);
    SpeedChange optMinSpeed = getSpeedChange(minSpeedStr);
    SpeedChange optSpeed = getSpeedChange(speedStr);

    const double optCorridorHeight = Mod::get()->getSettingValue<double>("corridor-height");

    const int64_t optLength = Mod::get()->getSettingValue<int64_t>("length");
    int y_swing = 0, cX = 435, cY = 195;
    int maxHeight = 195, minHeight = 45;

    std::vector<Segment> segments(optLength);

    LevelData levelData = {
        .name = "JFP Level",
        .biomes = {
            {
                .x_initial = cX,
                .y_initial = cY,
                .type = "Default",
                .theme = "Classic",
                .options = {
                    .length = static_cast<int>(optLength),
                    .corridorHeight = static_cast<int>(optCorridorHeight),
                    .maxHeight = maxHeight,
                    .minHeight = minHeight,
                    .visibility = Mod::get()->getSettingValue<bool>("low-vis") ? Visibility::Low : Visibility::Standard,
                    .startingGravity = gravity,
                    .startingSpeed = SpeedChange::Speed3x,
                    .colorMode = optColorMode,
                    .bgColor = {28, 28, 28},
                    .lineColor = {255, 255, 255}
                },
                .segments = segments
            }
        }
    };

    // random device setups - used with modulo to generate numbers in a range
    std::random_device rd;
    unsigned int seed = 0;
    try {
        std::string seedStr = Mod::get()->getSettingValue<std::string>("seed");
        if(!seedStr.empty() && state == AutoJFP::NotInAutoJFP) seed = std::stoul(seedStr);
    } catch(const std::exception &e) {
        return levelData; // Return base level data on error
    }
    
    if(seed == 0) seed = rd();
    std::mt19937 segmentRNG(seed);
    std::mt19937 portalRNG(seed);
    std::mt19937 fakePortalRNG(seed);
    std::mt19937 songRNG(seed);
    std::mt19937 bgRNG(seed);

    std::vector<int> antiZigzagMax = {1,-1,1,-1,-1,1,-1,1,1,-1};
    std::vector<int> antiZigzagMin = {-1,1,-1,1,1,-1,1,-1,-1,1};
    std::vector<int> antiZigzagStd1 = {-1,1,-1,1,1,-1,1,-1,-1,1,-1};
    std::vector<int> antiZigzagStd2 = {1,-1,1,-1,-1,1,-1,1,1,-1,1};
    std::vector<int> antiSpam1 = {1,-1,1,-1};
    std::vector<int> antiSpam2 = {-1,1,-1,1};
    std::vector<int> antiTpspam1 = {-1, -1};
    std::vector<int> antiTpspam2 = {1, 1};

    int portalOdds = 1;
    int fakePortalOdds = 1;
    int spikeOdds = 1;
    int last_tp = 0;

    static bool spikeActive = false;
    static bool spikeSideHold = false;
    static int spikeSide = 0;
    float current_speed = convertSpeedToFloat(optSpeed);
    
    std::array<int, 3> backgroundColor = {28, 28, 28};
    std::array<int, 3> lineColor = {255, 255, 255};
    if (optColorMode == JFPGen::ColorMode::NightMode) {
        backgroundColor = {0, 0, 0};
        int excluded = bgRNG() % 3;
        int maxed = bgRNG() % 3;
        if (excluded == maxed && excluded == 2) maxed = 0;
        else if (maxed == excluded) maxed += 1;

        lineColor[excluded] = 0;
        lineColor[maxed] = 255;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded || i == maxed) continue;
            lineColor[i] -= bgRNG() % 256;
        }
    }
    else if (optColorMode == JFPGen::ColorMode::AllColors) {
        backgroundColor = {0, 0, 0};
        int excluded = bgRNG() % 3;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded) continue;
            backgroundColor[i] += bgRNG() % 256;
        }
    }
    else if (optColorMode == JFPGen::ColorMode::ClassicMode) {
        backgroundColor = {128, 128, 128};
    }
    else {
        backgroundColor = {28, 28, 28};
        int excluded = bgRNG() % 3;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded) continue;
            backgroundColor[i] += bgRNG() % 17;
            if (backgroundColor[i] > 255) backgroundColor[i] = 255;
        }
    }
    // Set the background and line colors in the level data
    levelData.biomes[0].options.bgColor[0] = backgroundColor[0];
    levelData.biomes[0].options.bgColor[1] = backgroundColor[1];
    levelData.biomes[0].options.bgColor[2] = backgroundColor[2];
    levelData.biomes[0].options.lineColor[0] = lineColor[0];
    levelData.biomes[0].options.lineColor[1] = lineColor[1];
    levelData.biomes[0].options.lineColor[2] = lineColor[2];

    // future prior block to generate lengths of different biomes
    
    for (int i = 0; i < optLength; i++) {
        cX += 30;
        y_swing = 0;

        if (last_tp <= 1 && i > 1) {
            y_swing = segments[i - 1].y_swing;
        } else if (
            cY >= maxHeight &&(segments[i - 1].y_swing == 1 ||
            (
                optCorridorRules == CorridorRules::NoSpamNoZigzag &&
                orientationMatch(segments, i, antiZigzagMax)
            ) ||
            orientationMatch(segments, i, antiTpspam1) ||
            i == 1
        )) {
            y_swing = -1;
        } else if (
            cY <= minHeight && (segments[i - 1].y_swing == -1 ||
            (
                optCorridorRules == CorridorRules::NoSpamNoZigzag &&
                orientationMatch(segments, i, antiZigzagMin)
            ) ||
            orientationMatch(segments, i, antiTpspam2)
        )) {
            y_swing = 1;
        } else if (
            optCorridorRules == CorridorRules::NoSpamNoZigzag && orientationMatch(segments, i, antiZigzagStd1)) {
            y_swing = -1;
        } else if (
            optCorridorRules == CorridorRules::NoSpamNoZigzag && orientationMatch(segments, i, antiZigzagStd2)) {
            y_swing = 1;
        } else if ((
            optCorridorRules == CorridorRules::NoSpam ||
            optCorridorRules == CorridorRules::NoSpamNoZigzag
        ) && (
            orientationMatch(segments, i, antiSpam1) ||
            (i == 2 && orientationMatch(segments, i, {1, -1}))
        )) {
            y_swing = -1;
        } else if ((
            optCorridorRules == CorridorRules::NoSpam ||
            optCorridorRules == CorridorRules::NoSpamNoZigzag
        ) && orientationMatch(segments, i, antiSpam2)) {
            y_swing = 1;
        } else {
            // randomized coinflip condition
            y_swing = segmentRNG() % 2;
            if(y_swing == 0) y_swing = -1;
        }
        
        if ((i == 0 && y_swing == -1) || segments[i - 1].y_swing == y_swing) {
            cY += (y_swing * 30);
        }

        if (optPortals != Difficulties::None && segments[i - 1].y_swing != y_swing) {
            portalOdds = portalRNG() % portalOddsMap.at(portalsStr);
            fakePortalOdds = optFakePortals ? (fakePortalRNG() % 10) : 1;
            if (portalOdds == 0 || fakePortalOdds == 0) {
                log::info("Portal odds: {} {}", portalOdds, fakePortalOdds);
                gravity = portalOdds == 0 ? !gravity : gravity;
            }
        }

        if (optSpikes && segments[i - 1].y_swing != y_swing) {
            spikeSideHold = false;
            spikeOdds = segmentRNG() % 6;
            if (spikeOdds == 0) {
                if (spikeActive) spikeSideHold = true;
                if (!spikeSideHold) spikeSide = segmentRNG() % 2;
            } else {
                spikeActive = false;
            }
        }
        if (spikeActive) {
            segments[i].options.isSpikeM = true;
        }

        if (optFuzz) {
            segments[i].options.isFuzzy = true;
        }
        segments[i] = Segment{
            .coords = std::make_pair(cX, cY),
            .y_swing = y_swing,
            .options = {
                .gravity = gravity,
                .isSpikeM = segments[i].options.isSpikeM,
                .cornerPieces = (optCorridorRules == CorridorRules::Unrestricted),
                .isPortal = (optPortals != Difficulties::None && portalOdds == 0) ? Portals::Gravity : Portals::None,
                .speedChange = (optMaxSpeed != SpeedChange::None && optMinSpeed != SpeedChange::None) ? optMinSpeed : SpeedChange::None,
                .isFakePortal = (fakePortalOdds == 0),
                .isFuzzy = segments[i].options.isFuzzy
            }
        };
        // log::info(
        //     "Segment[{}]: coords=({}, {}), y_swing={}, gravity={}, isSpikeM={}, cornerPieces={}, isPortal={}, speedChange={}, isFakePortal={}, isFuzzy={}",
        //     i,
        //     segments[i].coords.first,
        //     segments[i].coords.second,
        //     segments[i].y_swing,
        //     segments[i].options.gravity,
        //     segments[i].options.isSpikeM,
        //     static_cast<int>(segments[i].options.cornerPieces),
        //     static_cast<int>(segments[i].options.isPortal),
        //     static_cast<int>(segments[i].options.speedChange),
        //     segments[i].options.isFakePortal,
        //     segments[i].options.isFuzzy
        // );

        // log::info(
        //     "Segment[{}]: cX={}, cY={}, y_swing={}",
        //     i, cX, cY, y_swing
        // );

        if (optChangingSpeed) {
            if (i % 10 == 0) {
                segments[i].options.speedChange = static_cast<SpeedChange>(segmentRNG() % 5);
                current_speed = convertSpeedToFloat(segments[i].options.speedChange);
            }
        } else {
            segments[i].options.speedChange = SpeedChange::None;
            current_speed = convertSpeedToFloat(optSpeed);
        }

        if(optTeleportals && last_tp > 2 &&
            ((cY >= maxHeight && segments[i].y_swing == 1) || (cY <= minHeight && segments[i].y_swing == -1)) &&
            cX + 120 < 435 + (optLength - 1) * 30) {
            int portalOdds = portalRNG() % ((last_tp < 40) ? (50 - last_tp) : 10);
            if (portalOdds == 0) {
                segments[i].options.isPortal = Portals::Teleportal;
                last_tp = 0; // reset teleportal counter
            }
        }
        if (last_tp < 40) last_tp += 1;
    }

    levelData.biomes[0].segments = segments;

    return levelData;
};

}