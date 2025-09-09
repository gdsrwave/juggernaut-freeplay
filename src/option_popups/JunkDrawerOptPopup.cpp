#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "JunkDrawerOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool JunkDrawerOptPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Junk Drawer");
    log::info("{}", m_debug);
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // SAVE BUTTON
    auto saveBtnMenu = CCMenu::create();
    auto saveBtnS = ButtonSprite::create(
        "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
    saveBtnS->setScale(0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveBtnS, this, menu_selector(JunkDrawerOptPopup::save));

    saveBtnMenu->addChild(saveBtn);
    saveBtnMenu->setPosition({200.f, 30.f});
    m_mainLayer->addChild(saveBtnMenu);

    // LMAO BUTTON OPT
    auto lmaoBtnMenu = CCMenu::create();

    auto lmaoBtnTxt = CCLabelBMFont::create("LMAO Button", "bigFont.fnt");

    auto lmaoBtnChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(JunkDrawerOptPopup::onToggle),
        1.f
    );
    lmaoBtnChk->setID("jfpopt-lmao-chk");
    lmaoBtnChk->toggle(m_lmaoButton);
    
    lmaoBtnMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    lmaoBtnMenu->addChild(lmaoBtnTxt);
    lmaoBtnMenu->addChild(lmaoBtnChk);
    lmaoBtnMenu->setPosition({20.f, 225.f});
    lmaoBtnMenu->setScale(0.41f);
    lmaoBtnMenu->setAnchorPoint({0, 0.5});

    // WAVEMAN BUTTON OPT
    auto wavemanBtnMenu = CCMenu::create();

    auto wavemanBtnTxt = CCLabelBMFont::create("Waveman Button", "bigFont.fnt");

    auto wavemanBtnChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(JunkDrawerOptPopup::onToggle),
        1.f
    );
    wavemanBtnChk->setID("jfpopt-waveman-chk");
    wavemanBtnChk->toggle(m_wavemanButton);
    
    wavemanBtnMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    wavemanBtnMenu->addChild(wavemanBtnTxt);
    wavemanBtnMenu->addChild(wavemanBtnChk);
    wavemanBtnMenu->setPosition({20.f, 200.f});
    wavemanBtnMenu->setScale(0.41f);
    wavemanBtnMenu->setAnchorPoint({0, 0.5});
    wavemanBtnMenu->updateLayout();
    lmaoBtnMenu->updateLayout();

    // THEME CREATOR TOOLS OPT
    auto themeToolsMenu = CCMenu::create();

    auto themeToolsTxt = CCLabelBMFont::create("Theme Creator Tools", "bigFont.fnt");

    auto themeToolsChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(JunkDrawerOptPopup::onToggle),
        1.f
    );
    themeToolsChk->setID("jfpopt-theme-tools-chk");
    themeToolsChk->toggle(m_themeTools);
    
    themeToolsMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    themeToolsMenu->addChild(themeToolsTxt);
    themeToolsMenu->addChild(themeToolsChk);
    themeToolsMenu->setPosition({20.f, 175.f});
    themeToolsMenu->setScale(0.41f);
    themeToolsMenu->setAnchorPoint({0, 0.5});
    themeToolsMenu->updateLayout();

    // DEBUG OPT
    auto debugMenu = CCMenu::create();

    auto debugTxt = CCLabelBMFont::create("Debug", "bigFont.fnt");

    auto debugChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(JunkDrawerOptPopup::onToggle),
        1.f
    );
    debugChk->setID("jfpopt-debug-chk");
    debugChk->toggle(m_debug);
    
    debugMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    debugMenu->addChild(debugTxt);
    debugMenu->addChild(debugChk);
    debugMenu->setPosition({20.f, 75.f});
    debugMenu->setScale(0.41f);
    debugMenu->setAnchorPoint({0, 0.5});
    debugMenu->updateLayout();

    // AUTOSAVE OPT
    auto autoSaveMenu = CCMenu::create();

    auto autoSaveTxt = CCLabelBMFont::create("Autosave on Close", "bigFont.fnt");

    auto autoSaveChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(JunkDrawerOptPopup::onToggle),
        1.f
    );
    autoSaveChk->setID("jfpopt-autosave-chk");
    autoSaveChk->toggle(m_autosave);
    
    autoSaveMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    autoSaveMenu->addChild(autoSaveTxt);
    autoSaveMenu->addChild(autoSaveChk);
    autoSaveMenu->setPosition({20.f, 150.f});
    autoSaveMenu->setScale(0.41f);
    autoSaveMenu->setAnchorPoint({0, 0.5});
    autoSaveMenu->updateLayout();

    m_mainLayer->addChild(lmaoBtnMenu);
    m_mainLayer->addChild(wavemanBtnMenu);
    m_mainLayer->addChild(themeToolsMenu);
    m_mainLayer->addChild(debugMenu);
    m_mainLayer->addChild(autoSaveMenu);
    

    return true;
}

void JunkDrawerOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        JunkDrawerOptPopup::save(object);
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void JunkDrawerOptPopup::save(CCObject*) {
    mod->setSettingValue<bool>("lmao-button", m_lmaoButton);
    mod->setSavedValue<bool>("opt-u-waveman-button-shown", m_wavemanButton);
    mod->setSavedValue<bool>("opt-u-theme-creator-tools", m_themeTools);
    mod->setSavedValue<bool>("opt-u-debug", m_debug);
    mod->setSavedValue<bool>("opt-u-save-on-close", m_autosave);
    log::info("saved {}", m_debug);

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void JunkDrawerOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    if (chkID == "jfpopt-lmao-chk") {
        m_lmaoButton = toggled;
    } else if (chkID == "jfpopt-waveman-chk") {
        m_wavemanButton = toggled;
    } else if (chkID == "jfpopt-theme-tools-chk") {
        m_themeTools = toggled;
    } else if (chkID == "jfpopt-debug-chk") {
        m_debug = toggled;
    } else if (chkID == "jfpopt-autosave-chk") {
        m_autosave = toggled;
    } else {
        log::info("Unknown toggle: {}", chkID);
    }
}

JunkDrawerOptPopup* JunkDrawerOptPopup::create(std::string const& text) {
    auto ret = new JunkDrawerOptPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
