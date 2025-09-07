// Copyright 2025 GDSRWave
#include <memory>
#include <string>
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "JFPOptionLayer.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/general.hpp>
#include "./OptionStrPopup.hpp"
#include "../option_popups/CorridorOptPopup.hpp"
#include "../option_popups/GravHazOptPopup.hpp"
#include "../option_popups/SoundtrackOptPopup.hpp"
#include "../option_popups/SpeedSizeOptPopup.hpp"
#include "../option_popups/JunkDrawerOptPopup.hpp"
#include "../option_popups/VisualsOptPopup.hpp"


JFPOptionLayer* JFPOptionLayer::create() {
    auto ret = new JFPOptionLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool JFPOptionLayer::init() {
    if (!JFPGenericLayer::init()) {
        return false;
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

    auto optionsBtn00 = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "setting1.png"_spr),
        this,
        menu_selector(JFPOptionLayer::onOptionsLaunch));
    optionsBtn00->setID("jfp-options-0-0");

    auto optionsBtn01 = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "setting2.png"_spr),
        this,
        menu_selector(JFPOptionLayer::onOptionsLaunch));
    optionsBtn01->setID("jfp-options-0-1");

    auto optionsBtn02 = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "setting3.png"_spr),
        this,
        menu_selector(JFPOptionLayer::onOptionsLaunch));
    optionsBtn02->setID("jfp-options-0-2");

    auto optionsBtn10 = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "setting4.png"_spr),
        this,
        menu_selector(JFPOptionLayer::onOptionsLaunch));
    optionsBtn10->setID("jfp-options-1-0");

    auto optionsBtn11 = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "setting5.png"_spr),
        this,
        menu_selector(JFPOptionLayer::onOptionsLaunch));
    optionsBtn11->setID("jfp-options-1-1");

    auto optionsBtn12 = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "setting6.png"_spr),
        this,
        menu_selector(JFPOptionLayer::onOptionsLaunch));
    optionsBtn12->setID("jfp-options-1-2");

    // auto oldOptionSprite = CircleButtonSprite::createWithSpriteFrameName(
    //     "options_s.png"_spr, 1.f,
    //     CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    // oldOptionSprite->setScale(1.1f);
    // auto oldOptionButton = CCMenuItemSpriteExtra::create(
    //     oldOptionSprite,
    //     this, menu_selector(JFPOptionLayer::onVisualsButton));
    // oldOptionButton->setID("jfp-options-0-1"_spr);

    // auto optionSprite = CircleButtonSprite::createWithSpriteFrameName(
    //     "options_s.png"_spr, 1.f,
    //     CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    // optionSprite->setScale(1.1f);
    // auto optionButton = CCMenuItemSpriteExtra::create(
    //     optionSprite,
    //     this, menu_selector(JFPOptionLayer::onSpeedButton));
    // optionButton->setID("jfp-options-0-2"_spr);

    auto menu = CCMenu::create();

    menu->setID("center-menu"_spr);
    menu->setPosition({windowDim.width/2, windowDim.height/2 + 4.f});
    menu->setContentSize({250.f, 200.f});
    menu->setLayout(
        RowLayout::create()
            ->setGap(9.f)
            ->setGrowCrossAxis(true)
            ->setCrossAxisOverflow(false));
    menu->addChild(optionsBtn00);
    menu->addChild(optionsBtn01);
    menu->addChild(optionsBtn02);
    menu->addChild(optionsBtn10);
    menu->addChild(optionsBtn11);
    menu->addChild(optionsBtn12);
    addChild(menu);

    menu->updateLayout();

    return true;
}

void JFPOptionLayer::keyBackClicked() {
    JFPGenericLayer::keyBackClicked();
}

void JFPOptionLayer::onBack(CCObject* object) {
    JFPGenericLayer::onBack(object);
}

void JFPOptionLayer::onOptionsLaunch(CCObject* object) {
    std::string selectorID = typeinfo_cast<CCNode*>(object)->getID();
    log::info("{}", selectorID);
    if (selectorID == "jfp-options-0-0") {
        CorridorOptPopup::create("")->show();
    } else if (selectorID == "jfp-options-0-1") {
        VisualsOptPopup::create("")->show();
    } else if (selectorID == "jfp-options-0-2") {
        SpeedSizeOptPopup::create("")->show();
    } else if (selectorID == "jfp-options-1-0") {
        GravHazOptPopup::create("")->show();
    } else if (selectorID == "jfp-options-1-1") {
        SoundtrackOptPopup::create("")->show();
    } else if (selectorID == "jfp-options-1-2") {
        JunkDrawerOptPopup::create("")->show();
    }
}

CCScene* JFPOptionLayer::scene() {
    auto scene = CCScene::create();
    auto layer = JFPOptionLayer::create();
    scene->addChild(layer);
    return scene;
}
