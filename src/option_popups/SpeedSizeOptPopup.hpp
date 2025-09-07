#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class SpeedSizeOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onToggle(CCObject* object);
    void onEnumSelect(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);

    Mod* mod = Mod::get();

    Ref<cocos2d::CCArray> m_sspeedItems = nullptr;
    Ref<cocos2d::CCArray> m_minSpeedItems = nullptr;
    Ref<cocos2d::CCArray> m_maxSpeedItems = nullptr;

    CCLabelBMFont* m_ssizeSelected;
    CCLabelBMFont* m_tTypeSelected;
    CCMenu* m_schangesMenu;

    int8_t m_sspeedIndex = mod->getSavedValue<int8_t>("opt-0-starting-speed");
    int8_t m_maxSpeedIndex = mod->getSavedValue<int8_t>("opt-0-maximum-speed");
    int8_t m_minSpeedIndex = mod->getSavedValue<int8_t>("opt-0-minimum-speed");
    int8_t m_speedChangesData = mod->getSavedValue<int8_t>("opt-0-speed-changes");
    bool m_changingSpeed = mod->getSavedValue<bool>("opt-0-using-speed-changes");

    int8_t m_ssizeIndex = mod->getSavedValue<int8_t>("opt-0-starting-size");
    int8_t m_ssizeIndexLen = static_cast<int8_t>(JFPGen::StartingSizeLabel.size());
    int8_t m_tTypeIndex = mod->getSavedValue<int8_t>("opt-0-size-transition-type");
    int8_t m_tTypeIndexLen = static_cast<int8_t>(JFPGen::SizeTransitionsLabel.size());
    bool m_changingSize = mod->getSavedValue<bool>("opt-0-using-size-changes");
public:
    static SpeedSizeOptPopup* create(std::string const& text);
};
