// Copyright 2025 GDSRWave
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>    
#include <string>
#include <thread>
#include "JFPMenuLayer.hpp"
#include "JFPOptionLayer.hpp"
#include "JFPScreenshotLayer.hpp"
#include "../utils/StringGen.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/general.hpp>
#include "../utils/shared.hpp"
#include "../utils/Ninja.hpp"
#include "../utils/OptionStr.hpp"
#include "../utils/Theming.hpp"
#include "./OptionStrPopup.hpp"
#include "./ThemeSelectPopup.hpp"

std::mutex mtx;
std::condition_variable cv;

// Reference: https://github.com/Cvolton/betterinfo-geode/blob/de80d5c843b1d6e5fc28816b1aeede1178ae9095/src/layers/CustomCreatorLayer.cpp

JFPMenuLayer* JFPMenuLayer::create() {
    auto ret = new JFPMenuLayer;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

GJGameLevel* createGameLevel() {
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

    if (commonLevel) {
        commonLevel->release();
        commonLevel = nullptr;
    }
    commonLevel = GJGameLevel::create();
    commonLevel->retain();
    commonLevel->dataLoaded(dict.get());
    commonLevel->m_levelType = GJLevelType::Editor;

    return commonLevel;
}

bool JFPMenuLayer::init() {
    if (!JFPGenericLayer::init()) {
        return false;
    }

    if (commonLevel) {
        commonLevel->release();
        commonLevel = nullptr;
    }

    auto windowDim = CCDirector::sharedDirector()->getWinSize();
    auto backgroundSprite2 = CCSprite::createWithSpriteFrameName(
        "game_bg_27_001_m.png"_spr);
    backgroundSprite2->setID("background-menu"_spr);

    auto size = backgroundSprite2->getContentSize();

    backgroundSprite2->setScale(windowDim.width / size.width);

    backgroundSprite2->setAnchorPoint({0, 0});
    backgroundSprite2->setPosition({0, -75});

    backgroundSprite2->setColor({8, 8, 8});

    backgroundSprite2->setZOrder(-2);
    addChild(backgroundSprite2);

    auto understandableSprite = CCSprite::createWithSpriteFrameName(
        "understandable_s.png"_spr);
    understandableSprite->setID("background"_spr);
    understandableSprite->setPosition({0, 0});
    understandableSprite->setScale(1.75f);
    understandableSprite->setAnchorPoint({0.f, 0.f});
    understandableSprite->setZOrder(-1);
    addChild(understandableSprite);

    uint32_t globalSeed = Mod::get()->getSavedValue<uint32_t>("global-seed", 0);
    std::string displaySeed = "Last Seed: " +
        (globalSeed ? geode::utils::numToString(globalSeed) : "N/A");
    auto seedTxt = CCLabelBMFont::create(displaySeed.c_str(), "goldFont.fnt");
    seedTxt->setScale(0.9f);
    auto seedCopyBtn = CCMenuItemSpriteExtra::create(
        seedTxt,
        this,
        menu_selector(JFPMenuLayer::onCopySeed));

    seedCopyBtn->setID("jfp-seed-button"_spr);
    seedCopyBtn->setSizeMult(1.1f);

    auto settings = getSettings(JFPGen::JFPBiome::Juggernaut);
    std::string displayOptStr = "Options: " + exportSettings(settings);
    m_optText = CCLabelBMFont::create(displayOptStr.c_str(), "goldFont.fnt");
    m_optText->setScale(0.9f);
    auto optCopyBtn = CCMenuItemSpriteExtra::create(
        m_optText,
        this,
        menu_selector(JFPMenuLayer::onCopyOpt));

    optCopyBtn->setID("jfp-options-btn"_spr);
    optCopyBtn->setSizeMult(0.9f);

    auto optMenu = CCMenu::create();
    auto optRefreshBtnSpr = CCSprite::createWithSpriteFrameName(
        "GJ_updateBtn_001.png");
    optRefreshBtnSpr->setScale(0.5f);
    auto optRefreshBtn = CCMenuItemSpriteExtra::create(
        optRefreshBtnSpr,
        this, menu_selector(JFPMenuLayer::onOptRefresh));
    optMenu->setLayout(RowLayout::create()
        ->setGap(2.f));

    optMenu->addChild(optCopyBtn);
    optMenu->addChild(optRefreshBtn);
    optMenu->updateLayout();

    // auto optTxt = CCLabelBMFont::create("", "goldFont.fnt");

    auto themeSprite = CircleButtonSprite::createWithSpriteFrameName(
        "paint_s.png"_spr, 1.f,
        CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    themeSprite->setScale(1.0f);
    auto themeButton = CCMenuItemSpriteExtra::create(
        themeSprite,
        this, menu_selector(JFPMenuLayer::onThemeButton));
    themeButton->setID("jfp-theme-button"_spr);

    auto optionSprite = CircleButtonSprite::createWithSpriteFrameName(
        "options_s.png"_spr, 1.f,
        CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    optionSprite->setScale(1.0f);
    auto optionButton = CCMenuItemSpriteExtra::create(
        optionSprite,
        this, menu_selector(JFPMenuLayer::openOptions));
    optionButton->setID("jfp-option-button"_spr);

    auto autoGenSprite = CircleButtonSprite::createWithSpriteFrameName(
        "dabbink_s.png"_spr, 1.125f,
        CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    autoGenSprite->setScale(1.3f);
    auto autoGenButton = CCMenuItemSpriteExtra::create(
        autoGenSprite,
        this,
        menu_selector(JFPMenuLayer::onAutoGenButton));
    autoGenButton->setID("auto-generate-level-button"_spr);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(JFPMenuLayer::onInfoButton));
    infoBtn->setID("info-button"_spr);
    infoBtn->setSizeMult(1.1f);

    auto importBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("importIcon_001.png"_spr),
        this,
        menu_selector(JFPMenuLayer::onImportButton));
    importBtn->setID("import-opt-button"_spr);
    importBtn->setSizeMult(1.1f);

    auto urBtnMenu = CCMenu::create();
    urBtnMenu->setLayout(ColumnLayout::create()
        ->setGap(5.f));
    urBtnMenu->addChild(importBtn);
    urBtnMenu->addChild(infoBtn);
    urBtnMenu->updateLayout();

    auto garageBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("garageRope_001.png"),
        this,
        menu_selector(JFPMenuLayer::onGarageButton));
    garageBtn->setID("garage-button"_spr);
    garageBtn->m_animationType = MenuAnimationType::Move;
    garageBtn->m_startPosition = CCPoint(18.875f, 35.250f);
    garageBtn->m_offset = CCPoint(0.f, -8.f);
    garageBtn->setSizeMult(1.1f);

    auto twttrBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("twitterIcon_001.png"_spr),
        this,
        menu_selector(JFPMenuLayer::onTwitterButton));
    twttrBtn->setID("twitter-link-button"_spr);
    twttrBtn->setSizeMult(1.1f);

    auto menu = CCMenu::create();
    auto menu2 = CCMenu::create();
    auto menu3 = CCMenu::create();
    auto menu4 = CCMenu::create();

    menu->setID("center-menu"_spr);
    menu->setPosition({windowDim.width/2, windowDim.height/2+50.f});
    menu->setContentSize({300.f, 240.f});
    menu->setLayout(RowLayout::create()
        ->setGap(7.f));
    menu->addChild(themeButton);
    menu->addChild(autoGenButton);
    menu->addChild(optionButton);
    addChild(menu);

    menu2->setID("inf-menu"_spr);
    menu2->setAnchorPoint({0.5, 0.5});
    menu2->setPosition({windowDim.width - 42.f, windowDim.height - 35.f});
    menu2->addChild(garageBtn);
    menu2->addChild(urBtnMenu);
    menu2->setLayout(RowLayout::create()
        ->setGap(7.f));
    menu2->updateLayout();
    garageBtn->setPositionY(garageBtn->getPositionY() + 8);
    addChild(menu2);

    menu3->setLayout(ColumnLayout::create()
        ->setGap(5.f));
    menu3->setPosition({windowDim.width / 2, windowDim.height / 2 - 10.f});
    menu3->setScale(0.7f);
    menu3->addChild(optMenu);
    menu3->addChild(seedCopyBtn);
    addChild(menu3);

    menu4->setID("twt-menu"_spr);
    menu4->setAnchorPoint({1, 0});
    menu4->setPosition({windowDim.width - 19.5f, 20.f});
    menu4->addChild(twttrBtn);
    menu4->updateLayout();
    addChild(menu4);

    menu->updateLayout();
    menu3->updateLayout();

    return true;
}

void JFPMenuLayer::onThemeButton(CCObject*) {
    ThemeSelectPopup::create("")->show();
}

void JFPMenuLayer::onCopySeed(CCObject*) {
    uint32_t globalSeed = Mod::get()->getSavedValue<uint32_t>("global-seed", 0);
    if (globalSeed) {
        clipboard::write(geode::utils::numToString(globalSeed));
        Notification::create("Copied to clipboard",
            NotificationIcon::None, 0.5f)->show();
    } else {
        Notification::create("No seed has been generated!",
            NotificationIcon::Error, 0.5f)->show();
    }
}

void JFPMenuLayer::onCopyOpt(CCObject*) {
    auto settings = getSettings(JFPGen::JFPBiome::Juggernaut);
    clipboard::write(exportSettings(settings));
    Notification::create("Copied to clipboard",
        NotificationIcon::None, 0.5f)->show();
}

void JFPMenuLayer::onOptRefresh(CCObject*) {
    auto settings = getSettings(JFPGen::JFPBiome::Juggernaut);
    std::string displayOptStr = "Options: " + exportSettings(settings);
    m_optText->setCString(displayOptStr.c_str());
}

void JFPMenuLayer::keyBackClicked() {
    if (state != JFPGen::AutoJFP::NotInAutoJFP) return;
    JFPGenericLayer::keyBackClicked();
}

void JFPMenuLayer::onBack(CCObject* object) {
    if (state != JFPGen::AutoJFP::NotInAutoJFP) return;
    JFPGenericLayer::onBack(object);
}

void JFPMenuLayer::onGarageButton(CCObject*) {
    if (state != JFPGen::AutoJFP::NotInAutoJFP) return;
    jfpActive = true;
    auto gjgTransition = CCTransitionMoveInT::create(
        0.5, GJGarageLayer::scene());
    CCDirector::sharedDirector()->pushScene(gjgTransition);
}

void JFPMenuLayer::onAutoGenButton(CCObject*) {
    if (state != JFPGen::AutoJFP::NotInAutoJFP) return;
    if (!Mod::get()->getSavedValue<bool>("ack-disclaimer")) {
        FLAlertLayer::create("JFP",
            "Please accept the disclaimer to continue!", "OK")->show();
        return;
    }
    state = JFPGen::AutoJFP::InAutoTransition;
    att1 = true;

    auto dir = CCDirector::sharedDirector();

    std::string themeName =
        Mod::get()->getSavedValue<std::string>("active-theme");
    auto tmd = ThemeGen::parseThemeMeta(themeName);
    auto conflicts = ThemeGen::tagConflicts(tmd);

    if (conflicts.size() > 0) {
        std::string message = "Your options are incompatible with the the \"" +
            themeName + "\" theme's following tags:\n" +
            fmt::format("{}", fmt::join(conflicts, ", ")) +
            "\nSome segments may not be themed.";
        auto jfpConfirm = createQuickPopup(
            "JFP",
            message.c_str(),
            "Continue", nullptr,
            [=](bool b1, auto) {
                if (b1) return dir->replaceScene(JFPScreenshotLayer::scene());
                else {
                    return true;
                }
            });
    } else {
        dir->replaceScene(JFPScreenshotLayer::scene());
    }
    return;
}

void JFPMenuLayer::onAutoGen() {
    jfpActive = true;
    Mod::get()->setSavedValue<uint32_t>(
        "total-played",
        Mod::get()->getSavedValue<uint32_t>("total-played", 0) + 1);

    auto level = createGameLevel();
    if (!level) {
        state = JFPGen::AutoJFP::NotInAutoJFP;
        return FLAlertLayer::create("Error",
            "Level generation failed. (gen-empty)", "OK")->show();
    }
    auto newScene = PlayLayer::scene(level, false, false);
    // seems to work better than pushScene?
    CCDirector::sharedDirector()->replaceScene(newScene);
}

void JFPMenuLayer::onInfoButton(CCObject*) {
    std::string playCt = geode::utils::numToString(
        Mod::get()->getSavedValue<uint32_t>("total-played", 0));
    std::string message = "Advanced Random Wave Generation\n\n"
        "Special thanks to the Geode community and to early playtesters;"
        " you made this possible!\n\n"
        "Total Rounds Played: " + playCt;

    auto infoLayer = FLAlertLayer::create(nullptr, "Juggernaut Freeplay",
        message.c_str(),
        "I get it man",
        nullptr,
        400.f
    );
    infoLayer->setID("jfp-info-layer"_spr);
    infoLayer->show();
}

void JFPMenuLayer::onImportButton(CCObject*) {
    OptionStrPopup::create("")->show();
}

void JFPMenuLayer::onTwitterButton(CCObject*) {
    CCApplication::sharedApplication()->openURL("https://twitter.com/shiestykahuna");
}

void JFPMenuLayer::openOptions(CCObject*) {
    auto optsLayer = JFPOptionLayer::scene();
    auto jlTransition = CCTransitionFade::create(0.5, optsLayer);
    CCDirector::sharedDirector()->pushScene(jlTransition);
}

CCScene* JFPMenuLayer::scene() {
    auto scene = CCScene::create();
    auto layer = JFPMenuLayer::create();
    scene->addChild(layer);

    if (!GameManager::sharedState()->getGameVariable("0122")) {
        auto bgmPath =
            std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "jfpLoop.mp3";
        FMODAudioEngine::get()->playMusic(bgmPath, true, 1.0f, 1);
    }
    // queueInMainThread([=]() {
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     if (state != JFPGen::AutoJFP::NotInAutoJFP) {
    //         layer->onAutoGen();
    //     }
    // });
    return scene;
}

void JFPMenuLayer::onEnterTransitionDidFinish() {
    JFPGenericLayer::onEnterTransitionDidFinish();
}


