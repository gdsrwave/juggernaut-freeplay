// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class ThemeSelectLayer : public CCLayer {
 protected:
    virtual bool init();
    void onBack(cocos2d::CCObject*);
    void onPickTheme(cocos2d::CCObject*);
 public:
    static ThemeSelectLayer* create();
    static cocos2d::CCScene* scene();
};

class ThemeSelectOuterMenu : public GJDropDownLayer {
 protected:
    virtual bool init();
    void onThemeSelected(cocos2d::CCObject*);
 public:
    static ThemeSelectOuterMenu* create();
    void updateThemeList();
    void setSelectedTheme(const std::string& themeName);
    std::string getSelectedTheme() const;
};
