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

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(CorridorOptPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

    // SET SEED OPT
    auto seedMenu = CCMenu::create();

    auto seedTxt = CCLabelBMFont::create("RNG Set Seed:", "bigFont.fnt");
    bool usingSetSeed = mod->getSavedValue<bool>("opt-0-using-set-seed");
    auto seedChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(CorridorOptPopup::onToggle),
        1.f
    );
    seedChk->setID("jfpopt-seed-chk"_spr);
    seedChk->toggle(usingSetSeed);
    std::string seedLoad = numToString(mod->getSavedValue<uint32_t>("opt-0-seed"));
    m_seedInput = TextInput::create(
        180.f,
        seedLoad.size() > 0 ? seedLoad : "2147483647",
        "bigFont.fnt"
    );
    m_seedInput->setString(seedLoad);
    m_seedInput->setID("jfpopt-seed-input"_spr);
    m_seedInput->setScale(1.f);
    m_seedInput->setEnabled(usingSetSeed);
    m_seedInput->setFilter("0123456789");
    m_seedInput->setMaxCharCount(11);

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
    m_rulesSelected->setID("jfpopt-corridor-rules"_spr);
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
    std::string lengthLoad = numToString(mod->getSavedValue<uint32_t>("opt-0-length"));
    m_lengthInput = TextInput::create(
        120.f,
        lengthLoad.size() > 0 ? lengthLoad : "400",
        "bigFont.fnt"
    );
    m_lengthInput->setString(lengthLoad);
    m_lengthInput->setID("jfpopt-length-input"_spr);
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
    std::string chLoad = numToString(mod->getSavedValue<uint8_t>("opt-0-corridor-height"));
    m_chInput = TextInput::create(
        120.f,
        chLoad.size() > 0 ? chLoad : "60",
        "bigFont.fnt"
    );
    m_chInput->setString(chLoad);
    m_chInput->setID("jfpopt-ch-input"_spr);
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
    wpbChk->setID("jfpopt-wide-bounds"_spr);
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
    std::string seedStr = m_seedInput->getString();
    if (seedStr.empty()) {
        mod->setSavedValue<bool>("opt-0-using-set-seed", false);
    } else {
        mod->setSavedValue<bool>("opt-0-using-set-seed", m_seedToggled);
        mod->setSavedValue<uint32_t>("opt-0-seed", numFromString<uint32_t>(seedStr).unwrapOr(0));
    }
    mod->setSavedValue<bool>("opt-0-widen-playfield-bounds", m_wpb);
    mod->setSavedValue<uint8_t>("opt-0-corridor-rules", m_crIndex);
    uint32_t parsedLength = numFromString<uint32_t>(m_lengthInput->getString()).unwrapOr(0);
    if (parsedLength >= 1 && parsedLength <= 100000) mod->setSavedValue<uint32_t>("opt-0-length", parsedLength);
    uint8_t parsedCH = numFromString<uint8_t>(m_chInput->getString()).unwrapOr(0);
    mod->setSavedValue<uint8_t>("opt-0-corridor-height", parsedCH);

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void CorridorOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    log::info("chkID: {}", chkID);

    if (chkID == "jfpopt-seed-chk"_spr) {
        m_seedInput->setEnabled(toggled);
        m_seedToggled = toggled;
    } else if (chkID == "jfpopt-wide-bounds"_spr) {
        m_wpb = toggled;
    } else {
        log::warn("Unknown toggle: {}", chkID);
    }
}

void CorridorOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-corridor-rules"_spr) {
        if (m_crIndex == 0) m_crIndex = m_crIndexLen;
        m_crIndex -= 1;
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }

    std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    lbl->setCString(crLabelText.c_str());
}

void CorridorOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-corridor-rules"_spr) {
        m_crIndex += 1;
        if (m_crIndex >= m_crIndexLen) m_crIndex = 0;
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }

    std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    lbl->setCString(crLabelText.c_str());
}

void CorridorOptPopup::onInfo(CCObject*) {
    const char* info =
        "#### Options related to Juggernaut corridor generation\n\n"
        "<cp>RNG Set Seed</c>: Determines JFP's randomizer pattern. A seed gives the same pattern every time\n\n"
        "<co>Corridor Rules</c>: Rules dictating how the corridor pattern can and can't generate.\n"
        "- NSNZ = \"No Spam, No Zigzagging,\" which prevents some intensive patterns.\n"
        "- LRD = \"Low Respectful Density,\" which removes spam and tends away from doubleclick inputs.\n\n"
        "<cb>Length (m)</c>: Corridor Length in meters (1 meter = 1 block)\n\n"
        "<cg>Corridor Height (u)</c>: Corridor height in units (60u ch = 2 blocks)\n\n"
        "IMPORTANT: By default, JFP actually adds 30 to this corridor height in miniwave. This is due to transitions "
        "between bigwave and miniwave, which are controlled by the <cy>Size Transition Type</c> option.\n\nSee Speed/Wave Size Options for more information.\n\n"
        "<cy>Widen Playfield Bounds</c>: Extends corridor boundaries by 1 block on the top/bottom";

    auto infoLayer = MDPopup::create("Corridor Options Info",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
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
