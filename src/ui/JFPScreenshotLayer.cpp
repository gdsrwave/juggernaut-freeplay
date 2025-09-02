// Copyright 2025 GDSRWave
#include <memory>
#include <string>
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "JFPScreenshotLayer.hpp"
#include "JFPMenuLayer.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/general.hpp>
#include "./OptionStrPopup.hpp"
#include "../utils/shared.hpp"
#include "../utils/StringGen.hpp"
#include "../utils/Ninja.hpp"
#include "../optionPopups/CorridorOptPopup.cpp"


JFPScreenshotLayer* JFPScreenshotLayer::create() {
    auto ret = new JFPScreenshotLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool JFPScreenshotLayer::init() {
    if (!JFPGenericLayer::init(false)) {
        return false;
    }

    log::info("bbb");

    // auto windowDim = CCDirector::sharedDirector()->getWinSize();
    // auto backgroundSprite2 = CCSprite::createWithSpriteFrameName(
    //     "game_bg_27_001_m.png"_spr);
    // backgroundSprite2->setID("background-menu"_spr);

    // auto size = backgroundSprite2->getContentSize();

    // backgroundSprite2->setScale(windowDim.width / size.width);

    // backgroundSprite2->setAnchorPoint({0, 0});
    // backgroundSprite2->setPosition({0, -75});

    // backgroundSprite2->setColor({8, 8, 8});

    // backgroundSprite2->setZOrder(-2);

    auto dir = CCDirector::sharedDirector();
    auto scene = CCScene::create();
    auto size = dir->getWinSize();

    // take screenshot, use as filler during the 1 frame between playlayers
    // without this there would be an annoying black flash for 1 frame
    auto rt = CCRenderTexture::create(size.width, size.height);
    rt->setPosition(size / 2);  // middle of screen
    rt->begin();
    dir->getRunningScene()->visit();
    rt->end();

    addChild(rt);



    return true;
}

void JFPScreenshotLayer::keyBackClicked() {
}

void JFPScreenshotLayer::onBack(CCObject* object) {
}

CCScene* JFPScreenshotLayer::scene() {
    auto scene = CCScene::create();
    auto layer = JFPScreenshotLayer::create();
    scene->addChild(layer);
    return scene;
}

void JFPScreenshotLayer::onAutoGen() {
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
    log::info("z");
    CCDirector::sharedDirector()->replaceScene(newScene);

}

void JFPScreenshotLayer::onEnterTransitionDidFinish() {

    log::info("a");
    JFPGenericLayer::onEnterTransitionDidFinish();
    queueInMainThread([=]() {
        if (state == JFPGen::AutoJFP::NotInAutoJFP) {
            CCDirector::sharedDirector()->replaceScene(JFPMenuLayer::scene());
        } else {
            JFPScreenshotLayer::onAutoGen();
        }
    });
}
