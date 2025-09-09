#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class SoundtrackOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject* object) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);

    Mod* mod = Mod::get();

    CCLabelBMFont* m_musicSourceSelected;

    int8_t m_msrcIndex = mod->getSavedValue<int8_t>("opt-0-music-source");
    int8_t m_msrcIndexLen = static_cast<int8_t>(JFPGen::MusicSourceLabel.size());
public:
    static SoundtrackOptPopup* create(std::string const& text);
};
