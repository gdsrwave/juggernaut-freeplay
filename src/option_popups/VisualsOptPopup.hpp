#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class VisualsOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);

    Mod* mod = Mod::get();

    CCLabelBMFont* m_bgTextureSelected;
    CCLabelBMFont* m_colorModeSelected;
    TextInput* m_markIntInput;

    CCMenu* m_markIntMenu;

    int8_t m_bgTextureIndex = mod->getSavedValue<int8_t>("opt-0-background-texture");
    int8_t m_bgTextureIndexLen = static_cast<int8_t>(JFPGen::MusicSourceLabel.size());
    int8_t m_colorModeIndex = mod->getSavedValue<int8_t>("opt-0-color-mode");
    int8_t m_colorModeIndexLen = static_cast<int8_t>(JFPGen::ColorModeLabel.size());
    bool m_meterMarks = mod->getSavedValue<bool>("opt-0-show-meter-marks");
    bool m_lowVisiblity = mod->getSavedValue<bool>("opt-0-low-visibility");
    bool m_cornerPieces = mod->getSavedValue<bool>("opt-0-add-corner-pieces");
    bool m_finishLine = mod->getSavedValue<bool>("opt-0-show-finish-line");
    bool m_hideIcon = mod->getSavedValue<bool>("opt-0-hide-icon");
public:
    static VisualsOptPopup* create(std::string const& text);
};
