#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include <random>
#include <cmath>
#include <fmt/core.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include "utils/constants.hpp"
#include "utils/StringGen.hpp"
#include "ui/JFPMenuLayer.hpp"

// bring used namespaces to scope
using namespace geode::prelude;
using namespace gmd;



#include <Geode/modify/CreatorLayer.hpp>
class $modify(GenerateLevelLayer, CreatorLayer) {

	bool init() {

		if (!CreatorLayer::init()) {
			return false;
		}
		auto optionButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("lmao_s.png"_spr, .80f, CircleBaseColor::DarkAqua, CircleBaseSize::Medium),
			this,
			menu_selector(GenerateLevelLayer::onJFPButton)
		);

		auto menu = this->getChildByID("bottom-left-menu");
		menu->addChild(optionButton);
		optionButton->setID("jfp-launch-button"_spr);

		menu->updateLayout();

		return true;
	}

	void onJFPButton(CCObject*) {
		//openSettingsPopup(Mod::get());
		auto jfpLayer = JFPMenuLayer::scene();
		CCDirector::sharedDirector()->pushScene(jfpLayer);
	}

	static GJGameLevel* createGameLevel() {
		std::srand(std::time(0));
		std::string levelString = "<?xml version=\"1.0\"?><plist version=\"1.0\" gjver=\"2.0\"><dict><k>root</k>" + JFPGen::jfpMainStringGen(false, state) + "</dict></plist>";
		
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
};

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(LBGenerateLevelLayer, LevelBrowserLayer) {

	bool init(GJSearchObject* s) {

		if (!LevelBrowserLayer::init(s)) {
			return false;
		}
		
		if(s->m_searchType != SearchType::MyLevels) return true;
		auto genButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("waveman_s.png"_spr, .90f, CircleBaseColor::Cyan, CircleBaseSize::Medium),
			this,
			menu_selector(LBGenerateLevelLayer::onGenButton)
		);

		auto menu = this->getChildByID("my-levels-menu");
		menu->addChild(genButton);
		genButton->setID("generate-level-button"_spr);

		menu->updateLayout();

		return true;
	}
	
	// waveman button callback -- return type here must be void and parameter must be CCObject*
	void onGenButton(CCObject*) {
		// Special thanks to HJFod for their work on GDShare and GMD-API, which was used as reference here
		// https://github.com/HJfod

		// create fileutils object and get GMD file from game data path
		// getWritablePath() = %LOCALAPPDATA%\GeometryDash
		std::srand(std::time(0));
		auto localPath = CCFileUtils::sharedFileUtils();
		std::string levelString = JFPGen::jfpMainStringGen(true, state);
		if(levelString.empty()) return;
		JFPGen::exportLvlStringGMD(std::string(localPath->getWritablePath()) + "/waveman.gmd", levelString);
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
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	void onQuit() {
		state = JFPGen::AutoJFP::NotInAutoJFP;
		PlayLayer::onQuit();
	}

	void resetLevel() {
		if (state == JFPGen::AutoJFP::NotInAutoJFP) return PlayLayer::resetLevel();
		else if (state == JFPGen::AutoJFP::JustStarted) {
			state = JFPGen::AutoJFP::PlayingLevelAtt1;
			return PlayLayer::resetLevel();
		}
		else if (state == JFPGen::AutoJFP::JustRestarted) {
			this->m_unk3229 = false; // this var controls whether the camera follows the player at the start
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

			state = JFPGen::AutoJFP::JustRestarted;
			
			// important note: resetLevel gets called somewhere within PlayLayer::scene()
			// so its important that the state is JustRestarted by this point
			auto scene = PlayLayer::scene(level, false, false);
			dir->replaceScene(scene);
			
			auto pl = PlayLayer::get();
			pl->startGame(); // gotta call this instantly to prevent the attempt 1 delay
			pl->m_attempts = atts;
			pl->updateAttempts();
			pl->m_level->m_normalPercent = best;
				
			state = JFPGen::AutoJFP::PlayingLevel;

			// necessary to prevent cursor flashing on respawn except it sometimes doesnt work idk
			if (!GameManager::get()->getGameVariable("0024")) PlatformToolbox::hideCursor(); 

			// necessary for compat with mh restart key for some reason
			queueInMainThread([]() {
				if (!GameManager::get()->getGameVariable("0024")) PlatformToolbox::hideCursor(); 
			});
		});
	}

	void startGame() {
		if (state == JFPGen::AutoJFP::PlayingLevel) return; // prevents lag from this func being called twice when restarting
		PlayLayer::startGame();
	}
};