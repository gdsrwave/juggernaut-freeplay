// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>
#include "../utils/OptionStr.hpp"

using namespace geode::prelude;

class OptionSelectPopup : public geode::Popup<std::string const&> {
 protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject*) override;
    void onSelectOption(CCObject* object);
    void onQuickImport(CCObject* object);
    void onReload(CCObject* object, bool update = false);
    void onTrash(CCObject* object);
    void onClickFolder(CCObject* object);
    void onInfo(CCObject* object);
    void onToggle(CCObject* object);
    void onAddOptions(CCObject* object);

    Ref<cocos2d::CCArray> m_optionCheckboxes = nullptr;
    Ref<cocos2d::CCArray> m_optionSlots = nullptr;
    std::vector<OptionString> m_optionStrs;
    std::vector<bool> m_checkboxes;
 public:
    static OptionSelectPopup* create(std::string const& text);
    ScrollLayer* m_scrl = nullptr;
};
