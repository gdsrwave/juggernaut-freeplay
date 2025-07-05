#include "JFPMenuLayer.hpp"
#include "ThemeSelect.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "../utils/StringGen.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "../utils/shared.hpp"

// Reference: https://github.com/Cvolton/betterinfo-geode/blob/de80d5c843b1d6e5fc28816b1aeede1178ae9095/src/layers/CustomCreatorLayer.cpp

JFPMenuLayer* JFPMenuLayer::create() {
    auto ret = new JFPMenuLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
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

    delete commonLevel;
    commonLevel = GJGameLevel::create();
    GJGameLevel* level = GJGameLevel::create();

    commonLevel->dataLoaded(dict.get());
    commonLevel->m_levelType = GJLevelType::Editor;
    return commonLevel;
}

bool JFPMenuLayer::init() {
    if (!JFPGenericLayer::init()) {
        return false;
    }
    auto windowDim = CCDirector::sharedDirector()->getWinSize();
    auto backgroundSprite2 = CCSprite::createWithSpriteFrameName("game_bg_27_001_m.png"_spr);
    backgroundSprite2->setID("background-menu"_spr);
    
    auto size = backgroundSprite2->getContentSize();
    
    backgroundSprite2->setScale(windowDim.width / size.width);
    
    backgroundSprite2->setAnchorPoint({0, 0});
    backgroundSprite2->setPosition({0,-75});
    
    backgroundSprite2->setColor({8, 8, 8});
    
    backgroundSprite2->setZOrder(-2);
    addChild(backgroundSprite2);

    auto understandableSprite = CCSprite::createWithSpriteFrameName("understandable_s.png"_spr);
    understandableSprite->setID("background"_spr);
    understandableSprite->setPosition({0, 0});
    understandableSprite->setScale(1.75f);
    understandableSprite->setAnchorPoint({0.f, 0.f});
    understandableSprite->setZOrder(-1);
    addChild(understandableSprite);

    // auto title = CCLabelBMFont::create("JFP Menu", "goldFont.fnt");
    // title->setPosition({windowDim.width / 2, windowDim.height / 2 + 100.f});
    // title->setColor({255, 255, 255});
    // title->setScale(1.5f);
    // addChild(title);

    auto themeSprite = CircleButtonSprite::createWithSpriteFrameName("snoop_s.png"_spr, 1.f, CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    themeSprite->setScale(1.1f);
    auto themeButton = CCMenuItemSpriteExtra::create(
        themeSprite,
        this,
        menu_selector(JFPMenuLayer::onThemeButton)
    );
    themeButton->setID("jfp-theme-button"_spr);

    auto optionSprite = CircleButtonSprite::createWithSpriteFrameName("options_s.png"_spr, 1.f, CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    optionSprite->setScale(1.1f);
    auto optionButton = CCMenuItemSpriteExtra::create(
        optionSprite,
        this,
        menu_selector(JFPMenuLayer::onOptionButton)
    );
    optionButton->setID("jfp-option-button"_spr);

    auto autoGenSprite = CircleButtonSprite::createWithSpriteFrameName("dabbink_s.png"_spr, 1.125f, CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    autoGenSprite->setScale(1.1f);
    auto autoGenButton = CCMenuItemSpriteExtra::create(
        autoGenSprite,
        this,
        menu_selector(JFPMenuLayer::onAutoGenButton)
    );
    autoGenButton->setID("auto-generate-level-button"_spr);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(JFPMenuLayer::onInfoButton)
    );
    infoBtn->setID("info-button"_spr);
    //infoBtn->setPosition({(windowDim.width/2)-25, (windowDim.height/2)-25});
    infoBtn->setSizeMult(1.1f);

    auto garageBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("garageRope_001.png"),
        this,
        menu_selector(JFPMenuLayer::onGarageButton)
    );
    garageBtn->setID("garage-button"_spr);
    garageBtn->setSizeMult(1.1f);
    
    auto menu = CCMenu::create();
    auto menu2 = CCMenu::create();

    menu->setID("center-menu"_spr);
    menu->setPosition({windowDim.width/2, windowDim.height/2+50.f});
    menu->setContentSize({300.f, 240.f});
    menu->setLayout(RowLayout::create()
        ->setGap(7.f)
    );
    menu->addChild(autoGenButton);
    menu->addChild(optionButton);
    addChild(menu);

    menu2->setID("inf-menu"_spr);
    menu2->setAnchorPoint({0, 0});
    menu2->setPosition({216, 265});
    menu2->addChild(garageBtn);
    menu2->addChild(infoBtn);
    //menu2->setPosition({(windowDim.width/2)-25.f, (windowDim.height/2)-25.f});
    menu2->setLayout(RowLayout::create()
        ->setGap(7.f)
    );
    menu2->updateLayout();
    garageBtn->setPositionY(garageBtn->getPositionY()-14.f);
    addChild(menu2);

    menu->updateLayout();

    return true;
}

void JFPMenuLayer::onOptionButton(CCObject*) {
    openSettingsPopup(Mod::get());
    // auto searchLayer = LevelSearchLayer::scene(0);
    // CCDirector::sharedDirector()->pushScene(searchLayer);
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
    auto gjgTransition = CCTransitionMoveInT::create(0.5, GJGarageLayer::scene());
    CCDirector::sharedDirector()->pushScene(gjgTransition);
}

void JFPMenuLayer::onAutoGenButton(CCObject*) {
    if (state != JFPGen::AutoJFP::NotInAutoJFP) return;
    jfpActive = true;
    state = JFPGen::AutoJFP::JustStarted;
    auto level = createGameLevel();
    if (!level) {
        state = JFPGen::AutoJFP::NotInAutoJFP;
        return FLAlertLayer::create("Error", "Level generation failed. (gen-empty)", "OK")->show();
    }
    auto newScene = PlayLayer::scene(level, false, false);
    CCDirector::sharedDirector()->replaceScene(newScene); // seems to work better than pushScene?
}

void JFPMenuLayer::onThemeButton(CCObject*) {
    auto themeSelectLayer = GJDropDownLayer::create("Select Theme", 220.f, true);
    themeSelectLayer->setID("theme-select-layer"_spr);
    themeSelectLayer->setContentSize({640, 480});
    themeSelectLayer->setAnchorPoint({0.5f, 0.5f});
    themeSelectLayer->setPosition({0, 0});
    themeSelectLayer->addChild(ThemeSelectLayer::create());
    CCScene::get()->addChild(themeSelectLayer);
}

void JFPMenuLayer::onInfoButton(CCObject*) {
    auto infoLayer = FLAlertLayer::create(nullptr, "Juggernaut Freeplay",
        "Advanced Random Wave Generation\n\n"
        "Contributors:\nMartin C. (gdsrwave)\ntheyareonit\n\n"
        "Special thanks to the Geode community and to early playtesters!\n",
        "I get it man",
        nullptr,
        400.f
    );
    infoLayer->setID("jfp-info-layer"_spr);
    infoLayer->show();
}

CCScene* JFPMenuLayer::scene() {
    auto scene = CCScene::create();
    auto layer = JFPMenuLayer::create();
    scene->addChild(layer);

    if(!GameManager::sharedState()->getGameVariable("0122")) {
        auto bgmPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "jfpLoop.mp3";
        FMODAudioEngine::get()->playMusic(bgmPath, true, 1.0f, 1);
    }
    return scene;
}