// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class OptionStrPopup : public geode::Popup<std::string const&> {
 protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject*) override;

    TextInput* m_inputOptTxt;

 public:
    static OptionStrPopup* create(std::string const& text);

    void clickImport(CCObject*);
};
