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
#include "constants.cpp"

using namespace geode::prelude;
using namespace gmd;

namespace JFPGen {

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

// WIP: ninja format
struct Segment {
	std::pair<int, int> coords;
	int y_swing;
	// Add more attributes as needed
};

struct Biome {
	int x_initial;
	std::string type;
	std::string theme;
	std::string options;
	std::vector<Segment> segments;
	// Add more attributes as needed
};

struct LevelData {
	std::string name;
	std::vector<Biome> biomes;
};

// checks if a certain orientation pattern matches the most recent previous orientations
// essentially this checks if the end of one int [] equals another int []
static bool orientationMatch(int prevO[11], const std::vector<int> pattern) {
	if(pattern.size()>11) return false;
	for(int i = 0; i < pattern.size(); i++) {
		if(pattern[i] != prevO[i+(11-pattern.size())]) {
			return false;
		}
	}
	return true;
}

static void orientationShift(int prevO[11], int newO) {
	for(int i = 0; i < 10; i++) {
		prevO[i] = prevO[i+1];
	}
	prevO[10] = newO;
}

static void exportLvlStringGMD(std::filesystem::path const& path, std::string ld1) {
	auto lvlData = ByteVector(ld1.begin(), ld1.end());
	(void) file::writeBinary(path, lvlData);
}

static int convertSpeed(const std::string& speed) {
	if (speed == "0.5x") return 200;
	else if (speed == "1x") return 201;
	else if (speed == "2x") return 202;
	else if (speed == "4x") return 1334;
	return 203; // default speed
}

static float convertSpeedToFloat(const std::string& speed) {
	if (speed == "0.5x") return 0.5f;
	else if (speed == "1x") return 1.0f;
	else if (speed == "2x") return 2.0f;
	else if (speed == "4x") return 4.0f;
	return 3.0f; // default speed
}

static int convertFloatSpeed(float speed) {
	if (speed == 0.5f) return 200;
	else if (speed == 1.0f) return 201;
	else if (speed == 2.0f) return 202;
	else if (speed == 4.0f) return 1334;
	return 203; // default speed
}

enum class AutoJFP : int {
	NotInAutoJFP = 0,
	JustStarted = 1,
	JustRestarted = 2,
	PlayingLevelAtt1 = 3,
	PlayingLevel = 4,
};

AutoJFP state = AutoJFP::NotInAutoJFP;

static std::string jfpMainStringGen(bool compress, AutoJFP state = AutoJFP::NotInAutoJFP) {

    // random device setups - used with modulo to generate numbers in a range
    std::random_device rd;
    unsigned int seed = 0;
    try {
        std::string seedStr = Mod::get()->getSettingValue<std::string>("seed");
        if(!seedStr.empty() && state == AutoJFP::NotInAutoJFP) seed = std::stoul(seedStr);
    } catch(const std::exception &e) {
        FLAlertLayer::create("Error", e.what(), "Sure thing...")->show();
        return "";
    }
    
    if(seed == 0) seed = rd();
    std::mt19937 segmentRNG(seed);
    std::mt19937 portalRNG(seed);
    std::mt19937 fakePortalRNG(seed);
    std::mt19937 songRNG(seed);
    std::mt19937 bgRNG(seed);

    const std::string filename = Mod::get()->getSettingValue<std::string>("filename");

    // numeric constants
    const std::string speed = Mod::get()->getSettingValue<std::string>("speed");
    int speedID = convertSpeed(speed);
    int speedFloat = convertSpeedToFloat(speed);
    const int64_t length = Mod::get()->getSettingValue<int64_t>("length");
    const int64_t markInterval = Mod::get()->getSettingValue<int64_t>("marker-interval");

    // Example LevelData with 1 biome and 1 segment
    std::vector<Segment> segments(length);
    LevelData levelData = {
        .name = "JFP Level",
        .biomes = {
            {
                .x_initial = 435,
                .type = "Default",
                .theme = "Classic",
                .options = "",
                .segments = segments
            }
        }
    };

    // bg-color option
    const std::string colorModeStr = Mod::get()->getSettingValue<std::string>("color-mode");
    int colorMode = 0;
    if (colorModeStr == "Classic Mode") colorMode = 1;
    else if (colorModeStr == "All Colors") colorMode = 2;
    else if (colorModeStr == "Night Mode") colorMode = 3;

    std::array<int, 3> background_color = {28, 28, 28};
    std::string line_color;
    if (colorMode == 3) {
        background_color = {0, 0, 0};
        int excluded = bgRNG() % 3;
        int maxed = bgRNG() % 3;
        if (excluded == maxed && excluded == 2) maxed = 0;
        else if (maxed == excluded) maxed += 1;

        std::array<int, 3> object_color = {255, 255, 255};
        object_color[excluded] = 0;
        object_color[maxed] = 255;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded || i == maxed) continue;
            object_color[i] -= bgRNG() % 256;
        }
        line_color = fmt::format("1_{}_2_{}_3_{}_11_255_12_255_13_255_4_-1_6_1004_7_1_15_1_18_0_8_1|",
            object_color[0], object_color[1], object_color[2]);
    }
    else if (colorMode == 2) {
        background_color = {0, 0, 0};
        int excluded = bgRNG() % 3;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded) continue;
            background_color[i] += bgRNG() % 256;
        }
    }
    else if (colorMode == 1) {
        background_color = {128, 128, 128};
    }
    else {
        background_color = {28, 28, 28};
        int excluded = bgRNG() % 3;
        for (int i = 0; i < 3; ++i) {
            if (i == excluded) continue;
            background_color[i] += bgRNG() % 17;
            if (background_color[i] > 255) background_color[i] = 255;
        }
    }

    const double corridorHeight = Mod::get()->getSettingValue<double>("corridor-height");
    const int maxHeight = 195; // 6.5 blocks (based on lower bound)
    const int minHeight = 45; // 1.5 blocks

    // T/F constants
    const bool marks = Mod::get()->getSettingValue<bool>("marks");
    const bool cornerPieces = Mod::get()->getSettingValue<bool>("corners");
    const std::string corridorRule = Mod::get()->getSettingValue<std::string>("corridor-rules");
    const bool lowvis = Mod::get()->getSettingValue<bool>("low-vis");

    const bool debug = Mod::get()->getSettingValue<bool>("debug");
    const std::string portals = Mod::get()->getSettingValue<std::string>("portals");
    const bool teleportals = Mod::get()->getSettingValue<bool>("teleportals");
    
    const bool corridorSpikes = Mod::get()->getSettingValue<bool>("corridor-spikes");
    const bool fuzzySpikes = Mod::get()->getSettingValue<bool>("fuzzy-spikes");

    const std::string changingSpeed = Mod::get()->getSettingValue<std::string>("changing-speed");
    const std::string minSpeed = Mod::get()->getSettingValue<std::string>("min-speed");
    const std::string maxSpeed = Mod::get()->getSettingValue<std::string>("max-speed");
    const int minSpeedFloat = convertSpeedToFloat(minSpeed);
    const int maxSpeedFloat = convertSpeedToFloat(maxSpeed);

    const bool fakeGravityPortals = Mod::get()->getSettingValue<bool>("fake-gravity-portals");
    const bool upsideStart = Mod::get()->getSettingValue<bool>("upside-start");

    const bool removeSpam = corridorRule == "No Spam" || corridorRule == "No Spam, No Zigzagging";
    const bool zigzagLimit = corridorRule == "No Spam, No Zigzagging";

    // Initialize the string, which contains the level base formatted with certain values from settings
    // This is very long and verbose, but I'm okay with how it works
    std::string level = fmt::format("kS38,1_{bg_0}_2_{bg_1}_3_{bg_2}_11_255_12_255_13_255_4_-1_6_1000_7_1_15_1_18_0_8_1|1_25_2_24_3_24_11_255_12_255_13_255_4_-1_6_1001_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1009_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1002_5_1_7_1_15_1_18_0_8_1|{line_color}1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1005_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1006_5_1_7_1_15_1_18_0_8_1|1_0_2_0_3_0_11_255_12_255_13_255_4_-1_6_3_7_1_15_1_18_0_8_1|,kA13,0,kA15,0,kA16,0,kA14,,kA6,0,kA7,7,kA17,0,kA18,0,kS39,0,kA2,0,kA3,0,kA8,0,kA4,1,kA9,0,kA10,0,kA11,0;1,747,2,15,3,15,54,160,116,1;1,7,2,15,3,105,6,-90,21,1004,64,1,67,1;1,5,2,15,3,75,21,1004,64,1,67,1;1,5,2,15,3,45,21,1004,64,1,67,1;1,5,2,15,3,15,21,1004,64,1,67,1;1,7,2,45,3,105,6,-90,21,1004,64,1,67,1;1,5,2,45,3,15,21,1004,64,1,67,1;1,5,2,45,3,45,21,1004,64,1,67,1;1,1007,2,-15,3,285,20,1,36,1,51,2,10,1.57,35,0.5,64,1,67,1;1,1007,2,-15,3,315,20,1,36,1,51,3,10,0.0,35,0,64,1,67,1;1,5,2,45,3,75,21,1004,64,1,67,1;1,7,2,75,3,105,6,-90,21,1004,64,1,67,1;1,5,2,75,3,15,21,1004,64,1,67,1;1,5,2,75,3,75,21,1004,64,1,67,1;1,5,2,75,3,45,21,1004,64,1,67,1;1,7,2,195,3,15,21,1004,64,1,67,1;1,5,2,105,3,75,21,1004,64,1,67,1;1,5,2,105,3,15,21,1004,64,1,67,1;1,7,2,105,3,105,6,-90,21,1004,64,1,67,1;1,5,2,165,3,75;1,5,2,105,3,45,21,1004,64,1,67,1;1,103,2,165,3,129,64,1,67,1;1,5,2,135,3,75,21,1004,64,1,67,1;1,5,2,135,3,45,21,1004,64,1,67,1;1,5,2,135,3,15,21,1004,64,1,67,1;1,8,2,195,3,135,64,1,67,1;1,5,2,165,3,15,21,1004,64,1,67,1;1,5,2,165,3,45,21,1004,64,1,67,1;1,7,2,135,3,105,6,-90,21,1004,64,1,67,1;1,7,2,165,3,105,6,-90,64,1,67,1;1,1,2,195,3,105,64,1,67,1;1,7,2,195,3,75,64,1,67,1;1,7,2,195,3,45,21,1004,64,1,67,1;1,1338,2,255,3,45,64,1,67,1;1,660,2,255,3,163,6,17,13,0,64,1,67,1;1,{speedID},2,255,3,165,13,1,64,1,67,1;1,1338,2,225,3,15,64,1,67,1;1,1338,2,285,3,{ch_1},6,270;1,1338,2,285,3,75,64,1,67,1;1,1338,2,345,3,{ch_2},6,270,64,1,67,1;1,1338,2,345,3,135,64,1,67,1;1,1338,2,375,3,{ch_2},6,180,64,1,67,1;1,1338,2,315,3,105,64,1,67,1;1,1338,2,375,3,165,64,1,67,1;1,1338,2,315,3,{ch_3},6,270,64,1,67,1;1,1338,2,405,3,195,64,1,67,1;1,1338,2,435,3,195,6,90,64,1,67,1;1,1338,2,405,3,{ch_3},6,180,64,1,67,1;1,1338,2,435,3,{ch_3},6,270,64,1,67,1;1,1338,2,255,3,{ch_4},6,270,64,1,67,1;1,1338,2,225,3,{ch_5},6,270,64,1,67,1;",
        fmt::arg("speedID", speedID),
        fmt::arg("ch_1", 225+corridorHeight),
        fmt::arg("ch_2", 165+corridorHeight),
        fmt::arg("ch_3", 195+corridorHeight),
        fmt::arg("ch_4", 255+corridorHeight),
        fmt::arg("ch_5", 285+corridorHeight),
        fmt::arg("bg_0", background_color[0]),
        fmt::arg("bg_1", background_color[1]),
        fmt::arg("bg_2", background_color[2]),
        fmt::arg("line_color", line_color)
    );

    if (upsideStart) {
        level += "1,11,2,293,3,105,6,45,32,0.57;";
    }
    
    log::info("Seed: {}", seed);

    // y_swing = the direction the wave corridor is currently moving - can be 0, 1, -1, and possibly -2/2 for miniwave in the future
    // prevO stands for Previous Orientations. Stores previous swings
    // the name prevO is a remnant from the Python version, where y_swing was stored numerically as is now but orientations were stored in the +- str format e.g. "+-++-+-+--+"
    int y_swing = 0, x = 435, y = 195;
    
    int prevO[11] = {0,0,0,0,0,0,0,1,1,1,-1};

    // constant patterns for anti-zigzagging
    // Max/Min are used because, since there's a bound above/below, the pattern needs to be redirected earlier to prevent a forced extra zigzag
    // (a repeating zigzag pattern looks like this, and we're trying to prevent these from repeating in the level
    //     +-  +-        +
    //    +  -+  -  +-  + 
    //   +        -+  -+
    std::vector<int> antiZigzagMax = {1,-1,1,-1,-1,1,-1,1,1,-1};
    std::vector<int> antiZigzagMin = {-1,1,-1,1,1,-1,1,-1,-1,1};
    std::vector<int> antiZigzagStd1 = {-1,1,-1,1,1,-1,1,-1,-1,1,-1};
    std::vector<int> antiZigzagStd2 = {1,-1,1,-1,-1,1,-1,1,1,-1,1};

    // todo:
    // orientationmatch doesnt work need to use vectors instead for dynamic length

    std::vector<int> antiSpam1 = {1,-1,1,-1};
    std::vector<int> antiSpam2 = {-1,1,-1,1};

    std::vector<int> antiTpspam1 = {-1, -1};
    std::vector<int> antiTpspam2 = {1, 1};

    bool gravity = upsideStart; // true == upside down

    int portalOdds = 1;
    int fakePortalOdds = 1;

    static bool spikeActive = false;
    static bool spikeSideHold = false;
    static int spikeSide = 0;
    int spikeOdds = 0;
    static float current_speed = speedFloat;
    int last_tp = 0;

    for(int i = 0; i < length-1; i++) {
        // for each loop, reset the current y_swing (might be unnecessary) and increment x by 1 block/30 units
        x += 30;
        y_swing = 0;

        if (last_tp <= 1) {
            y_swing = prevO[10];
        } else if (y >= maxHeight && (prevO[10] == 1 || (zigzagLimit && removeSpam && orientationMatch(prevO, antiZigzagMax)) || orientationMatch(prevO, antiTpspam1))) {
            y_swing = -1;
        } else if(y <= minHeight && (prevO[10] == -1 || (zigzagLimit && removeSpam && orientationMatch(prevO, antiZigzagMin)) || orientationMatch(prevO, antiTpspam2))) {
            y_swing = 1;
        } else {
            if(zigzagLimit && removeSpam && orientationMatch(prevO, antiZigzagStd1)) {
                y_swing = -1;
            } else if (zigzagLimit && removeSpam && orientationMatch(prevO, antiZigzagStd2)) {
                y_swing = 1;
            } else if(removeSpam && orientationMatch(prevO, antiSpam1)) {
                y_swing = -1;
            } else if (removeSpam && orientationMatch(prevO, antiSpam2)) {
                y_swing = 1;
            } else {
                // randomized coinflip condition
                y_swing = segmentRNG() % 2;
                if(y_swing == 0) y_swing = -1;
            }
        }

        if(prevO[10] == y_swing) y += (y_swing * 30);
        
        // blocks from this segment: F = floor, C = ceiling
        std::string genBuildF = fmt::format("1,1338,2,{x},3,{y}", fmt::arg("x", x), fmt::arg("y", y));
        if(y_swing < 0) genBuildF += ",6,90";
        genBuildF += ",64,1,67,1;";

        levelData.biomes[0].segments[i] = Segment{
            .coords = std::make_pair(x, y),
            .y_swing = y_swing
        };
        
        std::string genBuildC = fmt::format("1,1338,2,{x},3,{y}", fmt::arg("x", x), fmt::arg("y", y+corridorHeight));
        if(y_swing > 0) {
            genBuildC += ",6,180";
        } else {
            genBuildC += ",6,270";
        }
        genBuildC += ",64,1,67,1;";

        std::string cornerBuild = "";
        if(cornerPieces) {
            if(prevO[10] == 1 && y_swing == -1) {
                cornerBuild = fmt::format("1,473,2,{cnr1x},3,{cnry},6,-180;1,473,2,{cnr2x},3,{cnry},6,-90,64,1,67,1;",
                fmt::arg("cnr1x", x-30),
                fmt::arg("cnr2x", x),
                fmt::arg("cnry", y+corridorHeight+30));
            } else if(prevO[10] == -1 && y_swing == 1) {
                cornerBuild = fmt::format("1,473,2,{cnr1x},3,{cnry},6,90;1,473,2,{cnr2x},3,{cnry},64,1,67,1;",
                fmt::arg("cnr1x", x-30),
                fmt::arg("cnr2x", x),
                fmt::arg("cnry", y+30));
            }
        }

        std::string portalBuild = "";
        if(portals != "None" && prevO[10] != y_swing) {
            portalOdds = portalRNG() % portalOddsMap[portals];

            fakePortalOdds = fakeGravityPortals ? (fakePortalRNG() % 10) : 1;

            if(portalOdds == 0 || fakePortalOdds == 0) {
                log::info("Portal odds: {} {}", portalOdds, fakePortalOdds);
                double portalFactor = ((double)corridorHeight / 60.0) * 1.414;
                // no float precision right now. not sure if it will matter, especially for what people are actually playing.
                int portalNormal = corridorHeight / 10;
                int portalPos = corridorHeight / 4;
                int portalID = portalOdds == 0 ? (gravity ? 10 : 11) : (gravity ? 11 : 10); // real portal hit? : real portal : fake portal
                gravity = portalOdds == 0 ? (gravity ? false : true) : gravity;
                portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rPdeg},32,{scale},64,1,67,1;",
                fmt::arg("portalID", portalID),
                fmt::arg("xP", x-15-portalNormal+portalPos),
                fmt::arg("yP", y+(y_swing == 1 ? portalNormal+portalPos-15 : corridorHeight+15-portalNormal-portalPos)),
                fmt::arg("rPdeg", (y_swing == 1 ? 45 : -45)),
                fmt::arg("scale", portalFactor / 2.5));
            }
        }

        // SPIKE GENERATION
        if (corridorSpikes && prevO[10] != y_swing) {
            spikeSideHold = false;
            spikeOdds = segmentRNG() % 6;
            if (spikeOdds == 0) {
                if (spikeActive) spikeSideHold = true;
                spikeActive = true;
                if (!spikeSideHold) spikeSide = segmentRNG() % 2;
            } else {
                spikeActive = false;
            }
        }

        if (spikeActive) {
            // x should be -6 from the current block if top side + spike down, or bottom side + spike up
            // y should be -6 from the current block if 
            int xS = x - 6;
            if (spikeSide == 0) {
                xS = x + 6;
            }
            int yS = y + 6;
            if ((y_swing == 1 && spikeSide == 0) || (y_swing == -1 && spikeSide == 1)) {
                yS = y + corridorHeight - 6;
            }
            int rS = 45;
            if (y_swing == 1 && spikeSide == 0) {
                rS = 135;
            } else if (y_swing == 1 && spikeSide == 1) {
                rS = -45;
            } else if (y_swing == -1 && spikeSide == 1) {
                rS = -135;
            }
            std::string spikeBuild = fmt::format("1,103,2,{xS},3,{yS},6,{rS},64,1,67,1;",
                fmt::arg("xS", xS),
                fmt::arg("yS", yS),
                fmt::arg("rS", rS)
            );
            level += spikeBuild;
        }

        std::string fuzzPieces = "";
        if (fuzzySpikes) {
            std::string colorMod = (colorMode == 3) ? ",21,1004" : "";
            std::string fuzzId = "1717"; // Only big wave supported in this code
            if (y_swing == 1) {
                // Upwards
                fuzzPieces = fmt::format(
                    "1,{fuzzId},2,{x},3,{y}{colorMod};1,1717,2,{x},3,{yC},6,180{colorMod},64,1,67,1;",
                    fmt::arg("fuzzId", fuzzId),
                    fmt::arg("x", x),
                    fmt::arg("y", y),
                    fmt::arg("yC", y + corridorHeight),
                    fmt::arg("colorMod", colorMod)
                );
            } else {
                // Downwards
                fuzzPieces = fmt::format(
                    "1,{fuzzId},2,{x},3,{y},6,90{colorMod};1,1717,2,{x},3,{yC},6,-90{colorMod},64,1,67,1;",
                    fmt::arg("fuzzId", fuzzId),
                    fmt::arg("x", x),
                    fmt::arg("y", y),
                    fmt::arg("yC", y + corridorHeight),
                    fmt::arg("colorMod", colorMod)
                );
            }
        }
        level += fuzzPieces;

        orientationShift(prevO, y_swing);

        // SPEED CHANGE GENERATION
        if (changingSpeed != "None" && ((prevO[8] == 1 && prevO[9] == -1 && prevO[10] == -1) || (prevO[8] == -1 && prevO[9] == 1 && prevO[10] == 1)) && maxSpeedFloat > minSpeedFloat) {
            int speedOdds = portalRNG() % speedOddsMap[changingSpeed]; // You can adjust the odds as needed
            if (speedOdds == 0) {
                double speedFactor = 0.5 * (corridorHeight / 60.0);
                int spY = y + corridorHeight / 2 + (corridorHeight / 4) * ((prevO[10] == 1) ? -1 : 1);
                int spR = (prevO[10] == 1) ? -45 : 45;
                float new_speed = current_speed;
                int minSpeedMod = (minSpeedFloat == 0.5) ? 0 : minSpeedFloat;

                // Pick a new speed different from current_speed
                for (int tries = 0; tries < 10 && new_speed == current_speed; ++tries) {
                    if (maxSpeedFloat == 0.5) break;
                    new_speed = minSpeedMod + (songRNG() % (maxSpeedFloat - minSpeedMod + 1));
                    if (new_speed == 0) new_speed = 0.5;
                }
                current_speed = new_speed;
                std::string speedBuild = fmt::format("1,{speed},2,{x},3,{y},6,{r},32,{factor},64,1,67,1;",
                    fmt::arg("speed", convertFloatSpeed(current_speed)),
                    fmt::arg("x", x - 15),
                    fmt::arg("y", spY),
                    fmt::arg("r", spR),
                    fmt::arg("factor", speedFactor)
                );
                level += speedBuild;
            }
        }

        // TELEPORTAL GENERATION
        if (teleportals && last_tp > 2 &&
            ((y >= maxHeight && prevO[10] == 1) || (y <= minHeight && prevO[10] == -1)) &&
            x + 120 < 435 + (length - 1) * 30) {
            int portalOdds = portalRNG() % ((last_tp < 40) ? (50 - last_tp) : 10);
            if (portalOdds == 0) {
                double tpFactor = 0.5 * (corridorHeight / 60.0);
                last_tp = 0;
                int tpX = x + corridorHeight / 4;
                int tpY1 = (y_swing == 1) ? (y + 3 * corridorHeight / 4) : (y + corridorHeight / 4);
                int tpY2 = (y_swing == 1)
                    ? (minHeight + 2 * corridorHeight / 3 - 30 - tpY1)
                    : (maxHeight + 30 + corridorHeight / 3 - tpY1);
                int tpR = (y_swing == 1) ? -45 : 45;

                // add teleportal to level
                level += fmt::format("1,747,2,{x},3,{y1},6,{r},32,{factor},54,{y2},64,1,67,1;",
                    fmt::arg("x", tpX),
                    fmt::arg("y1", tpY1),
                    fmt::arg("r", tpR),
                    fmt::arg("factor", tpFactor),
                    fmt::arg("y2", tpY2)
                );

                // Generate corridor end connectors
                std::string tpConnector = "";
                int r1 = (y_swing == 1) ? 0 : 90;
                int r2 = (y_swing == 1) ? 180 : 270;
                for (int j = 1; j <= 3; ++j) {
                    int xj = x + 30 * j;
                    int yj = y + 30 * j * y_swing;
                    int yj2 = yj + 60;
                    tpConnector += fmt::format("1,1338,2,{x},3,{y},6,{r1},64,1,67,1;1,1338,2,{x},3,{y2},6,{r2},64,1,67,1;",
                        fmt::arg("x", xj),
                        fmt::arg("y", yj),
                        fmt::arg("y2", yj2),
                        fmt::arg("r1", r1),
                        fmt::arg("r2", r2)
                    );
                }

                int yFuture = (y <= minHeight) ? (maxHeight + 30) : (minHeight - 30);
                for (int j = 0; j <= 2; ++j) {
                    int xj = x + 30 * j * -1;
                    int yj = yFuture + 30 * j * y_swing * -1;
                    int yj2 = yj + 60;
                    tpConnector += fmt::format("1,1338,2,{x},3,{y},6,{r1},64,1,67,1;1,1338,2,{x},3,{y2},6,{r2},64,1,67,1;",
                        fmt::arg("x", xj),
                        fmt::arg("y", yj),
                        fmt::arg("y2", yj2),
                        fmt::arg("r1", r1),
                        fmt::arg("r2", r2)
                    );
                }

                // add teleportal corridor ends
                level += tpConnector;

                // store the teleportal coordinates for later use
                y = yFuture;
            }
        }
        if (last_tp < 40) last_tp += 1;


        level += (genBuildF + genBuildC + cornerBuild + portalBuild);
    }

    // Ending Connectors
    int xB = x, yB = y, xT = x, yT = y + corridorHeight;
    if(prevO[10] == 1) {
        yB += 30;
    } else {
        yT -= 30;
    }
    while(yT <= (maxHeight + corridorHeight + 30)) {
        xT += 30;
        yT += 30;
        level += fmt::format("1,1338,2,{x},3,{y},6,180,64,1,67,1;", fmt::arg("x", xT), fmt::arg("y", yT));
    }
    while(yB >= (minHeight)) {
        xB += 30;
        yB -= 30;
        level += fmt::format("1,1338,2,{x},3,{y},6,90,64,1,67,1;", fmt::arg("x", xB), fmt::arg("y", yB));
    }

    // Meter Mark Generation
    std::string metermarksStr = "";
    std::string currentMark;
    if (marks && markInterval > 0) {
        int meters = markInterval;
        double markHeight;
        for(int j = 0; j < (length / markInterval); j++) {
            currentMark = "";
            markHeight = 15.5;

            for(int i = 0; i < 10; i++) {
                currentMark += fmt::format("1,508,2,{dist},3,{markHeight},20,1,57,2,6,-90,64,1,67,1;", fmt::arg("dist", 435+meters*30), fmt::arg("markHeight", markHeight));
                markHeight += 30.0;
            }

            std::string meterLabel = ZipUtils::base64URLEncode(fmt::format("{}m", meters));
            meterLabel.erase(std::find(meterLabel.begin(), meterLabel.end(), '\0'), meterLabel.end());
            currentMark += fmt::format("1,914,2,{dist},3,21,20,1,57,2,32,0.62,31,{meterLabel};", fmt::arg("dist", 391+meters*30), fmt::arg("meterLabel", meterLabel));
            meters += markInterval;
            metermarksStr += currentMark;
        }
    }
    level += metermarksStr;

    if(lowvis) {
        std::string lvBuild = "1,901,2,315,3,285,20,2,36,1,51,3,28,0,29,0,10,1000,30,0,85,2,58,1;1,1011,2,495,3,150,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,210,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,30,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,90,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,270,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,211,2,600,3,225,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,600,3,75,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,1007,2,645,3,315,20,2,36,1,51,3,10,1.01,35,1;1,211,2,750,3,75,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,750,3,225,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,900,3,225,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,900,3,75,20,2,57,3,64,1,67,1,21,3,24,9,32,5;";
        level += lvBuild;
    }

    if(debug) log::info("{}", level);

    int songSelection = JFP::soundtrack[(songRNG() % (sizeof(JFP::soundtrack)/sizeof(int)))];
    std::string b64;
    if (compress) b64 = ZipUtils::compressString(level, true, 0);
    else b64 = level;
    std::string desc = fmt::format("Seed: {}", seed);
    desc = ZipUtils::base64URLEncode(ZipUtils::base64URLEncode(desc)); // double encoding might be unnecessary according to gmd-api source?
    b64.erase(std::find(b64.begin(), b64.end(), '\0'), b64.end());
    desc.erase(std::find(desc.begin(), desc.end(), '\0'), desc.end());

    std::string levelString = fmt::format("<k>kCEK</k><i>4</i><k>k2</k><s>JFP {title}</s><k>k3</k><s>{desc}</s><k>k4</k><s>{b64}</s><k>k45</k><i>{song}</i><k>k13</k><t/><k>k21</k><i>2</i><k>k50</k><i>35</i>",
    fmt::arg("desc", desc),
    fmt::arg("b64", b64),
    fmt::arg("title", std::to_string(seed)),
    fmt::arg("song", songSelection));

    if(debug) log::info("{}", levelString);

    return levelString;

    // todo: remove any unused code left from example, add comments, debug

}

}