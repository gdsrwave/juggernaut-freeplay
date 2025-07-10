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
#include "../utils/shared.hpp"
#include "../utils/StringGen.hpp"
#include "../ui/JFPMenuLayer.hpp"

using namespace geode::prelude;
using namespace gmd;

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(LBGenerateLevelLayer, LevelBrowserLayer) {

	bool init(GJSearchObject* s) {

		if (!LevelBrowserLayer::init(s)) {
			return false;
		}
		
        if(Mod::get()->getSettingValue<bool>("waveman-button")) {
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
        }

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
		std::string levelString = JFPGen::jfpStringGen(true);
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