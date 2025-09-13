// Copyright 2025 GDSRWave
#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class JFPGenericLayer : public cocos2d::CCLayer {
 protected:
    virtual bool init(bool defaults = true);
    virtual void keyBackClicked();
    void onEnterTransitionDidFinish();
    void onBack(cocos2d::CCObject*);
 public:
    static JFPGenericLayer* create();
    static cocos2d::CCScene* scene();
};
