#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class JunkDrawerOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject* object) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);

    Mod* mod = Mod::get();

    bool m_lmaoButton = mod->getSettingValue<bool>("lmao-button");
    bool m_wavemanButton = mod->getSavedValue<bool>("opt-u-waveman-button-shown");
    bool m_themeTools = mod->getSavedValue<bool>("opt-u-theme-creator-tools");
    bool m_debug = mod->getSavedValue<bool>("opt-u-debug");
    bool m_autosave = mod->getSavedValue<bool>("opt-u-save-on-close");
public:
    static JunkDrawerOptPopup* create(std::string const& text);
};
