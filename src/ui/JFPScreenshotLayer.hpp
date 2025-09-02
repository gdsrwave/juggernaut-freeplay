// Copyright 2025 GDSRWave
#pragma once

#include <Geode/Geode.hpp>

#include "JFPGenericLayer.hpp"

using namespace geode::prelude;

class JFPScreenshotLayer : public JFPGenericLayer {
 protected:
    virtual bool init();
    void keyBackClicked();
    void onEnterTransitionDidFinish();
    void onAutoGen();

 public:
    static JFPScreenshotLayer* create();
    static cocos2d::CCScene* scene();
    void onBack(cocos2d::CCObject*);
};
