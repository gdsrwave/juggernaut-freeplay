// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>
#include "./OptionSelectPopup.hpp"

using namespace geode::prelude;

class OptionStrPopupDeluxe : public geode::Popup<std::string const&> {
 protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject*) override;

    TextInput* m_inputNameTxt;
    TextInput* m_inputOptTxt;
    OptionSelectPopup* m_optionPopup;

 public:
    static OptionStrPopupDeluxe* create(OptionSelectPopup* popup);

    void clickImport(CCObject*);
};
