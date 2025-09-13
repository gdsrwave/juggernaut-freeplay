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
    void save();
    void onSave(CCObject* object);
    void onInfo(CCObject* object);

    Mod* mod = Mod::get();

    TextInput* m_lengthInput;
    TextInput* m_seedInput;
    TextInput* m_chInput;
    CCLabelBMFont* m_rulesSelected;

    uint8_t m_crIndex = mod->getSavedValue<uint8_t>("opt-0-corridor-rules");
    uint8_t m_crIndexLen = static_cast<uint8_t>(JFPGen::CorridorRulesLabel.size());
    bool m_seedToggled = mod->getSavedValue<bool>("opt-0-using-set-seed");
    bool m_wpb = mod->getSavedValue<bool>("opt-0-widen-playfield-bounds");
public:
    static CorridorOptPopup* create(std::string const& text);
};
