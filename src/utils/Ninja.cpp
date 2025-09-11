// Copyright 2025 GDSRWave
#include "Ninja.hpp"
#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <Geode/Geode.hpp>
#include "./shared.hpp"

using namespace geode::prelude;

JFPGen::AutoJFP state = JFPGen::AutoJFP::NotInAutoJFP;

bool att1 = false;
int globalAtt = 1;
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
    {"override-endup", false},
    {"override-start", false},
    {"override-slope", false},
    {"override-slope-mini", false}
};

namespace JFPGen {

int convertFloatSpeed(float speed) {
    if (speed == 0.5f) return 200;
    else if (speed == 1.0f) return 201;
    else if (speed == 2.0f) return 202;
    else if (speed == 4.0f) return 1334;
    return 203;
}

SpeedChange convertFloatSpeedEnum(float speed) {
    if (speed == 0.5f) return SpeedChange::Speed05x;
    else if (speed == 1.0f) return SpeedChange::Speed1x;
    else if (speed == 2.0f) return SpeedChange::Speed2x;
    else if (speed == 3.0f) return SpeedChange::Speed3x;
    else if (speed == 4.0f) return SpeedChange::Speed4x;
    return SpeedChange::Random;
}

float convertSpeedToFloat(const std::string& speed) {
    if (speed == "0.5x") return 0.5f;
    else if (speed == "1x") return 1.0f;
    else if (speed == "2x") return 2.0f;
    else if (speed == "4x") return 4.0f;
    return 3.0f;
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

bool orientationMatch(const std::vector<Segment>& segments, int idx,
        const std::vector<int>& pattern, bool strictMini) {
    if (idx < static_cast<int>(pattern.size())) return false;
    int y_swing = 0;
    for (int i = 0; i < pattern.size(); i++) {
        auto seg = segments[idx - pattern.size() + i];
        y_swing = seg.y_swing;
        if (strictMini && seg.options.mini) y_swing *= 2;
        if ((strictMini && seg.options.isTransition) || y_swing != pattern[i])
            return false;
    }
    return true;
}

// legacy
bool orientationMatch(int prevO[11], const std::vector<int> pattern) {
    if (pattern.size() > 11) return false;
    for (int i = 0; i < pattern.size(); i++) {
        if (pattern[i] != prevO[i+(11-pattern.size())]) {
            return false;
        }
    }
    return true;
}

std::map<Difficulties, int> portalOddsMap = {
    {Difficulties::Light, 40},
    {Difficulties::Balanced, 10},
    {Difficulties::Aggressive, 3},
};

std::map<Difficulties, int> speedOddsMap = {
    {Difficulties::Light, 60},
    {Difficulties::Balanced, 10},
    {Difficulties::Aggressive, 5},
};

LevelData generateJFPLevel() {
    auto* mod = Mod::get();

    // random device setups - used with modulo to generate integers in a range
    std::random_device rd;
    uint32_t seed = 0;
    if (mod->getSavedValue<bool>("opt-0-using-set-seed")) {
        seed = mod->getSavedValue<uint32_t>("opt-0-seed");
    } else {
        seed = rd();
    }

    mod->setSavedValue<uint32_t>("global-seed", seed);

    std::mt19937 segmentRNG(seed);
    std::mt19937 portalRNG(seed);
    std::mt19937 fakePortalRNG(seed);
    std::mt19937 songRNG(seed);
    std::mt19937 bgRNG(seed);
    std::mt19937 speedRNG(seed);
    std::mt19937 spikeRNG(seed);
    std::mt19937 sizeRNG(seed);
    std::mt19937 roptRNG(seed);

    const bool optTeleportals = false;  // RIP. (10 September 2025) -M

    const bool optLowvis = mod->getSavedValue<bool>("opt-0-low-visibility");
    const bool optSpikes = mod->getSavedValue<bool>("opt-0-using-corridor-spikes");
    const bool optFakePortals = mod->getSavedValue<bool>("opt-0-fake-grav-portals");
    const bool optFuzz = mod->getSavedValue<bool>("opt-0-corridor-fuzz");
    bool gravity = mod->getSavedValue<bool>("opt-0-grav-portal-start");
    const bool cw = mod->getSavedValue<bool>("opt-0-widen-playfield-bounds");
    
    ColorMode optColorMode =
        static_cast<ColorMode>(mod->getSavedValue<uint8_t>("opt-0-color-mode"));
    if (optColorMode == ColorMode::Random) {
        optColorMode = static_cast<ColorMode>(bgRNG() % 4 + 1);
    }
    const PlacementBySize optSpikePlacement =
        static_cast<PlacementBySize>(mod->getSavedValue<uint8_t>("opt-0-spike-placement-types"));
    CorridorRules optCorridorRules = 
        static_cast<CorridorRules>(mod->getSavedValue<uint8_t>("opt-0-corridor-rules"));
    if (optCorridorRules == CorridorRules::Random) {
        optCorridorRules = static_cast<CorridorRules>(roptRNG() % 6 + 1);
    }
    const PortalInputs optPortalInputs =
        static_cast<PortalInputs>(mod->getSavedValue<uint8_t>("opt-0-portal-input-types"));
    const bool upo = mod->getSavedValue<bool>("opt-0-using-grav-portals");
    const Difficulties optPortals =
        static_cast<Difficulties>(upo ? mod->getSavedValue<uint8_t>("opt-0-grav-portals-diff") + 1 : 0);

    const bool ucs = mod->getSavedValue<bool>("opt-0-using-speed-changes");
    const Difficulties optChangingSpeed =
        static_cast<Difficulties>(ucs ? mod->getSavedValue<uint8_t>("opt-0-speed-changes-diff") + 1 : 0);
    const bool removeSpamPortals = mod->getSavedValue<bool>("opt-0-remove-portals-in-spams");
    const uint8_t optSpeedsRaw = mod->getSavedValue<uint8_t>("opt-0-speed-changes");
    std::map<SpeedChange, bool> optSpeedChanges = {
        {SpeedChange::Speed05x, static_cast<bool>(optSpeedsRaw & 1)},
        {SpeedChange::Speed1x,  static_cast<bool>(optSpeedsRaw & 2)},
        {SpeedChange::Speed2x,  static_cast<bool>(optSpeedsRaw & 4)},
        {SpeedChange::Speed3x,  static_cast<bool>(optSpeedsRaw & 8)},
        {SpeedChange::Speed4x,  static_cast<bool>(optSpeedsRaw & 16)}
    };
    const SpeedChange optSpeed =
        static_cast<SpeedChange>(mod->getSavedValue<uint8_t>("opt-0-starting-speed") + 1);

    const WaveSize optStartingSizeEnum =
        static_cast<WaveSize>(mod->getSavedValue<uint8_t>("opt-0-starting-size"));
    bool mini;
    if (optStartingSizeEnum == WaveSize::Random) {
        mini = static_cast<bool>(roptRNG() % 2);
    } else {
        mini = static_cast<bool>(optStartingSizeEnum);
    }
    bool changingSize = mod->getSavedValue<bool>("opt-0-using-size-changes");
    bool typeA = !(static_cast<bool>(mod->getSavedValue<uint8_t>("opt-0-size-transition-type")));

    uint8_t optCorridorHeight = mod->getSavedValue<uint8_t>("opt-0-corridor-height");

    uint32_t optLength = mod->getSavedValue<uint32_t>("opt-0-length");
    if (optLength < 1) optLength = 1;
    int y_swing = 0, cX = 345, cY = 135;
    int maxHeight = 255, minHeight = 45;

    if (mini) {
        cY -= 90;
        cX -= 30;
    }
    if (cw) {
        maxHeight += 30;
        minHeight -= 30;
    }

    std::vector<Segment> segments(optLength);
    LevelData levelData = {
        .name = "JFP Level",
        .seed = seed,
        .biomes = {
            {
                .x_initial = cX,
                .y_initial = cY,
                .type = JFPBiome::Juggernaut,
                .theme = "Classic",
                .song = 234565,
                .options = {
                    .length = static_cast<int>(optLength),
                    .corridorHeight = static_cast<int>(optCorridorHeight),
                    .startingMini = mini,
                    .maxHeight = maxHeight,
                    .minHeight = minHeight,
                    .visibility = optLowvis ? Visibility::Low : Visibility::Standard,
                    .startingGravity = gravity,
                    .startingSpeed = optSpeed,
                    .colorMode = optColorMode,
                    .bgColor = {28, 28, 28},
                    .lineColor = {255, 255, 255},
                    .typeA = typeA
                },
                .segments = segments
            }
        }
    };

    std::vector<int> antiZigzagMax = {1, -1, 1, -1, -1, 1, -1, 1, 1, -1};
    std::vector<int> antiZigzagMin = {-1, 1, -1, 1, 1, -1, 1, -1, -1, 1};
    std::vector<int> antiZigzagStd1 = {-1, 1, -1, 1, 1, -1, 1, -1, -1, 1, -1};
    std::vector<int> antiZigzagStd2 = {1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1};
    std::vector<int> antiSpam1 = {1, -1, 1, -1};
    std::vector<int> antiSpam2 = {-1, 1, -1, 1};
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
    int relMaxHeight = maxHeight - (optCorridorHeight);
    if (mini && typeA) relMaxHeight -= 30;
    SpeedChange currentSpeed = optSpeed;
    bool specialRules = false;
    bool lowRespectfulDensity = false;
    int8_t y_bias = -1;
    Portals currentPortal;

    MusicSources musicSource =
        static_cast<MusicSources>(mod->getSavedValue<uint8_t>("opt-0-music-source"));
    if (musicSource == MusicSources::LocalFiles) {
        auto songsList = getUserSongs();
        levelData.biomes[0].song = songsList[songRNG() % (songsList.size())];
    } else {
        levelData.biomes[0].song = jfpSoundtrack[songRNG() % (jfpSoundtrackSize)];
    }

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
    } else if (optColorMode == JFPGen::ColorMode::AllColors) {
        backgroundColor = {0, 0, 0};
        int excluded = bgRNG() % 3;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded) continue;
            backgroundColor[i] += bgRNG() % 256;
        }
    } else if (optColorMode == JFPGen::ColorMode::ClassicMode) {
        backgroundColor = {128, 128, 128};
    } else {
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

    uint8_t bgImage = Mod::get()->getSavedValue<uint8_t>("opt-0-background-texture");
    if (bgImage > 0) {
        kBank["kA6"] = geode::utils::numToString(bgImage);
    } else {
        kBank["kA6"] = geode::utils::numToString(bgRNG() % 59 + 1);
    }

    RawCR cr;
    if (optCorridorRules == CorridorRules::NoSpamNoZigzag) {
        cr.NZ = true;
        cr.NS = true;
    } else if (optCorridorRules == CorridorRules::NoSpam) {
        cr.NS = true;
    } else if (optCorridorRules == CorridorRules::Juggernaut) {
        cr.NS = true;
        cr.SPECIAL = true;
        cr.FD = true;
    } else if (optCorridorRules == CorridorRules::LRD) {
        cr.NS = true;
        cr.NZ = true;
        cr.LRD = true;
    } else if (optCorridorRules == CorridorRules::Limp) {
        cr.NS = true;
        cr.NZ = true;
        cr.LRD = true;
        cr.ND = true;
    }

    // Future prior block to generate lengths of different biomes

    for (int i = 0; i < optLength; i++) {
        cX += 30;
        y_swing = 0;
        currentPortal = Portals::None;

        if (cr.SPECIAL && (cY == minHeight+30 || cY == relMaxHeight-30) &&
            (i > 0 && (segments[i - 1].coords.second == minHeight ||
                segments[i - 1].coords.second == relMaxHeight))
        ) {
            specialRules = true;
        }

        if (i < 3 && !mini) {
            y_swing = corridorStart[i];
        } else if (i < 2 && mini) {
            y_swing = 1;
        } else if (last_tp <= 1 && i > 1) {
            y_swing = segments[i - 1].y_swing;
        } else if (
            cr.ND && segments[i - 2].y_swing != segments[i - 1].y_swing
        ) {
            if (segments[i - 1].y_swing == 1) y_swing = 1;
            else y_swing = -1;
        } else if (
            cY >= relMaxHeight && (segments[i - 1].y_swing == 1 ||
            (cr.NZ && orientationMatch(segments, i, antiZigzagMax)) ||
            (cr.NS &&orientationMatch(segments, i, {-1, 1, -1})))) {
            y_swing = -1;
        } else if (
            cY <= (mini ? minHeight + 30 : minHeight) &&
            (segments[i - 1].y_swing == -1 ||
            (cr.NZ && orientationMatch(segments, i, antiZigzagMin)) ||
            (cr.NS && orientationMatch(segments, i, {1, -1, 1})))) {
            y_swing = 1;
        } else if (
            cr.NZ && orientationMatch(segments, i, antiZigzagStd1)) {
            y_swing = -1;
        } else if (
            cr.NZ && orientationMatch(segments, i, antiZigzagStd2)) {
            y_swing = 1;
        } else if (cr.NS && (
            orientationMatch(segments, i, antiSpam1) &&
            (!cr.FD ||
                !(gravity && cY != relMaxHeight)))) {
            y_swing = -1;
        } else if (cr.NS && (
            orientationMatch(segments, i, antiSpam2) &&
            (!cr.FD ||
                !(!gravity && cY != minHeight)))) {
            y_swing = 1;
        } else {
            if (cr.LRD) {
                if (
                    orientationMatch(segments, i, {1, -1, 1}) ||
                    (cY <= (mini ? minHeight + 30 : minHeight) &&
                    segments[i - 1].y_swing == 1)
                ) {
                    lowRespectfulDensity = true;
                    y_bias = 1;
                } else if (
                    orientationMatch(segments, i, {-1, 1, -1}) ||
                    (cY >= relMaxHeight && segments[i - 1].y_swing == -1)
                ) {
                    lowRespectfulDensity = true;
                    y_bias = -1;
                } else {
                    lowRespectfulDensity = false;
                }
            }

            // special condition used in Juggernaut rules
            if (specialRules) {
                y_swing = segmentRNG() % 4;
                if (y_swing > 0) y_swing = 1;
                else y_swing = -1;
                if (cY >= relMaxHeight - 30) {
                    y_swing *= -1;
                }
            } else if (lowRespectfulDensity) {
                y_swing = segmentRNG() % 7;
                if (y_swing == 0) y_swing = y_bias * -1;
                else y_swing = y_bias;
            } else {
                // 50/50 corridor direction flip
                y_swing = segmentRNG() % 2;
                if (y_swing == 0) y_swing = -1;
            }
        }

        if (i > 0 && changingSize && lastSize > 2 &&
                ((typeA &&
                    !(!mini && y_swing == segments[i - 1].y_swing &&
                    (cY <= minHeight + 30 || cY >= relMaxHeight - 30))) ||
                (!typeA &&
                    !(mini && cw && y_swing != segments[i - 1].y_swing &&
                    (cY <= minHeight || cY >= relMaxHeight)))) &&
                sizeRNG() % std::max(10, 50 - lastSize) == 0) {
            mini = !mini;
            lastSize = 0;
            if (typeA) {
                segments[i - 1].options.isTransition = true;
                segments[i - 1].options.isSpikeM = false;
                spikeActive = false;
            }
        }

        if (i > 0 && typeA && mini && !segments[i - 1].options.mini) {
            relMaxHeight -= 30;
        } else if (i > 0 && typeA && !mini && segments[i - 1].options.mini) {
            relMaxHeight += 30;
        }

        // special type B transitional cases
        if (i > 0 && !typeA && segments[i - 1].options.mini != mini) {
            if (mini) {
                if (segments[i - 1].y_swing == -1) cY += 30;
            } else {
                if (segments[i - 1].y_swing == -1) cY -= 30;
            }
        }

        // size change y_swing catch - reorients the corridor if about
        // to get stuck. -M
        if (i > 0 && mini && !segments[i - 1].options.mini) {
            if (y_swing == 1 && cY >= relMaxHeight &&
                    orientationMatch(segments, i, {1, -1})) {
                y_swing = -1;
            } else if (y_swing == -1 && cY <= minHeight + 30 &&
                    orientationMatch(segments, i, {-1, 1})) {
                y_swing = 1;
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
        if (i > 0 && !mini && optPortals == Difficulties::Aggressive &&
                portalRNG() % 4 == 0) {
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
        if (i > 0 && !midCorridorPortal && segments[i - 1].y_swing != y_swing)
            midCorridorPortal = true;
        if (i > 0 && optPortals != Difficulties::None &&
                segments[i - 1].y_swing != y_swing) {
            portalOdds = portalRNG() % portalOddsMap.at(optPortals);

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
                // log::info("Portal odds: {} {} {}", portalOdds, cX, cY);
                gravity = portalOdds == 0 ? !gravity : gravity;
                currentPortal = (portalOdds == 0) ? Portals::Gravity : Portals::Fake;
            }
        }

        int speedOdds = 1;
        if (optChangingSpeed != Difficulties::None &&
            (
                (orientationMatch(segments, i, antiSpeedspam1) && y_swing == -1) ||
                (orientationMatch(segments, i, antiSpeedspam2) && y_swing == 1)) &&
            optSpeedsRaw != 0
        ) {
            speedOdds = speedRNG() % speedOddsMap.at(optChangingSpeed);
            if (speedOdds == 0) {
                double speedFactor = 0.5 * (optCorridorHeight / 60.0);
                int spY = cY + optCorridorHeight / 2 +
                    (optCorridorHeight / 4) * ((segments[i].y_swing == 1) ? -1 : 1);
                int spR = (segments[i].y_swing == 1) ? -45 : 45;
                SpeedChange newSpeed = currentSpeed;
                
                std::vector<SpeedChange> availableSpeeds;
                for (const auto& [speed, enabled] : optSpeedChanges) {
                    if (enabled && speed != currentSpeed) {
                        availableSpeeds.push_back(speed);
                    }
                }
                if (!availableSpeeds.empty()) {
                    newSpeed = availableSpeeds[speedRNG() % availableSpeeds.size()];
                }

                currentSpeed = newSpeed;
            }
        }

        if (i > 0 && optSpikes && segments[i - 1].y_swing != y_swing &&
                currentPortal == Portals::None) {
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

        if (
            (optSpikePlacement == PlacementBySize::Mini && !mini) ||
            (optSpikePlacement == PlacementBySize::Big && mini)
        ) {
            segments[i].options.isSpikeM = false;
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
                .isPortal = currentPortal,
                .speedChange = speedOdds == 0 ? currentSpeed : SpeedChange::None,
                .isFuzzy = segments[i].options.isFuzzy,
                .mini = mini,
                .isTransition = false
            }
        };
        // log::info(
        //     "Segment[{}]: coords=({}, {}), y_swing={}, gravity={}, isSpikeM={},
        // cornerPieces={}, isPortal={}, speedChange={}, isFakePortal={}, isFuzzy={}",
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

        if (removeSpamPortals &&
            segments[i - 1].options.isPortal == Portals::Gravity &&
            segments[i - 2].y_swing != segments[i - 1].y_swing &&
            y_swing != segments[i - 1].y_swing
        ) {
            gravity = !gravity;
            segments[i - 1].options.gravity = !segments[i - 1].options.gravity;
            segments[i].options.gravity = !segments[i].options.gravity;
            segments[i - 1].options.isPortal = Portals::None;
        }

        // Legacy teleportals generation code - optTeleportals is always false. -M
        if (optTeleportals && last_tp > 2 &&
                ((cY >= maxHeight && segments[i].y_swing == 1) ||
                (cY <= minHeight && segments[i].y_swing == -1)) &&
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
}

}  // namespace JFPGen
