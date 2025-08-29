#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CorridorOptPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Corridor Generation Settings");
        log::info("abcd");

        auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
        m_mainLayer->addChildAtPosition(label, Anchor::Center);

        return true;
    }

public:
    static CorridorOptPopup* create(std::string const& text) {
        auto ret = new CorridorOptPopup();
        if (ret->initAnchored(360.f, 240.f, text)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
