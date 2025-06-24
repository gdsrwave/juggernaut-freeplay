#include "JFPMenuLayer.hpp"
#include "Geode/binding/FLAlertLayer.hpp"
#include "Geode/utils/cocos.hpp"

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

bool JFPMenuLayer::init() {
    auto backgroundSprite = CCSprite::create("game_bg_12_001.png");
    backgroundSprite->setID("background"_spr);
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto size = backgroundSprite->getContentSize();
    
    backgroundSprite->setScale(winSize.width / size.width);
    
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setPosition({0,-75});
    
    backgroundSprite->setColor({29, 120, 86});
    
    backgroundSprite->setZOrder(-2);
    addChild(backgroundSprite);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(JFPMenuLayer::onBack)
    );
    backBtn->setID("exit-button"_spr);
    backBtn->setSizeMult(1.2f);

    auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({24, winSize.height - 23});
    menuBack->setID("exit-menu"_spr);

    addChild(menuBack);

    setKeypadEnabled(true);

    return true;
}

void JFPMenuLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void JFPMenuLayer::onBack(CCObject* object) {
    keyBackClicked();
}

CCScene* JFPMenuLayer::scene() {
    auto layer = JFPMenuLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}