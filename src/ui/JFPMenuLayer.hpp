#pragma once

#include <Geode/Geode.hpp>

#include "JFPGenericLayer.hpp"

using namespace geode::prelude;

class JFPMenuLayer : public JFPGenericLayer {
protected:
    virtual bool init();
    void onBack(cocos2d::CCObject*);
    void onOptionButton(cocos2d::CCObject*);
    void onAutoGenButton(cocos2d::CCObject*);
    void onThemeButton(cocos2d::CCObject*);
    void onInfoButton(cocos2d::CCObject*);
public:
    static JFPMenuLayer* create();
    static cocos2d::CCScene* scene();
};