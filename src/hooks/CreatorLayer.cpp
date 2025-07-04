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
#include "../ui/JFPMenuLayer.hpp"
#include "../utils/StringGen.hpp"

// bring used namespaces to scope
using namespace geode::prelude;

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
		auto jlTransition = CCTransitionFade::create(0.5, jfpLayer);
		CCDirector::sharedDirector()->pushScene(jlTransition);
	}

	static GJGameLevel* createGameLevel() {
		std::srand(std::time(0));
		std::string levelString = "<?xml version=\"1.0\"?><plist version=\"1.0\" gjver=\"2.0\"><dict><k>root</k>" + JFPGen::jfpStringGen(false) + "</dict></plist>";
		
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