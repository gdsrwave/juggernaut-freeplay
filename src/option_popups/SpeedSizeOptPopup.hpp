#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class SpeedSizeOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject* object) override;
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
    CCLabelBMFont* m_speedDiffSelected;
    CCMenu* m_schangesMenu;

    uint8_t m_sspeedIndex = mod->getSavedValue<uint8_t>("opt-0-starting-speed");
    uint8_t m_maxSpeedIndex = mod->getSavedValue<uint8_t>("opt-0-maximum-speed");
    uint8_t m_minSpeedIndex = mod->getSavedValue<uint8_t>("opt-0-minimum-speed");
    uint8_t m_cspeedIndex = mod->getSavedValue<uint8_t>("opt-0-speed-changes-diff");
    uint8_t m_cspeedIndexLen = static_cast<uint8_t>(JFPGen::StartingSizeLabel.size());
    uint8_t m_speedChangesData = mod->getSavedValue<uint8_t>("opt-0-speed-changes");
    bool m_changingSpeed = mod->getSavedValue<bool>("opt-0-using-speed-changes");

    uint8_t m_ssizeIndex = mod->getSavedValue<uint8_t>("opt-0-starting-size");
    uint8_t m_ssizeIndexLen = static_cast<uint8_t>(JFPGen::StartingSizeLabel.size());
    uint8_t m_tTypeIndex = mod->getSavedValue<uint8_t>("opt-0-size-transition-type");
    uint8_t m_tTypeIndexLen = static_cast<uint8_t>(JFPGen::SizeTransitionsLabel.size());
    bool m_changingSize = mod->getSavedValue<bool>("opt-0-using-size-changes");
public:
    static SpeedSizeOptPopup* create(std::string const& text);
};
