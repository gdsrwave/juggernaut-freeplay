#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "CorridorOptPopup.hpp"

using namespace geode::prelude;

bool CorridorOptPopup::setup(std::string const& value) {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Corridor Generation");
        log::info("abcdgfe");
        auto windowDim = CCDirector::sharedDirector()->getWinSize();

        auto saveBtnMenu = CCMenu::create();
        auto saveBtnS = ButtonSprite::create(
            "Save", "bigFont.fnt", "GJ_button_04.png", 1.f);
        saveBtnS->setScale(0.8f);
        auto saveBtn = CCMenuItemSpriteExtra::create(
            saveBtnS, this, menu_selector(CorridorOptPopup::save));

        saveBtnMenu->addChild(saveBtn);
        saveBtnMenu->setPosition({200.f, 30.f});
        m_mainLayer->addChild(saveBtnMenu);

        auto seedMenu = CCMenu::create();

        auto seedTxt = CCLabelBMFont::create("Set Seed:", "bigFont.fnt");
        seedTxt->setScale(0.8f);
        auto seedChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(CorridorOptPopup::onToggle),
            1.f
        );
        seedChk->setID("jfpopt-seed-chk");
        m_seedInput = TextInput::create(
            150.f, "2147483647", "bigFont.fnt"
        );
        m_seedInput->setID("jfpopt-seed-input");
        m_seedInput->setScale(1.f);
        m_seedInput->setEnabled(false);
        m_seedInput->setFilter(
            "0123456789"
        );
        
        seedMenu->setLayout(RowLayout::create()
            ->setGap(5.f));
        seedMenu->addChild(seedTxt);
        seedMenu->addChild(seedChk);
        seedMenu->addChild(m_seedInput);
        seedMenu->setPosition({110.f, 220.f});
        seedMenu->setScale(0.5f);
        seedMenu->updateLayout();
        m_mainLayer->addChild(seedMenu);
        return true;
    }

void CorridorOptPopup::save(CCObject*) {
    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void CorridorOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = chk->isToggled();

    log::info("chkID: {}", chkID);

    if (chkID == "jfpopt-seed-chk") {
        m_seedInput->setEnabled(!toggled);
    } else {
        log::info("Unknown toggle: {}", chkID);
    }
}

CorridorOptPopup* CorridorOptPopup::create(std::string const& text) {
    auto ret = new CorridorOptPopup();
    if (ret->initAnchored(400.f, 280.f, text)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
