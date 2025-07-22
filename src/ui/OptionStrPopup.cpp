// Copyright 2025 GDSRWave
#include <string>
#include "OptionStrPopup.hpp"
#include "../utils/Ninja.hpp"
#include "../utils/OptionStr.hpp"

bool OptionStrPopup::setup(std::string const& value) {
        this->setTitle("Option Code:");

        auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");

        m_inputOptTxt = TextInput::create(
            150.f, "lByh_gWjwIcEQGAK+500", "chatFont.fnt");
        m_inputOptTxt->setID("option-str-input");
        m_inputOptTxt->setScale(1.3f);
        m_inputOptTxt->setFilter(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_+");

        auto importSprite = ButtonSprite::create(
            "Import", "bigFont.fnt", "GJ_button_01.png", 0.8f);
        importSprite->setScale(0.7f);
        auto importButton = CCMenuItemSpriteExtra::create(
            importSprite, this, menu_selector(OptionStrPopup::clickImport));

        auto mainMenu = CCMenu::create();
        mainMenu->setLayout(ColumnLayout::create()
            ->setGap(15.f));
        mainMenu->addChild(importButton);
        mainMenu->addChild(m_inputOptTxt);
        mainMenu->addChild(label);
        mainMenu->setPosition({120.f, 65.f});
        mainMenu->updateLayout();
        m_mainLayer->addChild(mainMenu);

        return true;
    }

void OptionStrPopup::clickImport(CCObject* object) {
    try {
        std::string input = m_inputOptTxt->getString();
        log::info("Importing Theme {}", input);
        importSettings(input);
    } catch (const std::runtime_error& re) {
        FLAlertLayer::create("Error", re.what(), "OK")->show();
    }

    OptionStrPopup::onClose(object);
}

void OptionStrPopup::onClose(CCObject* object) {
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

OptionStrPopup *OptionStrPopup::create(std::string const& text) {
    auto ret = new OptionStrPopup();
    if (ret->initAnchored(240.f, 140.f, text)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

OptionStrPopup::~OptionStrPopup() {
    if (this->getParent() != nullptr) this->removeAllChildrenWithCleanup(true);
}
