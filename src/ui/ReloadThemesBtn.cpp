// Copyright 2025 GDSRWave
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class ReloadThemesBtn : public SettingV3 {
 public:
    static Result<std::shared_ptr<SettingV3>> parse(
            std::string const& key, std::string const& modID,
            matjson::Value const& json) {
        auto res = std::make_shared<ReloadThemesBtn>();
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

class ReloadThemesBtnNodeV3 : public SettingNodeV3 {
 protected:
    bool init(std::shared_ptr<ReloadThemesBtn> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;

        auto folderSprite = ButtonSprite::create(
            "Open Folder", 0, false, "bigFont.fnt",
            "GJ_button_04.png", 0, 0.75f);
        folderSprite->setScale(.5f);
        auto folderButton = CCMenuItemSpriteExtra::create(
            folderSprite,
            this, menu_selector(ReloadThemesBtnNodeV3::onClickFolder));

        auto reloadSprite = ButtonSprite::create(
            "Reload Themes", 0, false, "bigFont.fnt",
            "GJ_button_04.png", 0, 0.75f);
        reloadSprite->setScale(.5f);
        auto reloadButton = CCMenuItemSpriteExtra::create(
            reloadSprite,
            this, menu_selector(ReloadThemesBtnNodeV3::onClickReload));

        this->getButtonMenu()->addChildAtPosition(folderButton, Anchor::Left);
        this->getButtonMenu()->addChildAtPosition(reloadButton, Anchor::Right);
        this->getButtonMenu()->setPosition(getContentSize() / 2);
        this->getButtonMenu()->setAnchorPoint({ .5f, .5f });
        this->getButtonMenu()->updateLayout();

        this->updateState(nullptr);

        return true;
    }

    void onClickFolder(CCObject*) {
        auto localPath = CCFileUtils::sharedFileUtils();
        file::openFolder(localPath->getWritablePath() + "jfp\\themes\\");
    }

    void onClickReload(CCObject*) {
        std::filesystem::path srcDir = Mod::get()->getResourcesDir();
        std::vector<std::string> themeFiles;
        for (const auto& fileName :
                std::filesystem::directory_iterator(srcDir)) {
            auto fileStr = fileName.path().filename().string();
            if (fileStr.size() >= 5 &&
                    fileStr.substr(fileStr.size() - 5) == ".jfpt") {
                themeFiles.push_back(fileStr);
            }
        }
        std::string message = "Are you sure you want to reload default themes?"
            "\nThe following files will be overwritten:\n" +
            fmt::format("{}", fmt::join(themeFiles, ", "));
        auto jfpConfirm = createQuickPopup(
            "JFP",
            message.c_str(),
            "No", "Yes",
            [&](bool b1, bool b2) {
                if (b2) {
                    log::info("Reloading themes...");
                    setupJFPDirectories(true);
                    FLAlertLayer::create("JFP",
                        "Successfully reloaded default themes.", "OK")->show();
                }
            });
    }

    void onCommit() override {}
    void onResetToDefault() override {}

 public:
    static ReloadThemesBtnNodeV3* create(
            std::shared_ptr<ReloadThemesBtn> setting, float width) {
        auto ret = new ReloadThemesBtnNodeV3();
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

SettingNodeV3* ReloadThemesBtn::createNode(float width) {
    return ReloadThemesBtnNodeV3::create(
        std::static_pointer_cast<ReloadThemesBtn>(shared_from_this()), width);
}

$execute {
    (void) Mod::get()->registerCustomSettingType(
        "reload-themes", &ReloadThemesBtn::parse);
}
