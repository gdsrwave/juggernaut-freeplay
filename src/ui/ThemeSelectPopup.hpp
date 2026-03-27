// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ThemeSelectPopup : public geode::Popup {
 protected:
    bool setup();
    void onClose(CCObject*) override;
    void onSelectTheme(CCObject* object);
    void onDeselect(CCObject* object);
    void onClickReload(CCObject* object);
    void onReload(CCObject* object, bool update = false);
    void onClickFolder(CCObject* object);
    void onInfo(CCObject* object);

    Ref<cocos2d::CCArray> m_themeUseBtns = nullptr;
    int16_t m_activeThemeIndex = -1;
    std::vector<std::string> m_themeFiles;
 public:
    static ThemeSelectPopup* create();
};
