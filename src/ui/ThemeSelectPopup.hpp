// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// specify parameters for the setup function in the Popup<...> template
class ThemeSelectPopup : public geode::Popup<std::string const&> {
 protected:
    bool setup(std::string const& value) override;
    void onClose(CCObject*) override;
    void onSelectTheme(CCObject* object);
 public:
    static ThemeSelectPopup* create(std::string const& text);
};
