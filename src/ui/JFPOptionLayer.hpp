// Copyright 2025 GDSRWave
#pragma once

#include <Geode/Geode.hpp>

#include "JFPGenericLayer.hpp"

using namespace geode::prelude;

class JFPOptionLayer : public JFPGenericLayer {
 protected:
    virtual bool init();
    void keyBackClicked();

    CCLabelBMFont* m_optText;
 public:
    static JFPOptionLayer* create();
    static cocos2d::CCScene* scene();
    void onBack(cocos2d::CCObject*);
    void onReset(cocos2d::CCObject*);
    void onOptionsLaunch(cocos2d::CCObject*);
};
