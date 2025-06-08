#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include <random>
#include <cmath>
#include <fmt/core.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>

// bring used namespaces to scope
using namespace geode::prelude;
using namespace gmd;

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

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(GenerateLevelLayer, LevelBrowserLayer) {

	bool init(GJSearchObject* s) {

		if (!LevelBrowserLayer::init(s)) {
			return false;
		}
		
		if(s->m_searchType != SearchType::MyLevels) return true;
		auto genButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("waveman_s.png"_spr, .90f, CircleBaseColor::Cyan, CircleBaseSize::Medium),
			this,
			menu_selector(GenerateLevelLayer::onGenButton)
		);
		auto autoGenButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("dabbink_s.png"_spr, 1.125f, CircleBaseColor::DarkAqua, CircleBaseSize::Medium),
			this,
			menu_selector(GenerateLevelLayer::onAutoGenButton)
		);

		auto menu = this->getChildByID("my-levels-menu");
		menu->addChild(genButton);
		menu->addChild(autoGenButton);
		genButton->setID("generate-level-button"_spr);
		autoGenButton->setID("auto-generate-level-button"_spr);

		menu->updateLayout();

		return true;
	}

	static std::string mainGen(bool compress) {

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
		std::mt19937 songRNG(seed);

		// current soundtrack
		const int soundtrack[] = {
			614361,
			569208,
			678606,
			592012,
			109650,
			595394,
			85046,
			112527,
			169589,
			32207,
			64842,
			306254,
			386900,
			167229,
			895761,
			620959,
			431573,
			623104,
			349494,
			91656,
			574484,
			575665,
			467267,
			575341,
			490203,
			510853,
			835420,
			741034,
			830628,
			436814,
			485978,
			505813,
			621166,
			813526,
			723722,
			591740,
			638150,
			557651,
			286138,
			581444,
			223469,
			621144,
			739358,
			495840,
			516709,
			234565,
			590533,
			76743,
			378965,
			691180,
			465909,
			117474,
			459080,
			513917,
			622111,
			564162,
			498828,
			530913,
			489111,
			1126156,
			383158,
			648035,
			32772,
			656428,
			469490,
			579560,
			472169,
			675129,
			586990,
			561277,
			114114,
			347865,
			718319,
			718171,
			469968,
			771738,
			666666,
			829777,
			739991,
			905110,
			709578,
			219635,
			764913,
			163061,
			333082,
			659843,
			323384,
			278217,
			645631,
			258437,
			98933,
			465681,
			31214,
			99641,
			478283,
			36355,
			797578,
			614354,
			28549,
			353593,
			141262,
			343707,
			778652,
			393342,
			137492,
			685686,
			616845,
			63082,
			233860,
			598349,
			437224
		};

		const std::string filename = Mod::get()->getSettingValue<std::string>("filename");

		// numeric constants
		const std::string speed = Mod::get()->getSettingValue<std::string>("speed");
		int speedID = convertSpeed(speed);
		int speedFloat = convertSpeedToFloat(speed);
		const int64_t length = Mod::get()->getSettingValue<int64_t>("length");
		const int64_t markInterval = Mod::get()->getSettingValue<int64_t>("marker-interval");

		// bg-color option
		const std::string colorModeStr = Mod::get()->getSettingValue<std::string>("color-mode");
		int colorMode = 0;
		if (colorModeStr == "Classic Mode") colorMode = 1;
		else if (colorModeStr == "All Colors") colorMode = 2;
		else if (colorModeStr == "Night Mode") colorMode = 3;
		std::mt19937 bg_rng(seed);
		std::uniform_int_distribution<int> dist256(0, 255);
		std::uniform_int_distribution<int> dist3(0, 2);
		std::uniform_int_distribution<int> dist17(0, 16);

		std::array<int, 3> background_color = {28, 28, 28};
		std::string line_color;
		if (colorMode == 3) {
			background_color = {0, 0, 0};
			int excluded = dist3(bg_rng);
			int maxed = dist3(bg_rng);
			if (maxed >= excluded) maxed += 1;
			if (maxed > 2) maxed = 2;

			std::array<int, 3> object_color = {255, 255, 255};
			object_color[excluded] = 0;
			object_color[maxed] = 255;
			for (int i = 0; i < 3; ++i) {
				if (i == excluded || i == maxed) continue;
				object_color[i] += dist256(bg_rng);
				if (object_color[i] > 255) object_color[i] = 255;
			}
			line_color = fmt::format("1_{}_2_{}_3_{}_11_255_12_255_13_255_4_-1_6_1004_7_1_15_1_18_0_8_1|",
				object_color[0], object_color[1], object_color[2]);
		}
		else if (colorMode == 2) {
			background_color = {0, 0, 0};
			int excluded = dist3(bg_rng);
			for (int i = 0; i < 3; ++i) {
				if (i == excluded) continue;
				background_color[i] += dist256(bg_rng);
				if (background_color[i] > 255) background_color[i] = 255;
			}
		}
		else if (colorMode == 1) {
			background_color = {128, 128, 128};
		}
		else {
			background_color = {28, 28, 28};
			int excluded = dist3(bg_rng);
			for (int i = 0; i < 3; ++i) {
				if (i == excluded) continue;
				background_color[i] += dist17(bg_rng);
				if (background_color[i] > 255) background_color[i] = 255;
			}
		}

		const double corridorHeight = Mod::get()->getSettingValue<double>("corridor-height");
		const int maxHeight = 195; // 6.5 blocks (based on lower bound)
		const int minHeight = 45; // 1.5 blocks

		// T/F constants
		const bool marks = Mod::get()->getSettingValue<bool>("marks");
		const bool cornerPieces = Mod::get()->getSettingValue<bool>("corners");
		const bool zigzagLimit = Mod::get()->getSettingValue<bool>("zig-limit");
		const bool removeSpam = Mod::get()->getSettingValue<bool>("remove-spam");
		const bool lowvis = Mod::get()->getSettingValue<bool>("low-vis");

		const bool debug = Mod::get()->getSettingValue<bool>("debug");
		const bool portals = Mod::get()->getSettingValue<bool>("portals");
		const bool teleportals = Mod::get()->getSettingValue<bool>("teleportals");
		
		const bool corridorSpikes = Mod::get()->getSettingValue<bool>("corridor-spikes");
		const bool fuzzySpikes = Mod::get()->getSettingValue<bool>("fuzzy-spikes");

		const bool changingSpeed = Mod::get()->getSettingValue<bool>("changing-speed");
		const std::string minSpeed = Mod::get()->getSettingValue<std::string>("min-speed");
		const std::string maxSpeed = Mod::get()->getSettingValue<std::string>("max-speed");
		const int minSpeedFloat = convertSpeedToFloat(minSpeed);
		const int maxSpeedFloat = convertSpeedToFloat(maxSpeed);


		// Initialize the string, which contains the level base formatted with certain values from settings
		// This is very long and verbose, but I'm okay with how it works
		std::string level = fmt::format("kS38,1_{bg_0}_2_{bg_1}_3_{bg_2}_11_255_12_255_13_255_4_-1_6_1000_7_1_15_1_18_0_8_1|1_25_2_24_3_24_11_255_12_255_13_255_4_-1_6_1001_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1009_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1002_5_1_7_1_15_1_18_0_8_1|{line_color}1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1005_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1006_5_1_7_1_15_1_18_0_8_1|1_0_2_0_3_0_11_255_12_255_13_255_4_-1_6_3_7_1_15_1_18_0_8_1|,kA13,0,kA15,0,kA16,0,kA14,,kA6,0,kA7,7,kA17,0,kA18,0,kS39,0,kA2,0,kA3,0,kA8,0,kA4,1,kA9,0,kA10,0,kA11,0;1,747,2,15,3,15,54,160,116,1;1,7,2,15,3,105,6,-90,21,1004;1,5,2,15,3,75,21,1004;1,5,2,15,3,45,21,1004;1,5,2,15,3,15,21,1004;1,7,2,45,3,105,6,-90,21,1004;1,5,2,45,3,15,21,1004;1,5,2,45,3,45,21,1004;1,1007,2,-15,3,285,20,1,36,1,51,2,10,1.57,35,0.5;1,1007,2,-15,3,315,20,1,36,1,51,3,10,0.0,35,0;1,5,2,45,3,75,21,1004;1,7,2,75,3,105,6,-90,21,1004;1,5,2,75,3,15,21,1004;1,5,2,75,3,75,21,1004;1,5,2,75,3,45,21,1004;1,7,2,195,3,15,21,1004;1,5,2,105,3,75,21,1004;1,5,2,105,3,15,21,1004;1,7,2,105,3,105,6,-90,21,1004;1,5,2,165,3,75;1,5,2,105,3,45,21,1004;1,103,2,165,3,129;1,5,2,135,3,75,21,1004;1,5,2,135,3,45,21,1004;1,5,2,135,3,15,21,1004;1,8,2,195,3,135;1,5,2,165,3,15,21,1004;1,5,2,165,3,45,21,1004;1,7,2,135,3,105,6,-90,21,1004;1,7,2,165,3,105,6,-90;1,1,2,195,3,105;1,7,2,195,3,75;1,7,2,195,3,45,21,1004;1,1338,2,255,3,45;1,660,2,255,3,163,6,17,13,0;1,{speedID},2,255,3,165,13,1;1,1338,2,225,3,15;1,1338,2,285,3,{ch_1},6,270;1,1338,2,285,3,75;1,1338,2,345,3,{ch_2},6,270;1,1338,2,345,3,135;1,1338,2,375,3,{ch_2},6,180;1,1338,2,315,3,105;1,1338,2,375,3,165;1,1338,2,315,3,{ch_3},6,270;1,1338,2,405,3,195;1,1338,2,435,3,195,6,90;1,1338,2,405,3,{ch_3},6,180;1,1338,2,435,3,{ch_3},6,270;",
			fmt::arg("speedID", speedID),
			fmt::arg("ch_1", 225+corridorHeight),
			fmt::arg("ch_2", 165+corridorHeight),
			fmt::arg("ch_3", 195+corridorHeight),
			fmt::arg("bg_0", background_color[0]),
			fmt::arg("bg_1", background_color[1]),
			fmt::arg("bg_2", background_color[2]),
			fmt::arg("line_color", line_color)
		);
		
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

		bool gravity = false; // true == upside down

		int portalOdds = 1;

		static bool spikeActive = false;
		static bool spikeSideHold = false;
		static int spikeSide = 0;
		int spikeOdds = 0;
		static float current_speed = speedFloat;
		int last_tp = 0;
		int tp_count = 0;
		std::array<int, 2> tp_stored = {0, 0};

		for(int i = 0; i < length; i++) {
			// for each loop, reset the current y_swing (might be unnecessary) and increment x by 1 block/30 units
			x += 30;
			y_swing = 0;

			if (tp_stored[0] != 0 || tp_stored[1] != 0) {
				if (prevO[10] == 1) {
					y_swing = 1;
					if (tp_count == 3) {
						x = tp_stored[0] - 90;
						y = tp_stored[1] - 120;
					}
				} else if (prevO[10] == -1) {
					y_swing = -1;
					if (tp_count == 4) {
						x = tp_stored[0] - 60;
						y = tp_stored[1] + 90;
					}
				}
				if (tp_count == 7) {
					tp_count = -1;
					tp_stored = {0, 0};
				}
				tp_count += 1;
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
			genBuildF += ";";
			
			std::string genBuildC = fmt::format("1,1338,2,{x},3,{y}", fmt::arg("x", x), fmt::arg("y", y+corridorHeight));
			if(y_swing > 0) {
				genBuildC += ",6,180";
			} else {
				genBuildC += ",6,270";
			}
			genBuildC += ";";

			std::string cornerBuild = "";
			if(cornerPieces) {
				if(prevO[10] == 1 && y_swing == -1) {
					cornerBuild = fmt::format("1,473,2,{cnr1x},3,{cnry},6,-180;1,473,2,{cnr2x},3,{cnry},6,-90;",
					fmt::arg("cnr1x", x-30),
					fmt::arg("cnr2x", x),
					fmt::arg("cnry", y+corridorHeight+30));
				} else if(prevO[10] == -1 && y_swing == 1) {
					cornerBuild = fmt::format("1,473,2,{cnr1x},3,{cnry},6,90;1,473,2,{cnr2x},3,{cnry};",
					fmt::arg("cnr1x", x-30),
					fmt::arg("cnr2x", x),
					fmt::arg("cnry", y+30));
				}
			}

			std::string portalBuild = "";
			if(portals && prevO[10] != y_swing) {
				portalOdds = portalRNG() % 10;
				if(portalOdds == 0) {
					double portalFactor = ((double)corridorHeight / 60.0) * 1.414;
					// no float precision right now. not sure if it will matter, especially for what people are actually playing.
					int portalNormal = corridorHeight / 10;
					int portalPos = corridorHeight / 4;
					int portalID = gravity ? 10 : 11;
					gravity = gravity ? false : true;
					portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rPdeg},32,{scale};",
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
				std::string spikeBuild = fmt::format("1,103,2,{xS},3,{yS},6,{rS};",
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
						"1,{fuzzId},2,{x},3,{y}{colorMod};1,1717,2,{x},3,{yC},6,180{colorMod};",
						fmt::arg("fuzzId", fuzzId),
						fmt::arg("x", x),
						fmt::arg("y", y),
						fmt::arg("yC", y + corridorHeight),
						fmt::arg("colorMod", colorMod)
					);
				} else {
					// Downwards
					fuzzPieces = fmt::format(
						"1,{fuzzId},2,{x},3,{y},6,90{colorMod};1,1717,2,{x},3,{yC},6,-90{colorMod};",
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
			if (changingSpeed && ((prevO[8] == 1 && prevO[9] == -1 && prevO[10] == -1) || (prevO[8] == -1 && prevO[9] == 1 && prevO[10] == 1)) && maxSpeedFloat > minSpeedFloat) {
				int speedOdds = portalRNG() % 10; // You can adjust the odds as needed
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
					std::string speedBuild = fmt::format("1,{speed},2,{x},3,{y},6,{r},32,{factor};",
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
			if (teleportals && tp_count == 0 &&
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
					level += fmt::format("1,747,2,{x},3,{y1},6,{r},32,{factor},54,{y2};",
						fmt::arg("x", tpX),
						fmt::arg("y1", tpY1),
						fmt::arg("r", tpR),
						fmt::arg("factor", tpFactor),
						fmt::arg("y2", tpY2)
					);
					tp_stored = {x, (y <= minHeight) ? (maxHeight + 30) : (minHeight - 30)};
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
			level += fmt::format("1,1338,2,{x},3,{y},6,180;", fmt::arg("x", xT), fmt::arg("y", yT));
		}
		while(yB >= (minHeight)) {
			xB += 30;
			yB -= 30;
			level += fmt::format("1,1338,2,{x},3,{y},6,90;", fmt::arg("x", xB), fmt::arg("y", yB));
		}

		// Meter Mark Generation
		if (marks && markInterval > 0) {
			int meters = markInterval;
			double markHeight;
			for(int j = 0; j < (length / markInterval); j++) {
				markHeight = 15.5;

				for(int i = 0; i < 10; i++) {
					level += fmt::format("1,508,2,{dist},3,{markHeight},20,1,57,2,6,-90;", fmt::arg("dist", 435+meters*30), fmt::arg("markHeight", markHeight));
					markHeight += 30.0;
				}

				std::string meterLabel = ZipUtils::base64URLEncode(fmt::format("{}m", meters));
				level += fmt::format("1,914,2,{dist},3,21,20,1,57,2,32,0.62,31,{meterLabel};", fmt::arg("dist", 391+meters*30), fmt::arg("meterLabel", meterLabel));
				meters += markInterval;
			}
		}

		if(lowvis) {
			std::string lvBuild = "1,901,2,315,3,285,20,2,36,1,51,3,28,0,29,0,10,1000,30,0,85,2,58,1;1,1011,2,495,3,150,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,210,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,30,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,90,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,1011,2,495,3,270,20,2,57,3,64,1,67,1,6,-90,21,3,24,9,32,2;1,211,2,600,3,225,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,600,3,75,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,1007,2,645,3,315,20,2,36,1,51,3,10,1.01,35,1;1,211,2,750,3,75,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,750,3,225,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,900,3,225,20,2,57,3,64,1,67,1,21,3,24,9,32,5;1,211,2,900,3,75,20,2,57,3,64,1,67,1,21,3,24,9,32,5;";
			level += lvBuild;
		}

		if(debug) log::info("{}", level);

		int songSelection = soundtrack[(songRNG() % (sizeof(soundtrack)/sizeof(int)))];
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
		fmt::arg("title", std::to_string(seed).substr(0, 6)),
		fmt::arg("song", songSelection));

		if(debug) log::info("{}", levelString);

		return levelString;

		// todo: remove any unused code left from example, add comments, debug

	}

	// waveman button callback -- return type here must be void and parameter must be CCObject*
	void onGenButton(CCObject*) {
		// Special thanks to HJFod for their work on GDShare and GMD-API, which was used as reference here
		// https://github.com/HJfod

		// create fileutils object and get GMD file from game data path
		// getWritablePath() = %LOCALAPPDATA%\GeometryDash
		std::srand(std::time(0));
		auto localPath = CCFileUtils::sharedFileUtils();
		std::string levelString = mainGen(true);
		if(levelString.empty()) return;
		exportLvlStringGMD(std::string(localPath->getWritablePath()) + "/waveman.gmd", levelString);
		auto jfpImport = ImportGmdFile::from(std::string(localPath->getWritablePath()) + "/waveman.gmd");

		// infer filetype, should always be .gmd for us
		jfpImport.tryInferType(); // .gmd
		// convert to GJGameLevel
		auto jfpResult = jfpImport.intoLevel();
		if(jfpResult.isErr()) return FLAlertLayer::create("Import Error", jfpResult.unwrapErr(), "Sure thing...")->show();

		// Insert level object into local list
		LocalLevelManager::get()->m_localLevels->insertObject(jfpResult.unwrap(), 0);

		// create new scene
		auto newScene = CCScene::create();
		auto newLayer = LevelBrowserLayer::create(GJSearchObject::create(SearchType::MyLevels));
		newScene->addChild(newLayer);
		CCDirector::sharedDirector()->replaceScene(newScene);

	}

	static GJGameLevel* createGameLevel() {
		std::srand(std::time(0));
		std::string levelString = "<?xml version=\"1.0\"?><plist version=\"1.0\" gjver=\"2.0\"><dict><k>root</k>" + mainGen(false) + "</dict></plist>";
		
		// somewhat copied from gmd-api's source code dont sue please
		std::unique_ptr<DS_Dictionary> dict = std::make_unique<DS_Dictionary>();
		if (!dict.get()->loadRootSubDictFromString(levelString)) {
			return nullptr;
    	}
		dict->stepIntoSubDictWithKey("root");

		GJGameLevel* level = GJGameLevel::create();
		level->dataLoaded(dict.get());
		level->m_levelType = GJLevelType::Editor;
		return level;
	}

	void onAutoGenButton(CCObject*) {
		state = AutoJFP::JustStarted;
		auto level = createGameLevel();
		if (!level) {
			state = AutoJFP::NotInAutoJFP;
			return FLAlertLayer::create("Error", "Could not generate level", "Okay buddy...")->show();
		}
		auto newScene = PlayLayer::scene(level, false, false);
		CCDirector::sharedDirector()->replaceScene(newScene); // seems to work better than pushScene?
	}
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	void onQuit() {
		state = AutoJFP::NotInAutoJFP;
		PlayLayer::onQuit();
	}

	void resetLevel() {
		if (state == AutoJFP::NotInAutoJFP) return PlayLayer::resetLevel();
		else if (state == AutoJFP::JustStarted) {
			state = AutoJFP::PlayingLevelAtt1;
			return PlayLayer::resetLevel();
		}
		else if (state == AutoJFP::JustRestarted) {
			this->m_unk3089 = false; // this var controls whether the camera follows the player at the start
			return PlayLayer::resetLevel();
		}

		auto pl = PlayLayer::get();
		int atts = pl->m_attempts;
		int best = pl->m_level->m_normalPercent;

		auto dir = CCDirector::sharedDirector();
		auto scene = CCScene::create();
		auto size = dir->getWinSize();

		// take screenshot, use this as filler during the 1 frame of transition between playlayers
		// without this there would be an annoying black flash for 1 frame
		auto rt = CCRenderTexture::create(size.width, size.height);
		rt->setPosition(size / 2); // middle of screen
		rt->begin();
		dir->getRunningScene()->visit();
		rt->end();

		scene->addChild(rt);
		dir->replaceScene(scene);
		
		// the original playlayer isnt destroyed until later in the frame when u call replacescene so we have to queue this for the next frame
		queueInMainThread([=]() {
			auto level = GenerateLevelLayer::createGameLevel();
			if (!level) return; // idk what to do here

			state = AutoJFP::JustRestarted;
			
			// important note: resetLevel gets called somewhere within PlayLayer::scene()
			// so its important that the state is JustRestarted by this point
			auto scene = PlayLayer::scene(level, false, false);
			dir->replaceScene(scene);
			
			auto pl = PlayLayer::get();
			pl->startGame(); // gotta call this instantly to prevent the attempt 1 delay
			pl->m_attempts = atts;
			pl->updateAttempts();
			pl->m_level->m_normalPercent = best;
				
			state = AutoJFP::PlayingLevel;

			// necessary to prevent cursor flashing on respawn except it sometimes doesnt work idk
			if (!GameManager::get()->getGameVariable("0024")) PlatformToolbox::hideCursor(); 

			// necessary for compat with mh restart key for some reason
			queueInMainThread([]() {
				if (!GameManager::get()->getGameVariable("0024")) PlatformToolbox::hideCursor(); 
			});
		});
	}

	void startGame() {
		if (state == AutoJFP::PlayingLevel) return; // prevents lag from this func being called twice when restarting
		PlayLayer::startGame();
	}
};