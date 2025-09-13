// Copyright 2025 GDSRWave
#pragma once

#include <Geode/Geode.hpp>

#include "JFPGenericLayer.hpp"

using namespace geode::prelude;

GJGameLevel* createGameLevel();

class JFPMenuLayer : public JFPGenericLayer {
 protected:
    virtual bool init();
    void keyBackClicked();
    void onEnterTransitionDidFinish();

    CCLabelBMFont* m_optText;
 public:
    static JFPMenuLayer* create();
    static cocos2d::CCScene* scene();
    void onBack(cocos2d::CCObject*);
    void onOptionButton(cocos2d::CCObject*);
    void onAutoGenButton(cocos2d::CCObject*);
    void onAutoGen();
    CCScene* onAutoGenScene();
    void onThemeButton(cocos2d::CCObject*);
    void onInfoButton(cocos2d::CCObject*);
    void onImportButton(cocos2d::CCObject*);
    void onGarageButton(cocos2d::CCObject*);
    void onTwitterButton(cocos2d::CCObject*);
    void onCopySeed(cocos2d::CCObject*);
    void onCopyOpt(cocos2d::CCObject*);
    void onOptRefresh(cocos2d::CCObject*);
    void openOptions(cocos2d::CCObject*);
};
