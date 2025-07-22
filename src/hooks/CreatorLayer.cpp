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
#include <Geode/modify/CreatorLayer.hpp>
#include "../ui/JFPMenuLayer.hpp"
#include "../utils/StringGen.hpp"

using namespace geode::prelude;


class $modify(GenerateLevelLayer, CreatorLayer) {

	bool init() {
		if (!CreatorLayer::init()) {
			return false;
		}
		if (Mod::get()->getSettingValue<bool>("lmao-button")) {
			auto optionButton = CCMenuItemSpriteExtra::create(
				CircleButtonSprite::createWithSpriteFrameName("lmao_s.png"_spr, .80f, CircleBaseColor::DarkAqua, CircleBaseSize::Medium),
				this,
				menu_selector(GenerateLevelLayer::onJFPButton)
			);

			auto menu = this->getChildByID("bottom-left-menu");
			menu->addChild(optionButton);
			optionButton->setID("jfp-launch-button"_spr);

			menu->updateLayout();
		}
		return true;
	}

	void onJFPConfirm() {
		auto jfpLayer = JFPMenuLayer::scene();
		auto jlTransition = CCTransitionFade::create(0.5, jfpLayer);
		CCDirector::sharedDirector()->pushScene(jlTransition);
	}

	void onJFPButton(CCObject*) {
		//openSettingsPopup(Mod::get());

		const char* warningMsg = "JFP is still in active development.\n\nBy clicking below, you acknowledge that this mod could lead to crashes and instability (particularly when exiting the game) and are choosing to continue anyway.\n\nRemember to back up your data!";

		if (!Mod::get()->getSavedValue<bool>("ackDisclaimer")) {
			createQuickPopup(
				"JFP",
				warningMsg,
				"I Understand", nullptr,
				[&](bool b1, auto) {
					if (b1) {
						Mod::get()->setSavedValue<bool>("ackDisclaimer", "true");
						onJFPConfirm();
					}
				}
			);
		} else {
			onJFPConfirm();
		}
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