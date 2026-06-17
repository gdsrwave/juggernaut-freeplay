#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class SoundtrackOptPopup : public geode::Popup {
protected:
    bool setup(std::string const& value);
    void onClose(CCObject* object) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void onSave(CCObject* object);
    void save();
    void onInfo(CCObject* object);

    Mod* mod = Mod::get();

    CCLabelBMFont* m_musicSourceSelected;

    uint8_t m_msrcIndex = mod->getSavedValue<uint8_t>("opt-0-music-source");
    bool m_musicOffset = mod->getSavedValue<bool>("opt-0-music-offset");
    bool m_musicLoop = mod->getSavedValue<bool>("opt-0-music-loop");
    uint8_t m_msrcIndexLen = static_cast<uint8_t>(JFPGen::MusicSourceLabel.size());
public:
    static SoundtrackOptPopup* create(std::string const& text);
};
