// Copyright 2025 GDSRWave
#include <string>
#include "OptionStrPopupDeluxe.hpp"
#include "../utils/Ninja.hpp"
#include "../utils/OptionStr.hpp"
#include "./OptionSelectPopup.hpp"

bool OptionStrPopupDeluxe::setup(std::string const& value) {
        this->setTitle("Add Option Code");

        auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
        
        auto inputNameMenu = CCMenu::create();
        auto inputNameLabel = CCLabelBMFont::create("Name:     ", "bigFont.fnt");
        m_inputNameTxt = TextInput::create(
            200.f, "Mega Options", "bigFont.fnt"
        );
        m_inputNameTxt->setID("option-name-input");
        m_inputNameTxt->setFilter(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789- ");
    
        inputNameMenu->setLayout(RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        inputNameMenu->addChild(inputNameLabel);
        inputNameMenu->addChild(m_inputNameTxt);
        inputNameMenu->setPosition({20.f, 100.f});
        inputNameMenu->setScale(0.45);
        inputNameMenu->setAnchorPoint({0, 0.5});
        inputNameMenu->updateLayout();
        m_inputNameTxt->setScale(1.5f);

        auto inputOptMenu = CCMenu::create();
        auto inputOptLabel = CCLabelBMFont::create("Option Code:    ", "bigFont.fnt");
        m_inputOptTxt = TextInput::create(
            150.f, "mAHh_gWjwIcQACDQAA+400", "chatFont.fnt");
        m_inputOptTxt->setID("option-str-input");
        m_inputOptTxt->setFilter(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_+");
    
        inputOptMenu->setLayout(RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        inputOptMenu->addChild(inputOptLabel);
        inputOptMenu->addChild(m_inputOptTxt);
        inputOptMenu->setPosition({20.f, 65.f});
        inputOptMenu->setScale(0.45);
        inputOptMenu->setAnchorPoint({0, 0.5});
        inputOptMenu->updateLayout();
        m_inputOptTxt->setScale(1.5f);

        // SAVE BUTTON
        auto importBtnMenu = CCMenu::create();
        auto importBtnS = ButtonSprite::create(
            "Add", "bigFont.fnt", "GJ_button_01.png", 1.f);
        auto importBtn = CCMenuItemSpriteExtra::create(
            importBtnS, this, menu_selector(OptionStrPopupDeluxe::clickImport));

        importBtnMenu->addChild(importBtn);
        importBtnMenu->setPosition({m_mainLayer->getContentSize().width / 2.f, 30.f});
        importBtnMenu->updateLayout();
        importBtn->setScale(0.7f);
        m_mainLayer->addChild(importBtnMenu);

        m_mainLayer->addChild(label);
        m_mainLayer->addChild(inputNameMenu);
        m_mainLayer->addChild(inputOptMenu);

        return true;
    }

void OptionStrPopupDeluxe::clickImport(CCObject* object) {

    std::string input = m_inputOptTxt->getString();
    log::info("Adding {}", input);

    auto savedOpts = Mod::get()->getSavedValue<SavedOptionStrs>("user-option-codes", SavedOptionStrs{});
    
    auto it = std::find_if(savedOpts.juggernaut.begin(), savedOpts.juggernaut.end(),
        [this](const OptionString& opt) { return opt.name == m_inputNameTxt->getString(); });
    if (it != savedOpts.juggernaut.end()) {
        FLAlertLayer::create("Error", "A preset with this name already exists.", "OK")->show();
        return;
    }

    auto newOptExport = OptionString { .name = m_inputNameTxt->getString(), .optExported = input};
    savedOpts.juggernaut.push_back(newOptExport);
    Mod::get()->setSavedValue<SavedOptionStrs>("user-option-codes", savedOpts);

    m_optionPopup->refreshLayout();

    OptionStrPopupDeluxe::onClose(object);
}

void OptionStrPopupDeluxe::onClose(CCObject* object) {
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

OptionStrPopupDeluxe *OptionStrPopupDeluxe::create(OptionSelectPopup* popup) {
    auto ret = new OptionStrPopupDeluxe();
    ret->m_optionPopup = popup;

    if (ret->initAnchored(280.f, 150.f, "", "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}