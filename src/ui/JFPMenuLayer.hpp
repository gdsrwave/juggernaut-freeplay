#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class JFPMenuLayer : public cocos2d::CCLayer {
protected:
    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
public:
    static JFPMenuLayer* create();
    static cocos2d::CCScene* scene();
};