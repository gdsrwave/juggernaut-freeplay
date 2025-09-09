#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "CorridorOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool CorridorOptPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Corridor Options");
    log::info("abcdgfe");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // SAVE BUTTON
    auto saveBtnMenu = CCMenu::create();
    auto saveBtnS = ButtonSprite::create(
        "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
    saveBtnS->setScale(0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveBtnS, this, menu_selector(CorridorOptPopup::save));

    saveBtnMenu->addChild(saveBtn);
    saveBtnMenu->setPosition({200.f, 30.f});
    m_mainLayer->addChild(saveBtnMenu);

    // SET SEED OPT
    auto seedMenu = CCMenu::create();

    auto seedTxt = CCLabelBMFont::create("RNG Set Seed:", "bigFont.fnt");
    bool usingSetSeed = mod->getSavedValue<bool>("opt-0-using-set-seed");
    auto seedChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(CorridorOptPopup::onToggle),
        1.f
    );
    seedChk->setID("jfpopt-seed-chk");
    seedChk->toggle(usingSetSeed);
    std::string seedLoad = numToString(mod->getSavedValue<uint32_t>("opt-0-seed"));
    m_seedInput = TextInput::create(
        150.f,
        seedLoad.size() > 0 ? seedLoad : "2147483647",
        "bigFont.fnt"
    );
    m_seedInput->setString(seedLoad);
    m_seedInput->setID("jfpopt-seed-input");
    m_seedInput->setScale(1.f);
    m_seedInput->setMaxCharCount(10);
    m_seedInput->setEnabled(usingSetSeed);
    m_seedInput->setCommonFilter(CommonFilter::Uint);
    
    seedMenu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    seedMenu->addChild(seedTxt);
    seedMenu->addChild(seedChk);
    seedMenu->addChild(m_seedInput);
    seedMenu->setPosition({20.f, 225.f});
    seedMenu->setScale(0.6f);
    seedMenu->setAnchorPoint({0, 0.5});
    seedMenu->updateLayout();

    seedTxt->setScale(0.8f);

    // CORRIDOR RULES OPT
    auto rulesMenu = CCMenu::create();
    
    auto rulesTxt = CCLabelBMFont::create("Corridor Rules:", "bigFont.fnt");
    m_rulesSelected = CCLabelBMFont::create("wwwwwwwwww", "bigFont.fnt");
    m_rulesSelected->setID("jfpopt-corridor-rules");
    auto rulesLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rulesLASpr->setScale(.7f);
    auto rulesLA = CCMenuItemSpriteExtra::create(
        rulesLASpr, this, menu_selector(CorridorOptPopup::onEnumDecrease)
    );
    rulesLA->setUserObject(m_rulesSelected);
    auto rulesRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rulesRASpr->setFlipX(true);
    rulesRASpr->setScale(.7f);
    auto rulesRA = CCMenuItemSpriteExtra::create(
        rulesRASpr, this, menu_selector(CorridorOptPopup::onEnumIncrease)
    );
    rulesRA->setUserObject(m_rulesSelected);

    rulesMenu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    rulesMenu->addChild(rulesTxt);
    rulesMenu->addChild(rulesLA);
    rulesMenu->addChild(m_rulesSelected);
    rulesMenu->addChild(rulesRA);
    rulesMenu->setPosition({18.f, 195.f});
    rulesMenu->setAnchorPoint({0, 0.5});
    rulesMenu->setScale(0.6f);
    rulesMenu->updateLayout();

    std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    m_rulesSelected->setCString(crLabelText.c_str());
    rulesTxt->setScale(0.8f);
    m_rulesSelected->setScale(0.8f);

    // CORRIDOR LENGTH OPT
    auto lengthMenu = CCMenu::create();

    auto lengthTxt = CCLabelBMFont::create("Length (m):", "bigFont.fnt");
    std::string lengthLoad = numToString(mod->getSavedValue<int32_t>("opt-0-length"));
    m_lengthInput = TextInput::create(
        120.f,
        lengthLoad.size() > 0 ? lengthLoad : "400",
        "bigFont.fnt"
    );
    m_lengthInput->setString(lengthLoad);
    m_lengthInput->setID("jfpopt-length-input");
    m_lengthInput->setScale(1.f);
    m_lengthInput->setMaxCharCount(10);
    m_lengthInput->setCommonFilter(CommonFilter::Uint);
    
    lengthMenu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    lengthMenu->addChild(lengthTxt);
    lengthMenu->addChild(m_lengthInput);
    lengthMenu->setPosition({25.f, 165.f});
    lengthMenu->setScale(0.6f);
    lengthMenu->setAnchorPoint({0, 0.5});
    lengthMenu->updateLayout();

    lengthTxt->setScale(0.8f);

    // CORRIDOR HEIGHT OPT
    auto chMenu = CCMenu::create();

    auto chTxt = CCLabelBMFont::create("Corridor Height (u):", "bigFont.fnt");
    std::string chLoad = numToString(mod->getSavedValue<int16_t>("opt-0-corridor-height"));
    m_chInput = TextInput::create(
        120.f,
        chLoad.size() > 0 ? chLoad : "60",
        "bigFont.fnt"
    );
    m_chInput->setString(chLoad);
    m_chInput->setID("jfpopt-ch-input");
    m_chInput->setScale(1.f);
    m_chInput->setMaxCharCount(10);
    m_chInput->setCommonFilter(CommonFilter::Uint);
    
    chMenu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    chMenu->addChild(chTxt);
    chMenu->addChild(m_chInput);
    chMenu->setPosition({15.f, 135.f});
    chMenu->setScale(0.6f);
    chMenu->setAnchorPoint({0, 0.5});
    chMenu->updateLayout();

    chTxt->setScale(0.8f);

    // WIDEN BOUNDS OPT
    auto wpbMenu = CCMenu::create();

    auto wpbTxt = CCLabelBMFont::create("Widen Playfield Bounds", "bigFont.fnt");

    auto wpbChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(CorridorOptPopup::onToggle),
        1.f
    );
    wpbChk->setID("jfpopt-wide-bounds");
    wpbChk->toggle(m_wpb);
    
    wpbMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    wpbMenu->addChild(wpbTxt);
    wpbMenu->addChild(wpbChk);
    wpbMenu->setPosition({26.f, 105.f});
    wpbMenu->setScale(0.5f);
    wpbMenu->setAnchorPoint({0, 0.5});
    wpbMenu->updateLayout();

    wpbTxt->setScale(0.9f);

    m_mainLayer->addChild(seedMenu);
    m_mainLayer->addChild(rulesMenu);
    m_mainLayer->addChild(lengthMenu);
    m_mainLayer->addChild(chMenu);
    m_mainLayer->addChild(wpbMenu);
    return true;
}

void CorridorOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        CorridorOptPopup::save(object);
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void CorridorOptPopup::save(CCObject*) {
    log::info("{}", m_seedToggled);
    std::string seedStr = m_seedInput->getString();
    if (seedStr.empty()) {
        mod->setSavedValue<bool>("opt-0-using-set-seed", false);
    } else {
        mod->setSavedValue<bool>("opt-0-using-set-seed", m_seedToggled);
        mod->setSavedValue<uint32_t>("opt-0-seed", numFromString<int32_t>(seedStr).unwrapOr(0));
    }
    mod->setSavedValue<bool>("opt-0-widen-playfield-bounds", m_wpb);
    mod->setSavedValue<int8_t>("opt-0-corridor-rules", m_crIndex);
    mod->setSavedValue<int32_t>("opt-0-length", numFromString<int32_t>(m_lengthInput->getString()).unwrapOr(0));
    mod->setSavedValue<int16_t>("opt-0-corridor-height", numFromString<int16_t>(m_chInput->getString()).unwrapOr(0));

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void CorridorOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    log::info("chkID: {}", chkID);

    if (chkID == "jfpopt-seed-chk") {
        m_seedInput->setEnabled(toggled);
        m_seedToggled = toggled;
    } else {
        log::info("Unknown toggle: {}", chkID);
    }
}

void CorridorOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-corridor-rules") {
        m_crIndex -= 1;
        if (m_crIndex < 0) m_crIndex = m_crIndexLen - 1;
    } else {
        log::info("Unknown toggle: {}", lbl->getID());
    }

    std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    lbl->setCString(crLabelText.c_str());
}

void CorridorOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-corridor-rules") {
        m_crIndex += 1;
        if (m_crIndex >= m_crIndexLen) m_crIndex = 0;
    } else {
        log::info("Unknown toggle: {}", lbl->getID());
    }

    std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    lbl->setCString(crLabelText.c_str());
}

CorridorOptPopup* CorridorOptPopup::create(std::string const& text) {
    auto ret = new CorridorOptPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
