// Copyright 2025 GDSRWave
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "../utils/shared.hpp"
#include "../ui/JFPMenuLayer.hpp"
#include "../utils/OptionStr.hpp"

using namespace geode::prelude;

class SettingsLaunchBtn : public SettingV3 {
 public:
    static Result<std::shared_ptr<SettingV3>> parse(
            std::string const& key, std::string const& modID,
            matjson::Value const& json) {
        auto res = std::make_shared<SettingsLaunchBtn>();
        auto root = checkJson(json, "CopyButtonSetting");

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);

        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }

    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}

    SettingNodeV3* createNode(float width) override;
};

class SettingsLaunchBtnNodeV3 : public SettingNodeV3 {
 protected:
    bool init(std::shared_ptr<SettingsLaunchBtn> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;

        auto launchSprite = ButtonSprite::create(
            "Open Menu", 0, false, "bigFont.fnt",
            "GJ_button_04.png", 0, 0.75f);
        launchSprite->setScale(.5f);
        auto launchButton = CCMenuItemSpriteExtra::create(
            launchSprite,
            this, menu_selector(SettingsLaunchBtnNodeV3::onJFPButton));

        this->getButtonMenu()->addChildAtPosition(launchButton, Anchor::Right);
        this->getButtonMenu()->setPosition(getContentSize() / 2);
        this->getButtonMenu()->setAnchorPoint({ .5f, .5f });
        this->getButtonMenu()->updateLayout();

        this->updateState(nullptr);

        return true;
    }

    void onJFPConfirm() {
        auto jfpLayer = JFPMenuLayer::scene();
        auto jlTransition = CCTransitionFade::create(0.5, jfpLayer);
        CCDirector::sharedDirector()->pushScene(jlTransition);
    }

    void onJFPButton(CCObject*) {

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

    void onCommit() override {}
    void onResetToDefault() override {}

 public:
    static SettingsLaunchBtnNodeV3* create(
            std::shared_ptr<SettingsLaunchBtn> setting, float width) {
        auto ret = new SettingsLaunchBtnNodeV3();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }
};

SettingNodeV3* SettingsLaunchBtn::createNode(float width) {
    return SettingsLaunchBtnNodeV3::create(
        std::static_pointer_cast<SettingsLaunchBtn>(shared_from_this()), width);
}

$execute {
    (void) Mod::get()->registerCustomSettingType(
        "jfp-alt-launcher", &SettingsLaunchBtn::parse);
}
