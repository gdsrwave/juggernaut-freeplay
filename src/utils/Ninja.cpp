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

LevelData generateJFPLevel() {
    const bool optChangingSpeed = Mod::get()->getSettingValue<bool>("changing-speed");
    const bool optSpikes = Mod::get()->getSettingValue<bool>("corridor-spikes");
    const bool optFakePortals = Mod::get()->getSettingValue<bool>("fake-gravity-portals");
    const bool optFuzz = Mod::get()->getSettingValue<bool>("fuzzy-spikes");
    const bool optLowvis = Mod::get()->getSettingValue<bool>("low-vis");
    const bool optTeleportals = Mod::get()->getSettingValue<bool>("teleportals");
    bool gravity = Mod::get()->getSettingValue<bool>("upside-start");

    const CorridorRules optCorridorRules = static_cast<CorridorRules>(Mod::get()->getSettingValue<int>("corridor-rules"));

    const Difficulties optPortals = static_cast<Difficulties>(Mod::get()->getSettingValue<int>("portals"));

    const SpeedChange optMaxSpeed = static_cast<SpeedChange>(Mod::get()->getSettingValue<int>("max-speed"));
    const SpeedChange optMinSpeed = static_cast<SpeedChange>(Mod::get()->getSettingValue<int>("min-speed"));
    const SpeedChange optSpeed = static_cast<SpeedChange>(Mod::get()->getSettingValue<int>("speed"));

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
                    .corridorHeight = static_cast<int>(Mod::get()->getSettingValue<double>("corridor-height")),
                    .maxHeight = maxHeight,
                    .minHeight = minHeight,
                    .visibility = Mod::get()->getSettingValue<bool>("low-vis") ? Visibility::Low : Visibility::Standard,
                    .startingGravity = gravity,
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

    // future prior block to generate lengths of different biomes
    
    for (int i = 0; i < optLength - 1; i++) {
        cX += 30;
        y_swing = 0;

        if (last_tp <= 1) {
            y_swing = i == 0 ? -1 : segments[i - 1].y_swing;
        } else if (
            cY >= maxHeight &&(segments[i - 1].y_swing == 1 ||
            (
                optCorridorRules == CorridorRules::NoSpamNoZigzag &&
                orientationMatch(segments, i, antiZigzagMax)
            ) ||
            orientationMatch(segments, i, antiTpspam1)
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
        ) && orientationMatch(segments, i, antiSpam1)) {
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
        
        if (segments.size() > 0 && segments[i - 1].y_swing == y_swing) {
            cY += (y_swing * 30);
        }

        if (optPortals != Difficulties::None && segments[i - 1].y_swing != y_swing) {
            portalOdds = portalRNG() % static_cast<int>(optPortals);
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

        log::info(
            "Segment[{}]: cX={}, cY={}, y_swing={}",
            i, cX, cY, y_swing
        );
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