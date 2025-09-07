#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class CorridorOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);

    TextInput* m_lengthInput;
    TextInput* m_seedInput;
    TextInput* m_chInput;
    CCLabelBMFont* m_rulesSelected;

    int8_t m_crIndex = Mod::get()->getSavedValue<int8_t>("opt-0-corridor-rules");
    int8_t m_crIndexLen = static_cast<int8_t>(JFPGen::CorridorRulesLabel.size());
    bool m_seedToggled = Mod::get()->getSavedValue<bool>("opt-0-using-set-seed");
    bool m_wpb = Mod::get()->getSavedValue<bool>("opt-0-widen-playfield-bounds");
public:
    static CorridorOptPopup* create(std::string const& text);
};
