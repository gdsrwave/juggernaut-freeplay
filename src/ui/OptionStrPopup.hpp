// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class OptionStrPopup : public geode::Popup {
 protected:
    bool setup();
    void onClose(CCObject*) override;

    TextInput* m_inputOptTxt;

 public:
    static OptionStrPopup* create();

    void clickImport(CCObject*);
};
