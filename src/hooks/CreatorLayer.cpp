// Copyright 2025 GDSRWave
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <fmt/core.h>
#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
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
                CircleButtonSprite::createWithSpriteFrameName(
                    "lmao_s.png"_spr, .80f,
                    CircleBaseColor::DarkAqua, CircleBaseSize::Medium),
                this,
                menu_selector(GenerateLevelLayer::onJFPButton));

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

    void loadDefaults(bool full = false) {
        auto* mod = Mod::get();

        // biome 0 (Juggernaut) default options
        mod->setSavedValue<uint8_t>("opt-0-corridor-rules", 5);
        mod->setSavedValue<uint16_t>("opt-0-corridor-height", 60);
        mod->setSavedValue<uint32_t>("opt-0-length", 400);
        mod->setSavedValue<uint8_t>("opt-0-grav-portals-diff", 1);
        mod->setSavedValue<uint8_t>("opt-0-speed-changes-diff", 1);
        mod->setSavedValue<uint8_t>("opt-0-speed-changes", 9);
        mod->setSavedValue<bool>("opt-0-show-meter-marks", true);
        mod->setSavedValue<uint32_t>("opt-0-mark-interval", 100);
        mod->setSavedValue<std::string>("opt-0-filename", "waveman");
        mod->setSavedValue<uint8_t>("opt-0-music-source", 1);
        mod->setSavedValue<uint8_t>("opt-0-color-mode", 1);
        mod->setSavedValue<uint8_t>("opt-u-starting-speed", 4);
        mod->setSavedValue<bool>("opt-0-background-texture", 1);

        // biome 0 full reload
        if (full) {
            mod->setSavedValue<bool>("opt-u-waveman-button-shown", false);
            mod->setSavedValue<bool>("opt-u-theme-creator-tools", false);
            mod->setSavedValue<bool>("opt-u-debug", false);
            mod->setSavedValue<bool>("opt-0-using-set-seed", false);
            mod->setSavedValue<uint8_t>("opt-0-seed", 0);
            mod->setSavedValue<bool>("opt-0-using-speed-changes", false);
            mod->setSavedValue<bool>("opt-0-using-size-changes", false);
            mod->setSavedValue<uint8_t>("opt-0-starting-size", 0);
            mod->setSavedValue<uint8_t>("opt-0-size-transition-type", 0);
            mod->setSavedValue<bool>("opt-u-save-on-close", false);
            mod->setSavedValue<bool>("opt-0-widen-playfield-bounds", false);
            mod->setSavedValue<bool>("opt-0-low-visibility", false);
            mod->setSavedValue<bool>("opt-0-add-corner-pieces", false);
            mod->setSavedValue<bool>("opt-0-show-finish-line", false);
            mod->setSavedValue<bool>("opt-0-hide-icon", false);
            mod->setSavedValue<bool>("opt-0-using-grav-portals", false);
            mod->setSavedValue<bool>("opt-0-grav-portal-start", false);
            mod->setSavedValue<bool>("opt-0-fake-grav-portals", false);
            mod->setSavedValue<bool>("opt-0-remove-portals-in-spams", false);
            mod->setSavedValue<uint8_t>("opt-0-portal-input-types", 0);
            mod->setSavedValue<bool>("opt-0-using-corridor-spikes", false);
            mod->setSavedValue<bool>("opt-0-corridor-fuzz", false);
            mod->setSavedValue<uint8_t>("opt-0-spike-placement-types", 0);
        }

        mod->setSavedValue<bool>("ack-disclaimer", "true");
    }

    void onJFPButton(CCObject*) {
        // openSettingsPopup(Mod::get());

        const char* warningMsg =
            "JFP is still in active development.\n\nBy clicking below, "
            "you acknowledge that this mod could lead to crashes and"
            "instability (particularly when exiting the game) and are choosing"
            "to continue anyway.\n\nRemember to back up your data!";

        if (!Mod::get()->getSavedValue<bool>("ack-disclaimer")) {
            createQuickPopup(
                "JFP",
                warningMsg,
                "I Understand", nullptr,
                [&](bool b1, auto) {
                    if (b1) {
                        loadDefaults();
                        onJFPConfirm();
                    }
                });
        } else {
            onJFPConfirm();
        }
    }

    static GJGameLevel* createGameLevel() {
        std::srand(std::time(0));
        std::string levelString = "<?xml version=\"1.0\"?>"
        "<plist version=\"1.0\" gjver=\"2.0\"><dict><k>root</k>"
        + JFPGen::jfpStringGen(false) + "</dict></plist>";

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
