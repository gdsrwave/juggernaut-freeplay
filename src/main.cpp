/**
 * Include the Geode headers.
 */
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

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;
using namespace gmd;

/**
 * `$modify` lets you extend and modify GD's classes.
 * To hook a function in Geode, simply $modify the class
 * and write a new function definition with the signature ofBASE_LEVEL = f'kS38,1_28_2_34_3_44_11_255_12_255_13_255_4_-1_6_1000_7_1_15_1_18_0_8_1|1_25_2_24_3_24_11_255_12_255_13_255_4_-1_6_1001_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1009_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1002_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1005_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1006_5_1_7_1_15_1_18_0_8_1|,kA13,0,kA15,0,kA16,0,kA14,,kA6,0,kA7,7,kA17,0,kA18,0,kS39,0,kA2,0,kA3,0,kA8,0,kA4,1,kA9,0,kA10,0,kA11,0;1,747,2,15,3,15,54,160;1,7,2,15,3,105,6,-90,21,1004;1,5,2,15,3,75,21,1004;1,5,2,15,3,45,21,1004;1,5,2,15,3,15,21,1004;1,7,2,45,3,105,6,-90,21,1004;1,5,2,45,3,15,21,1004;1,5,2,45,3,45,21,1004;1,1007,2,-15,3,285,20,1,36,1,51,2,10,1.57,35,0.5;1,5,2,45,3,75,21,1004;1,7,2,75,3,105,6,-90,21,1004;1,5,2,75,3,15,21,1004;1,5,2,75,3,75,21,1004;1,5,2,75,3,45,21,1004;1,7,2,195,3,15,21,1004;1,5,2,105,3,75,21,1004;1,5,2,105,3,15,21,1004;1,7,2,105,3,105,6,-90,21,1004;1,5,2,165,3,75;1,5,2,105,3,45,21,1004;1,103,2,165,3,129;1,5,2,135,3,75,21,1004;1,5,2,135,3,45,21,1004;1,5,2,135,3,15,21,1004;1,8,2,195,3,135;1,5,2,165,3,15,21,1004;1,5,2,165,3,45,21,1004;1,7,2,135,3,105,6,-90,21,1004;1,7,2,165,3,105,6,-90;1,1,2,195,3,105;1,7,2,195,3,75;1,7,2,195,3,45,21,1004;1,1338,2,255,3,45;1,660,2,255,3,163,6,17,13,0;1,{speed},2,255,3,165,13,1;1,1338,2,225,3,15;1,1338,2,285,3,{225+CORRIDOR_HEIGHT},6,270;1,1338,2,285,3,75;1,1338,2,345,3,{165+CORRIDOR_HEIGHT},6,270;1,1338,2,345,3,135;1,1338,2,375,3,{165+CORRIDOR_HEIGHT},6,180;1,1338,2,315,3,105;1,1338,2,375,3,165;1,1338,2,315,3,{195+CORRIDOR_HEIGHT},6,270;1,1338,2,405,3,195;1,1338,2,435,3,195,6,90;1,1338,2,405,3,{195+CORRIDOR_HEIGHT},6,180;1,1338,2,435,3,{195+CORRIDOR_HEIGHT},6,270;'
 * the function you want to hook.
 *
 * Here we use the overloaded `$modify` macro to set our own class name,
 * so that we can use it for button callbacks.
 *
 * Notice the header being included, you *must* include the header for
 * the class you are modifying, or you will get a compile error.
 */
#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(GenerateLevelLayer, LevelBrowserLayer) {
	/**
	 * Typically classes in GD are initialized using the `init` function, (though not always!),
	 * so here we use it to add our own button to the bottom menu.
	 *
	 * Note that for all hooks, your signature has to *match exactly*,
	 * `void init()` would not place a hook!
	*/
	bool init(GJSearchObject* s) {
		/**
		 * We call the original init function so that the
		 * original class is properly initialized.

		 todo
		 - gjsearchobject (ref gd share)
		 - test
		 */
		if (!LevelBrowserLayer::init(s)) {
			return false;
		}

		/**
		 * See this page for more info about buttons
		 * https://docs.geode-sdk.org/tutorials/buttons	
		*/
		if(s->m_searchType != SearchType::MyLevels) return true;
		auto genButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("waveman_s.png"_spr, .90f, CircleBaseColor::Cyan, CircleBaseSize::Medium),
			this,
			/**
			 * Here we use the name we set earlier for our modify class.
			*/
			menu_selector(GenerateLevelLayer::onGenButton)
		);

		/**
		 * Here we access the `bottom-menu` node by its ID, and add our button to it.
		 * Node IDs are a Geode feature, see this page for more info about it:
		 * https://docs.geode-sdk.org/tutorials/nodetree
		*/
		auto menu = this->getChildByID("my-levels-menu");
		menu->addChild(genButton);

		/**
		 * The `_spr` string literal operator just prefixes the string with
		 * your mod id followed by a slash. This is good practice for setting your own node ids.
		*/
		genButton->setID("generate-level-button"_spr);

		/**
		 * We update the layout of the menu to ensure that our button is properly placed.
		 * This is yet another Geode feature, see this page for more info about it:
		 * https://docs.geode-sdk.org/tutorials/layouts
		*/
		menu->updateLayout();

		/**
		 * We return `true` to indicate that the class was properly initialized.
		 */
		return true;
	}

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

	static std::string mainGen() {
		std::random_device rd;
		unsigned int seed = rd();
		std::mt19937_64 e2(seed);
		std::uniform_int_distribution<long long int> dtr(0, 1);
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
			167229
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
		const int64_t markinterval = Mod::get()->getSettingValue<int64_t>("marker-interval");
		const double corridorHeight = Mod::get()->getSettingValue<double>("corridor-height");
		const int maxHeight = 195; // 6.5 blocks (based on lower bound)
		const int minHeight = 45; // 1.5 blocks

		// T/F constants
		const bool marks = Mod::get()->getSettingValue<bool>("marks");
		const bool corner = Mod::get()->getSettingValue<bool>("corners");
		const bool zigzagLimit = Mod::get()->getSettingValue<bool>("zig-limit");
		const bool removeSpam = Mod::get()->getSettingValue<bool>("remove-spam");

		// Initialize the string, which contains the level base formatted with certain values from settings
		// This is very long and verbose, but I'm okay with how it works
		std::string level = fmt::format("kS38,1_28_2_34_3_44_11_255_12_255_13_255_4_-1_6_1000_7_1_15_1_18_0_8_1|1_25_2_24_3_24_11_255_12_255_13_255_4_-1_6_1001_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1009_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1002_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1005_5_1_7_1_15_1_18_0_8_1|1_255_2_75_3_0_11_255_12_255_13_255_4_-1_6_1006_5_1_7_1_15_1_18_0_8_1|,kA13,0,kA15,0,kA16,0,kA14,,kA6,0,kA7,7,kA17,0,kA18,0,kS39,0,kA2,0,kA3,0,kA8,0,kA4,1,kA9,0,kA10,0,kA11,0;1,747,2,15,3,15,54,160;1,7,2,15,3,105,6,-90,21,1004;1,5,2,15,3,75,21,1004;1,5,2,15,3,45,21,1004;1,5,2,15,3,15,21,1004;1,7,2,45,3,105,6,-90,21,1004;1,5,2,45,3,15,21,1004;1,5,2,45,3,45,21,1004;1,1007,2,-15,3,285,20,1,36,1,51,2,10,1.57,35,0.5;1,5,2,45,3,75,21,1004;1,7,2,75,3,105,6,-90,21,1004;1,5,2,75,3,15,21,1004;1,5,2,75,3,75,21,1004;1,5,2,75,3,45,21,1004;1,7,2,195,3,15,21,1004;1,5,2,105,3,75,21,1004;1,5,2,105,3,15,21,1004;1,7,2,105,3,105,6,-90,21,1004;1,5,2,165,3,75;1,5,2,105,3,45,21,1004;1,103,2,165,3,129;1,5,2,135,3,75,21,1004;1,5,2,135,3,45,21,1004;1,5,2,135,3,15,21,1004;1,8,2,195,3,135;1,5,2,165,3,15,21,1004;1,5,2,165,3,45,21,1004;1,7,2,135,3,105,6,-90,21,1004;1,7,2,165,3,105,6,-90;1,1,2,195,3,105;1,7,2,195,3,75;1,7,2,195,3,45,21,1004;1,1338,2,255,3,45;1,660,2,255,3,163,6,17,13,0;1,{speedID},2,255,3,165,13,1;1,1338,2,225,3,15;1,1338,2,285,3,{ch_1},6,270;1,1338,2,285,3,75;1,1338,2,345,3,{ch_2},6,270;1,1338,2,345,3,135;1,1338,2,375,3,{ch_2},6,180;1,1338,2,315,3,105;1,1338,2,375,3,165;1,1338,2,315,3,{ch_3},6,270;1,1338,2,405,3,195;1,1338,2,435,3,195,6,90;1,1338,2,405,3,{ch_3},6,180;1,1338,2,435,3,{ch_3},6,270;",
		fmt::arg("speedID", speedID),
		fmt::arg("ch_1", 225+corridorHeight),
		fmt::arg("ch_2", 165+corridorHeight),
		fmt::arg("ch_3", 195+corridorHeight));
		
		log::info("Seed {}", seed);

		
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

		for(int i = 0; i < length; i++) {
			// for each loop, reset the current y_swing (might be unnecessary) and increment x by 1 block/30 units
			x += 30;
			y_swing = 0;

			if (y >= maxHeight && (prevO[10] == 1 || (zigzagLimit && orientationMatch(prevO, antiZigzagMax)))) {
				y_swing = -1;
			} else if(y <= minHeight && (prevO[10] == -1 || (zigzagLimit && orientationMatch(prevO, antiZigzagMin)))) {
				y_swing = 1;
			} else {
				if(zigzagLimit && orientationMatch(prevO, antiZigzagStd1)) {
					y_swing = -1;
				} else if (zigzagLimit && orientationMatch(prevO, antiZigzagStd2)) {
					y_swing = 1;
				} else if(removeSpam && orientationMatch(prevO, antiSpam1)) {
					y_swing = -1;
				} else if (removeSpam && orientationMatch(prevO, antiSpam2)) {
					y_swing = 1;
				} else {
					// randomized coinflip condition
					y_swing = dtr(e2);
					if(y_swing == 0) y_swing = -1;
				}
			}

			if(prevO[10] == y_swing) y += (y_swing * 30);

			orientationShift(prevO, y_swing);
			
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

			std::string corners = "";

			level += (genBuildF + genBuildC);
		}

		//log::info("{}", level);

		// deflate usage source: https://github.com/HJfod/GMD-API/blob/main/src/GMD.cpp
		/*unsigned char* gz = nullptr;
		int memEnd = ZipUtils::ccDeflateMemory(reinterpret_cast<unsigned char*>(level.data()), level.size(), &gz);
		if(!memEnd) return FLAlertLayer::create("Export Error", "Level data compression failed! Please try restarting, updating JFP or reporting the issue.", "Sure thing...")->show();
		auto levelBV = ByteVector(reinterpret_cast<uint8_t*>(gz), reinterpret_cast<uint8_t(gz + memEnd));
		if(gz) delete*/
		std::string b64 = ZipUtils::compressString(level, true, 0);
		std::string desc = fmt::format("Seed: {}", seed);
		desc = ZipUtils::base64URLEncode(ZipUtils::base64URLEncode(desc)); // double encoding might be unnecessary according to gmd-api source?
		b64.erase(std::find(b64.begin(), b64.end(), '\0'), b64.end());
		desc.erase(std::find(desc.begin(), desc.end(), '\0'), desc.end());

		std::string levelString = fmt::format("<d><k>kCEK</k><i>4</i><k>k2</k><s>JFP {title}</s><k>k3</k><s>{desc}</s><k>k4</k><s>{b64}</s><k>k45</k><i>614361</i><k>k13</k><t/><k>k21</k><i>2</i><k>k50</k><i>35</i></d>",
		fmt::arg("desc", desc),
		fmt::arg("b64", b64),
		fmt::arg("title", std::to_string(seed).substr(0, 6)));

		log::info("{}", levelString);

		return levelString;

		// todo: add top and bottom slopes from this segment to the level string
		// todo: add optional corner piece blocks
		// todo: top/bottom ending connectors - requires coordinates and brief loops for each
		// todo: meter mark generation
		// todo: remove any unused code left from example, add comments, debug

	}

	static void exportLvlStringGMD(ghc::filesystem::path const& path, std::string ld1) {
		auto lvlData = ByteVector(ld1.begin(), ld1.end());
		file::writeBinary(path, lvlData);
	}


	/**
	 * This is the callback function for the button we created earlier.
	 * The signature for button callbacks must always be the same,
	 * return type `void` and taking a `CCObject*`.
	*/
	void onGenButton(CCObject*) {
		// Special thanks to HJFod for their work on GDShare and GMD-API, which was used as reference here
		// https://github.com/HJfod

		// create fileutils object and get GMD file from game data path
		// getWritablePath() = %LOCALAPPDATA%\GeometryDash
		std::srand(std::time(0));
		auto localPath = CCFileUtils::sharedFileUtils();
		auto levelString = mainGen();
		exportLvlStringGMD(std::string(localPath->getWritablePath()) + "/waveman.gmd", levelString);
		auto jfpImport = ImportGmdFile::from(std::string(localPath->getWritablePath()) + "/waveman.gmd");

		// infer filetype, should always be .gmd for us
		jfpImport.tryInferType(); // .gmd
		// convert to GJGameLevel
		auto jfpResult = jfpImport.intoLevel();
		if(!jfpResult) return FLAlertLayer::create("Import Error", jfpResult.error(), "Sure thing...")->show();

		// Insert level object into local list
		LocalLevelManager::get()->m_localLevels->insertObject(jfpResult.value(), 0);

		// create new scene
		auto newScene = CCScene::create();
		auto newLayer = LevelBrowserLayer::create(GJSearchObject::create(SearchType::MyLevels));
		newScene->addChild(newLayer);
		CCDirector::sharedDirector()->replaceScene(newScene);

	}
};
