// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>
#include "../utils/OptionStr.hpp"

using namespace geode::prelude;

class OptionSelectPopup : public geode::Popup {
 protected:
    bool setup(std::string const& value);
    void onClose(CCObject*) override;
    void onSelectOption(CCObject* object);
    void onCopyOpt(CCObject* object);
    void onQuickImport(CCObject* object);
    void onReload(CCObject* object, bool update = false);
    void onTrash(CCObject* object);
    void onClickFolder(CCObject* object);
    void onInfo(CCObject* object);
    void onToggle(CCObject* object);
    void onAddOptions(CCObject* object);
    void onShuffle(CCObject* object);
    void onToggleShuffle(CCObject* object);

    Ref<cocos2d::CCArray> m_optionCheckboxes = nullptr;
    Ref<cocos2d::CCArray> m_optionSlots = nullptr;
 public:
    static OptionSelectPopup* create(std::string const& text);
    void refreshLayout();
    ScrollLayer* m_scrl = nullptr;
    std::vector<OptionString> m_optionStrs = {};
};
