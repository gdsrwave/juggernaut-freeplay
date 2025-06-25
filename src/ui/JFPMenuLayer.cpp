#include "JFPMenuLayer.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "../utils/StringGen.hpp"
#include <Geode/ui/GeodeUI.hpp>

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

    auto corrozSprite = CCSprite::createWithSpriteFrameName("corroz_s.png"_spr);
    corrozSprite->setID("corroz-sprite"_spr);
    corrozSprite->setScale(1.7f);

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

    auto menu = CCMenu::create();

    menu->setPosition({windowDim.width / 2, windowDim.height / 2 + 50.f});
    menu->setContentSize({ 300.f, 240.f });
    menu->setLayout(RowLayout::create()
        ->setGap(7.f)
    );
    menu->addChild(autoGenButton);
    menu->addChild(corrozSprite);
    menu->addChild(optionButton);
    addChild(menu);

    menu->updateLayout();

    return true;
}

void JFPMenuLayer::onOptionButton(CCObject*) {
    openSettingsPopup(Mod::get());
}

void JFPMenuLayer::onAutoGenButton(CCObject*) {
    state = JFPGen::AutoJFP::JustStarted;
    auto level = createGameLevel();
    if (!level) {
        state = JFPGen::AutoJFP::NotInAutoJFP;
        return FLAlertLayer::create("Error", "Could not generate level", "Okay buddy...")->show();
    }
    auto newScene = PlayLayer::scene(level, false, false);
    CCDirector::sharedDirector()->replaceScene(newScene); // seems to work better than pushScene?
}

CCScene* JFPMenuLayer::scene() {
    auto layer = JFPMenuLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}