// Copyright 2025 GDSRWave
#include "JFPGenericLayer.hpp"
#include "Geode/binding/FLAlertLayer.hpp"
#include "Geode/utils/cocos.hpp"

JFPGenericLayer* JFPGenericLayer::create() {
    auto ret = new JFPGenericLayer;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool JFPGenericLayer::init(bool defaults) {
    if (defaults) {
        auto backgroundSprite = CCSprite::create("game_bg_12_001.png");
        backgroundSprite->setID("background"_spr);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto size = backgroundSprite->getContentSize();

        backgroundSprite->setScale(winSize.width / size.width);

        backgroundSprite->setAnchorPoint({0, 0});
        backgroundSprite->setPosition({0, -75});

        backgroundSprite->setColor({29, 120, 86});

        backgroundSprite->setZOrder(-3);
        addChild(backgroundSprite);

        auto backBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
            this,
            menu_selector(JFPGenericLayer::onBack));
        backBtn->setID("exit-button"_spr);
        backBtn->setSizeMult(1.2f);

        auto menuBack = CCMenu::create();
        menuBack->addChild(backBtn);
        menuBack->setPosition({24, winSize.height - 23});
        menuBack->setID("exit-menu"_spr);

        addChild(menuBack);
    }


    setKeypadEnabled(true);

    return true;
}

void JFPGenericLayer::keyBackClicked() {
    GameManager::sharedState()->playMenuMusic();
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void JFPGenericLayer::onBack(CCObject* object) {
    keyBackClicked();
}

CCScene* JFPGenericLayer::scene() {
    auto layer = JFPGenericLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void JFPGenericLayer::onEnterTransitionDidFinish() {
    return CCLayer::onEnterTransitionDidFinish();
}
