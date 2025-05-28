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
			895761
		};

		const std::string filename = Mod::get()->getSettingValue<std::string>("filename");

		// numeric constants
		const double speed = Mod::get()->getSettingValue<double>("speed");
		int speedID = 203;
		if(speed == 0.5) {
			speedID = 200;
		} else if(speed == 1.0) {
			speedID = 201;
		} else if(speed == 2.0) {
			speedID = 202;
		} else if(speed == 4.0) {
			speedID = 1334;
		}
		const int64_t length = Mod::get()->getSettingValue<int64_t>("length");
		const int64_t markInterval = Mod::get()->getSettingValue<int64_t>("marker-interval");
		
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

		// Initialize the string, which contains the level base formatted with certain values from settings
		// This is very long and verbose, but I'm okay with how it works
		std::string level = fmt::format("kS38,1_28_2_34_3_44_11_255_12_255_13_255_4_-1_6_1000_7_1_15_1_18_0_8_1|1_25_2_24_3_24_11_255_12_255_13_255_4_-1_6_1001_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1009_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1002_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1005_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1006_5_1_7_1_15_1_18_0_8_1|1_0_2_0_3_0_11_255_12_255_13_255_4_-1_6_3_7_1_15_1_18_0_8_1|,kA13,0,kA15,0,kA16,0,kA14,,kA6,0,kA7,7,kA17,0,kA18,0,kS39,0,kA2,0,kA3,0,kA8,0,kA4,1,kA9,0,kA10,0,kA11,0;1,747,2,15,3,15,54,160;1,7,2,15,3,105,6,-90,21,1004;1,5,2,15,3,75,21,1004;1,5,2,15,3,45,21,1004;1,5,2,15,3,15,21,1004;1,7,2,45,3,105,6,-90,21,1004;1,5,2,45,3,15,21,1004;1,5,2,45,3,45,21,1004;1,1007,2,-15,3,285,20,1,36,1,51,2,10,1.57,35,0.5;1,1007,2,-15,3,315,20,1,36,1,51,3,10,0.0,35,0;1,5,2,45,3,75,21,1004;1,7,2,75,3,105,6,-90,21,1004;1,5,2,75,3,15,21,1004;1,5,2,75,3,75,21,1004;1,5,2,75,3,45,21,1004;1,7,2,195,3,15,21,1004;1,5,2,105,3,75,21,1004;1,5,2,105,3,15,21,1004;1,7,2,105,3,105,6,-90,21,1004;1,5,2,165,3,75;1,5,2,105,3,45,21,1004;1,103,2,165,3,129;1,5,2,135,3,75,21,1004;1,5,2,135,3,45,21,1004;1,5,2,135,3,15,21,1004;1,8,2,195,3,135;1,5,2,165,3,15,21,1004;1,5,2,165,3,45,21,1004;1,7,2,135,3,105,6,-90,21,1004;1,7,2,165,3,105,6,-90;1,1,2,195,3,105;1,7,2,195,3,75;1,7,2,195,3,45,21,1004;1,1338,2,255,3,45;1,660,2,255,3,163,6,17,13,0;1,{speedID},2,255,3,165,13,1;1,1338,2,225,3,15;1,1338,2,285,3,{ch_1},6,270;1,1338,2,285,3,75;1,1338,2,345,3,{ch_2},6,270;1,1338,2,345,3,135;1,1338,2,375,3,{ch_2},6,180;1,1338,2,315,3,105;1,1338,2,375,3,165;1,1338,2,315,3,{ch_3},6,270;1,1338,2,405,3,195;1,1338,2,435,3,195,6,90;1,1338,2,405,3,{ch_3},6,180;1,1338,2,435,3,{ch_3},6,270;",
		fmt::arg("speedID", speedID),
		fmt::arg("ch_1", 225+corridorHeight),
		fmt::arg("ch_2", 165+corridorHeight),
		fmt::arg("ch_3", 195+corridorHeight));
		
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

		bool gravity = false; // true == upside down

		int portalOdds = 1;

		for(int i = 0; i < length; i++) {
			// for each loop, reset the current y_swing (might be unnecessary) and increment x by 1 block/30 units
			x += 30;
			y_swing = 0;

			if (y >= maxHeight && (prevO[10] == 1 || (zigzagLimit && removeSpam && orientationMatch(prevO, antiZigzagMax)))) {
				y_swing = -1;
			} else if(y <= minHeight && (prevO[10] == -1 || (zigzagLimit && removeSpam && orientationMatch(prevO, antiZigzagMin)))) {
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

			orientationShift(prevO, y_swing);

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

		int atts = this->m_attempts;
		state = AutoJFP::JustRestarted;
		
		auto level = GenerateLevelLayer::createGameLevel();
		if (!level) return; // idk what to do here
		
		// important note: resetLevel gets called somewhere within PlayLayer::scene()
		// so its important that the state is JustRestarted by this point
		auto dir = CCDirector::sharedDirector();
		dir->popScene(); // gotta do this before creating the new playlayer to fix restart hotkey + cursor visibility
		auto scene = PlayLayer::scene(level, false, false);
		dir->pushScene(scene);
		
		auto pl = PlayLayer::get();
		pl->startGame(); // gotta call this instantly to prevent the attempt 1 delay
		pl->m_attempts = atts;
		pl->updateAttempts();
		
		state = AutoJFP::PlayingLevel;
	}

	void startGame() {
		if (state == AutoJFP::PlayingLevel) return; // prevents lag from this func being called twice when restarting
		PlayLayer::startGame();
	}
};