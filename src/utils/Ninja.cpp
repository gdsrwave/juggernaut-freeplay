#include <Geode/Geode.hpp>
#include "Ninja.hpp"
#include <random>
#include "constants.hpp"
#include <map>

using namespace geode::prelude;
JFPGen::AutoJFP state = JFPGen::AutoJFP::NotInAutoJFP;
// Use std::vector for colorBank, but provide a pushOrReplaceColor function
std::vector<JFPGen::Color> colorBank = {
    JFPGen::Color{1000, {128, 128, 128}, false, 1.0f},
    JFPGen::Color{1001, {25, 24, 24}, false, 1.0f},
    JFPGen::Color{1009, {0, 102, 255}, false, 1.0f},
    JFPGen::Color{1002, {255, 255, 255}, true, 1.0f},
    JFPGen::Color{1004, {255, 255, 255}, false, 1.0f},
    JFPGen::Color{1005, {255, 75, 0}, true, 1.0f},
    JFPGen::Color{1006, {255, 75, 0}, true, 1.0f},
    JFPGen::Color{101, {0, 0, 0}, false, 1.0f}
};

void pushColor(const JFPGen::Color& color) {
    for (auto& c : colorBank) {
        if (c.slot == color.slot) {
            c = color;
            return;
        }
    }
    colorBank.push_back(color);
}

unsigned int globalSeed = 0;

std::map<std::string, std::string> kBank = {
    {"kA13", "0"},
    {"kA15", "0"},
    {"kA16", "0"},
    {"kA14", ""},
    {"kA6", "0"},
    {"kA7", "7"},
    {"kA25", "0"},
    {"kA17", "0"},
    {"kA18", "0"},
    {"kS39", "0"},
    {"kA2", "0"},
    {"kA3", "0"},
    {"kA8", "0"},
    {"kA4", "1"},
    {"kA9", "0"},
    {"kA10", "0"},
    {"kA22", "0"},
    {"kA23", "0"},
    {"kA24", "0"},
    {"kA27", "0"},
    {"kA40", "0"},
    {"kA41", "0"},
    {"kA42", "0"},
    {"kA28", "0"},
    {"kA29", "0"},
    {"kA31", "0"},
    {"kA32", "0"},
    {"kA36", "0"},
    {"kA43", "0"},
    {"kA44", "0"},
    {"kA45", "0"},
    {"kA46", "0"},
    {"kA33", "0"},
    {"kA34", "0"},
    {"kA35", "0"},
    {"kA37", "0"},
    {"kA38", "0"},
    {"kA39", "0"},
    {"kA19", "0"},
    {"kA26", "0"},
    {"kA20", "0"},
    {"kA21", "0"},
    {"kA11", "0"}
};

std::map<std::string, bool> overrideBank = {
    {"override-base", false},
    {"override-enddown", false},
    {"override-endup", false}
};

namespace JFPGen {

int convertFloatSpeed(float speed) {
	if (speed == 0.5f) return 200;
	else if (speed == 1.0f) return 201;
	else if (speed == 2.0f) return 202;
	else if (speed == 4.0f) return 1334;
	return 203; // default speed
}

SpeedChange convertFloatSpeedEnum(float speed) {
    if (speed == 0.5f) return SpeedChange::Speed05x;
    else if (speed == 1.0f) return SpeedChange::Speed1x;
    else if (speed == 2.0f) return SpeedChange::Speed2x;
    else if (speed == 3.0f) return SpeedChange::Speed3x;
    else if (speed == 4.0f) return SpeedChange::Speed4x;
    return SpeedChange::None; // default
}

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
    if (pattern.size()>11) return false;
    for(int i = 0; i < pattern.size(); i++) {
        if (pattern[i] != prevO[i+(11-pattern.size())]) {
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
    const bool optSpikes = Mod::get()->getSettingValue<bool>("corridor-spikes");
    const bool optFakePortals = Mod::get()->getSettingValue<bool>("fake-gravity-portals");
    const bool optFuzz = Mod::get()->getSettingValue<bool>("fuzzy-spikes");
    const bool optLowvis = Mod::get()->getSettingValue<bool>("low-vis");
    const bool optTeleportals = false; // TODO: Bring Teleportals back
    bool gravity = Mod::get()->getSettingValue<bool>("upside-start");

    std::string colorModeStr = Mod::get()->getSettingValue<std::string>("color-mode");
    ColorMode optColorMode = ColorMode::Washed;
    if (colorModeStr == "All Colors") optColorMode = ColorMode::AllColors;
    else if (colorModeStr == "Classic Mode") optColorMode = ColorMode::ClassicMode;
    else if (colorModeStr == "Night Mode") optColorMode = ColorMode::NightMode;

    std::string corridorRulesStr = Mod::get()->getSettingValue<std::string>("corridor-rules");
    CorridorRules optCorridorRules = CorridorRules::Unrestricted;
    if (corridorRulesStr == "No Spam") optCorridorRules = CorridorRules::NoSpam;
    else if (corridorRulesStr == "NSNZ") optCorridorRules = CorridorRules::NoSpamNoZigzag;
    else if (corridorRulesStr == "Juggernaut") optCorridorRules = CorridorRules::Juggernaut;

    std::string portalInputsStr = Mod::get()->getSettingValue<std::string>("portal-inputs");
    PortalInputs optPortalInputs = PortalInputs::Both;
    if (portalInputsStr == "Releases") optPortalInputs = PortalInputs::Releases;
    else if (portalInputsStr == "Holds") optPortalInputs = PortalInputs::Holds;

    std::string portalsStr = Mod::get()->getSettingValue<std::string>("portals");
    Difficulties optPortals = Difficulties::None;
    if (portalsStr == "Light") optPortals = Difficulties::Light;
    else if (portalsStr == "Balanced") optPortals = Difficulties::Balanced;
    else if (portalsStr == "Aggressive") optPortals = Difficulties::Aggressive;

    std::string cspeedStr = Mod::get()->getSettingValue<std::string>("changing-speed");
    Difficulties optChangingSpeed = Difficulties::None;
    if (cspeedStr == "Light") optChangingSpeed = Difficulties::Light;
    else if (cspeedStr == "Balanced") optChangingSpeed = Difficulties::Balanced;
    else if (cspeedStr == "Aggressive") optChangingSpeed = Difficulties::Aggressive;

    std::string ssizeStr = Mod::get()->getSettingValue<std::string>("starting-size");
    bool mini = false;
    if (ssizeStr == "Mini") mini = true;
    bool changingSize = Mod::get()->getSettingValue<bool>("changing-size");

    std::string scTypeStr = Mod::get()->getSettingValue<std::string>("transition-type");
    bool typeA = true;
    if (scTypeStr == "Type B") typeA = false;

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
    float maxSpeedFloat = convertSpeedToFloat(optMaxSpeed);
    float minSpeedFloat = convertSpeedToFloat(optMinSpeed);

    double optCorridorHeight = Mod::get()->getSettingValue<double>("corridor-height");

    const int64_t optLength = Mod::get()->getSettingValue<int64_t>("length");
    int y_swing = 0, cX = 345, cY = 135;
    int maxHeight = 255, minHeight = 45;

    if (mini) cY -= 30;
    if (Mod::get()->getSettingValue<bool>("corridor-widening")) {
        maxHeight += 30;
        minHeight -= 30;
    }

    std::vector<Segment> segments(optLength);

    LevelData levelData = {
        .name = "JFP Level",
        .seed = 0,
        .biomes = {
            {
                .x_initial = cX,
                .y_initial = cY,
                .type = Biomes::Juggernaut,
                .theme = "Classic",
                .song = 234565,
                .options = {
                    .length = static_cast<int>(optLength),
                    .corridorHeight = static_cast<int>(optCorridorHeight),
                    .startingMini = mini,
                    .maxHeight = maxHeight,
                    .minHeight = minHeight,
                    .visibility = Mod::get()->getSettingValue<bool>("low-vis") ? Visibility::Low : Visibility::Standard,
                    .startingGravity = gravity,
                    .startingSpeed = optSpeed,
                    .colorMode = optColorMode,
                    .bgColor = {28, 28, 28},
                    .lineColor = {255, 255, 255},
                    .sizeTransitionTypeA = typeA
                },
                .segments = segments
            }
        }
    };

    // random device setups - used with modulo to generate integers in a range
    std::random_device rd;
    unsigned int seed = 0;
    try {
        std::string seedStr = Mod::get()->getSettingValue<std::string>("seed");
        if (!seedStr.empty()) seed = std::stoul(seedStr);
    } catch(const std::exception &e) {
        return levelData;
    }
    if (seed == 0) seed = rd();
    levelData.seed = seed;
    globalSeed = seed;

    std::mt19937 segmentRNG(seed);
    std::mt19937 portalRNG(seed);
    std::mt19937 fakePortalRNG(seed);
    std::mt19937 songRNG(seed);
    std::mt19937 bgRNG(seed);
    std::mt19937 speedRNG(seed);
    std::mt19937 spikeRNG(seed);
    std::mt19937 sizeRNG(seed);

    std::vector<int> antiZigzagMax = {1,-1,1,-1,-1,1,-1,1,1,-1};
    std::vector<int> antiZigzagMin = {-1,1,-1,1,1,-1,1,-1,-1,1};
    std::vector<int> antiZigzagStd1 = {-1,1,-1,1,1,-1,1,-1,-1,1,-1};
    std::vector<int> antiZigzagStd2 = {1,-1,1,-1,-1,1,-1,1,1,-1,1};
    std::vector<int> antiSpam1 = {1,-1,1,-1};
    std::vector<int> antiSpam2 = {-1,1,-1,1};
    std::vector<int> antiTpspam1 = {-1, -1};
    std::vector<int> antiTpspam2 = {1, 1};
    std::vector<int> antiSpeedspam1 = {1, -1};
    std::vector<int> antiSpeedspam2 = {-1, 1};
    
    const int corridorStart[3] = {1, 1, -1};

    int portalOdds = 1;
    int fakePortalOdds = 1;
    int spikeOdds = 1;
    int last_tp = 0;
    int lastSize = 0;

    bool spikeActive = false;
    bool midCorridorPortal = true;
    bool spikeSideHold = false;
    int spikeSide = 0;
    int relMaxHeight = maxHeight - optCorridorHeight;
    float currentSpeed = convertSpeedToFloat(optSpeed);
    bool specialRules = false;
    Portals currentPortal;

    levelData.biomes[0].song = jfpSoundtrack[songRNG() % (jfpSoundtrackSize)];

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

    levelData.biomes[0].options.bgColor[0] = backgroundColor[0];
    levelData.biomes[0].options.bgColor[1] = backgroundColor[1];
    levelData.biomes[0].options.bgColor[2] = backgroundColor[2];
    levelData.biomes[0].options.lineColor[0] = lineColor[0];
    levelData.biomes[0].options.lineColor[1] = lineColor[1];
    levelData.biomes[0].options.lineColor[2] = lineColor[2];

    pushColor(JFPGen::Color{1000, backgroundColor, false, 1.0f});
    pushColor(JFPGen::Color{1004, lineColor, false, 1.0f});


    // future prior block to generate lengths of different biomes

    for (int i = 0; i < optLength; i++) {
        cX += 30;
        y_swing = 0;
        currentPortal = Portals::None;

        if (optCorridorRules == CorridorRules::Juggernaut &&
            (cY == minHeight+30 || cY == relMaxHeight-30) &&
            (i > 0 && (segments[i - 1].coords.second == minHeight || segments[i - 1].coords.second == relMaxHeight))
        ) {
            specialRules = true;
        }

        if (i < 3 && !mini) {
            y_swing = corridorStart[i];
        } else if (i == 0 && mini) {
            y_swing = 1;
        } else if (last_tp <= 1 && i > 1) {
            y_swing = segments[i - 1].y_swing;
        } else if (
            cY >= relMaxHeight &&(segments[i - 1].y_swing == 1 ||
            (
                optCorridorRules == CorridorRules::NoSpamNoZigzag &&
                orientationMatch(segments, i, antiZigzagMax)
            ) ||
            orientationMatch(segments, i, antiTpspam1) ||
            i == 1
        )) {
            y_swing = -1;
        } else if (
            cY <= (mini ? minHeight + 30 : minHeight) && (segments[i - 1].y_swing == -1 ||
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
            optCorridorRules != CorridorRules::Unrestricted
        ) && (
            orientationMatch(segments, i, antiSpam1) &&
            (optCorridorRules != CorridorRules::Juggernaut ||
                !(gravity && cY != relMaxHeight)
            )
        )) {
            y_swing = -1;
        } else if ((
            optCorridorRules != CorridorRules::Unrestricted
        ) && (
            orientationMatch(segments, i, antiSpam2) &&
            (optCorridorRules != CorridorRules::Juggernaut ||
                !(!gravity && cY != minHeight)))) {
            y_swing = 1;
        } else {
            // special condition used in Juggernaut rules
            if (specialRules) {
                y_swing = segmentRNG() % 4;
                if (y_swing > 0) y_swing = 1;
                else y_swing = -1;
                if (cY >= relMaxHeight - 30) {
                    y_swing *= -1;
                }
            } else {
                // 50/50 corridor direction flip
                y_swing = segmentRNG() % 2;
                if (y_swing == 0) y_swing = -1;
            }
        }

        if (i > 0 && changingSize && lastSize > 2 &&
                (!typeA || !(!mini && y_swing == -1 && segments[i - 1].y_swing == -1 && cY <= minHeight + 30)) &&
                sizeRNG() % std::max(10, 50 - lastSize) == 0) {
            mini = !mini;
            lastSize = 0;
        }

        if (i > 0 && typeA && mini && !segments[i - 1].options.mini) {
            relMaxHeight -= 30;
        } else if (i > 0 && typeA && !mini && segments[i - 1].options.mini) {
            relMaxHeight += 30;
        }

        // special type B transitional cases
        if (!typeA && segments[i - 1].options.mini != mini) {
            if (mini) {
                if (segments[i - 1].y_swing == -1) cY += 30;
            } else {
                if (segments[i - 1].y_swing == -1) cY -= 30;
            }
        }
        
        if ((i == 0 && y_swing == 1) || segments[i - 1].y_swing == y_swing) {
            int mfac = 1;
            if (mini) {
                mfac = 2;
            }
            cY += mfac * (y_swing * 30);
        }

        lastSize++;

        for (int fakeRngCount = 0; fakeRngCount < 10; ++fakeRngCount) {
            volatile auto _ = fakePortalRNG();
        }
        if (i > 0 && !mini && optPortals == Difficulties::Aggressive && portalRNG() % 4 == 0) {
            if (y_swing == 1 &&
                orientationMatch(segments, i, {1, 1, 1, 1}) &&
                gravity && midCorridorPortal
            ) {
                segments[i-2].options.isPortal = Portals::Gravity;
                segments[i-2].options.gravity = !segments[i-2].options.gravity;
                segments[i-1].options.gravity = !segments[i-1].options.gravity;
                gravity = !gravity;
                midCorridorPortal = false;
            } else if (y_swing == -1 &&
                orientationMatch(segments, i, {-1, -1, -1, -1}) &&
                !gravity && midCorridorPortal
            ) {
                segments[i-2].options.isPortal = Portals::Gravity;
                segments[i-2].options.gravity = !segments[i-2].options.gravity;
                segments[i-1].options.gravity = !segments[i-1].options.gravity;
                gravity = !gravity;
                midCorridorPortal = false;
            }
        }
        if (!midCorridorPortal && segments[i - 1].y_swing != y_swing) midCorridorPortal = true;
        if (i > 0 && optPortals != Difficulties::None && segments[i - 1].y_swing != y_swing) {
            portalOdds = portalRNG() % portalOddsMap.at(portalsStr);

            if (
                optPortalInputs == PortalInputs::Releases && (
                    (segments[i - 1].y_swing == 1 && y_swing == -1 && !gravity) ||
                    (segments[i - 1].y_swing == -1 && y_swing == 1 && gravity)
                )
            ) {
                portalOdds = -1;
            } else if (
                optPortalInputs == PortalInputs::Holds && (
                    (segments[i - 1].y_swing == 1 && y_swing == -1 && gravity) ||
                    (segments[i - 1].y_swing == -1 && y_swing == 1 && !gravity)
                )
            ) {
                portalOdds = -1;
            }
            fakePortalOdds = optFakePortals ? (fakePortalRNG() % 12) : 1;
            if (portalOdds == 0 || fakePortalOdds == 0) {
                //log::info("Portal odds: {} {} {}", portalOdds, cX, cY);
                gravity = portalOdds == 0 ? !gravity : gravity;
                currentPortal = (portalOdds == 0) ? Portals::Gravity : Portals::Fake;
            }
        }

        int speedOdds = 1;
        if (optChangingSpeed != Difficulties::None &&
            (
                (orientationMatch(segments, i, antiSpeedspam1) && y_swing == -1) ||
                (orientationMatch(segments, i, antiSpeedspam2) && y_swing == 1)
            ) &&
            maxSpeedFloat > minSpeedFloat
        ) {
            speedOdds = speedRNG() % speedOddsMap.at(cspeedStr); // You can adjust the odds as needed
            if (speedOdds == 0) {
                double speedFactor = 0.5 * (optCorridorHeight / 60.0);
                int spY = cY + optCorridorHeight / 2 + (optCorridorHeight / 4) * ((segments[i].y_swing == 1) ? -1 : 1);
                int spR = (segments[i].y_swing == 1) ? -45 : 45;
                float newSpeed = currentSpeed;
                int minSpeedMod = (minSpeedFloat == 0.5) ? 0 : minSpeedFloat;

                for (int tries = 0; tries < 10 && newSpeed == currentSpeed; ++tries) {
                    if (maxSpeedFloat == 0.5) break;
                    newSpeed = minSpeedMod + (speedRNG() % (static_cast<int>(maxSpeedFloat - minSpeedMod + 1)));
                    if (newSpeed == 0) newSpeed = 0.5;
                }
                SpeedChange newSpeedEnum = convertFloatSpeedEnum(newSpeed);
                //log::info("SpeedChange: {} (enum int: {})", newSpeed, static_cast<int>(newSpeedEnum));
                currentSpeed = newSpeed;
            }
        }

        if (optSpikes && segments[i - 1].y_swing != y_swing && currentPortal == Portals::None) {
            spikeSideHold = false;
            spikeOdds = spikeRNG() % 6;
            if (spikeOdds == 0) {
                if (spikeActive) spikeSideHold = true;
                spikeActive = true;
                if (!spikeSideHold) spikeSide = spikeRNG() % 2;
            } else {
                spikeActive = false;
            }
        }

        if (optSpikes && spikeActive) {
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
                .spikeSide = static_cast<bool>(spikeSide),
                .cornerPieces = (optCorridorRules == CorridorRules::Unrestricted),
                .isPortal = currentPortal,
                .speedChange = speedOdds == 0 ? convertFloatSpeedEnum(currentSpeed) : SpeedChange::None,
                .isFuzzy = segments[i].options.isFuzzy,
                .mini = mini
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
        //     segments[i].options.isFuzzy
        // );

        // log::info(
        //     "Segment[{}]: cX={}, cY={}, y_swing={}",
        //     i, cX, cY, y_swing
        // );

        if (optTeleportals && last_tp > 2 &&
            ((cY >= maxHeight && segments[i].y_swing == 1) || (cY <= minHeight && segments[i].y_swing == -1)) &&
            cX + 120 < 435 + (optLength - 1) * 30) {
            int portalOdds = portalRNG() % ((last_tp < 40) ? (50 - last_tp) : 10);
            if (portalOdds == 0) {
                segments[i].options.isPortal = Portals::Teleportal;
                last_tp = 0;
            }
        }
        if (last_tp < 40) last_tp += 1;
    }

    levelData.biomes[0].segments = segments;

    return levelData;
};

}