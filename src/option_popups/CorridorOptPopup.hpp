#pragma once
#include <Geode/Geode.hpp>
#include "../utils/shared.hpp"

using namespace geode::prelude;

class CorridorOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject* object) override;
    void onToggle(CCObject* object);
    void onEnumDecrease(CCObject* object);
    void onEnumIncrease(CCObject* object);
    void save(CCObject*);
    void onInfo(CCObject* object);

    Mod* mod = Mod::get();

    TextInput* m_lengthInput;
    TextInput* m_seedInput;
    TextInput* m_chInput;
    CCLabelBMFont* m_rulesSelected;

    int8_t m_crIndex = mod->getSavedValue<int8_t>("opt-0-corridor-rules");
    int8_t m_crIndexLen = static_cast<int8_t>(JFPGen::CorridorRulesLabel.size());
    bool m_seedToggled = mod->getSavedValue<bool>("opt-0-using-set-seed");
    bool m_wpb = mod->getSavedValue<bool>("opt-0-widen-playfield-bounds");
public:
    static CorridorOptPopup* create(std::string const& text);
};
