#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "GravHazOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool GravHazOptPopup::setup(std::string const& value) {
        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Gravity Portal Options");
        auto windowDim = CCDirector::sharedDirector()->getWinSize();

        // SAVE BUTTON
        auto saveBtnMenu = CCMenu::create();
        auto saveBtnS = ButtonSprite::create(
            "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
        saveBtnS->setScale(0.7f);
        auto saveBtn = CCMenuItemSpriteExtra::create(
            saveBtnS, this, menu_selector(GravHazOptPopup::save));

        saveBtnMenu->addChild(saveBtn);
        saveBtnMenu->setPosition({200.f, 30.f});
        m_mainLayer->addChild(saveBtnMenu);

        // GRAVITY PORTAL SPAWNING OPT
        auto portalsMenu = CCMenu::create();
        
        auto portalsTxt = CCLabelBMFont::create("Gravity Portals:", "bigFont.fnt");
        m_portalDiffSelected = CCLabelBMFont::create("wwwwwwwwww", "bigFont.fnt");
        m_portalDiffSelected->setID("jfpopt-gravity-portal-spawn");
        m_portalDiffSelected->setOpacity(150);

        auto portalsChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(GravHazOptPopup::onToggle),
            1.f
        );
        portalsChk->setID("jfpopt-portals-chk");
        portalsChk->toggle(m_portalsToggled);

        auto portalsLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        portalsLASpr->setScale(.7f);
        auto portalsLA = CCMenuItemSpriteExtra::create(
            portalsLASpr, this, menu_selector(GravHazOptPopup::onEnumDecrease)
        );
        portalsLA->setUserObject(m_portalDiffSelected);

        auto portalsRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        portalsRASpr->setFlipX(true);
        portalsRASpr->setScale(.7f);
        auto portalsRA = CCMenuItemSpriteExtra::create(
            portalsRASpr, this, menu_selector(GravHazOptPopup::onEnumIncrease)
        );
        portalsRA->setUserObject(m_portalDiffSelected);

        portalsMenu->setLayout(RowLayout::create()
            ->setGap(25.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        portalsMenu->addChild(portalsTxt);
        portalsMenu->addChild(portalsChk);
        portalsMenu->addChild(portalsLA);
        portalsMenu->addChild(m_portalDiffSelected);
        portalsMenu->addChild(portalsRA);
        portalsMenu->setPosition({20.f, 235.f});
        portalsMenu->setAnchorPoint({0, 0.5});
        portalsMenu->setScale(0.5f);
        portalsMenu->updateLayout();

        std::string gpLabelText = JFPGen::DifficultiesLabel.at(m_gpIndex);
        m_portalDiffSelected->setCString(gpLabelText.c_str());

        // FAKE GRAVITY PORTALS OPT
        auto fakesMenu = CCMenu::create();

        auto fakesTxt = CCLabelBMFont::create("Fake Portals", "bigFont.fnt");

        auto fakesChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(GravHazOptPopup::onToggle),
            1.f
        );
        fakesChk->setID("jfpopt-fake-portals-chk");
        fakesChk->toggle(m_fakesToggled);
        
        fakesMenu->setLayout(RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        fakesMenu->addChild(fakesTxt);
        fakesMenu->addChild(fakesChk);
        fakesMenu->setPosition({20.f, 210.f});
        fakesMenu->setScale(0.41f);
        fakesMenu->setAnchorPoint({0, 0.5});
        fakesMenu->updateLayout();

        // START UPSIDE DOWN OPT
        auto udMenu = CCMenu::create();

        auto udTxt = CCLabelBMFont::create("Start Upside Down", "bigFont.fnt");

        auto udChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(GravHazOptPopup::onToggle),
            1.f
        );
        udChk->setID("jfpopt-upside-down-chk");
        udChk->toggle(m_upsideDownToggled);
        
        udMenu->setLayout(RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        udMenu->addChild(udTxt);
        udMenu->addChild(udChk);
        udMenu->setPosition({210.f, 210.f});
        udMenu->setScale(0.41f);
        udMenu->setAnchorPoint({0, 0.5});
        udMenu->updateLayout();

        // PORTAL INPUTS OPT
        auto pInputsMenu = CCMenu::create();
        
        auto pInputsTxt = CCLabelBMFont::create("Allowed Input Types:", "bigFont.fnt");
        m_portalInputSelected = CCLabelBMFont::create("wwwwwwww", "bigFont.fnt");
        m_portalInputSelected->setID("jfpopt-portal-input-type");

        auto pInputsLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        pInputsLASpr->setScale(.7f);
        auto pInputsLA = CCMenuItemSpriteExtra::create(
            pInputsLASpr, this, menu_selector(GravHazOptPopup::onEnumDecrease)
        );
        pInputsLA->setUserObject(m_portalInputSelected);

        auto pInputsRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        pInputsRASpr->setFlipX(true);
        pInputsRASpr->setScale(.7f);
        auto pInputsRA = CCMenuItemSpriteExtra::create(
            pInputsRASpr, this, menu_selector(GravHazOptPopup::onEnumIncrease)
        );
        pInputsRA->setUserObject(m_portalInputSelected);

        pInputsMenu->setLayout(RowLayout::create()
            ->setGap(25.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        pInputsMenu->addChild(pInputsTxt);
        pInputsMenu->addChild(pInputsLA);
        pInputsMenu->addChild(m_portalInputSelected);
        pInputsMenu->addChild(pInputsRA);
        pInputsMenu->setPosition({20.f, 185.f});
        pInputsMenu->setAnchorPoint({0, 0.5});
        pInputsMenu->setScale(0.5f);
        pInputsMenu->updateLayout();

        std::string piLabelText = JFPGen::PortalInputsLabel.at(m_piIndex);
        m_portalInputSelected->setCString(piLabelText.c_str());

        // PORTALS WITHIN SPAMS OPT
        auto pwsMenu = CCMenu::create();

        auto pwsTxt = CCLabelBMFont::create("Spawn within spams", "bigFont.fnt");

        auto pwsChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(GravHazOptPopup::onToggle),
            1.f
        );
        pwsChk->setID("jfpopt-portals-in-spams-chk");
        pwsChk->toggle(!m_portalsWithinSpams);
        
        pwsMenu->setLayout(RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        pwsMenu->addChild(pwsTxt);
        pwsMenu->addChild(pwsChk);
        pwsMenu->setPosition({20.f, 160.f});
        pwsMenu->setScale(0.41f);
        pwsMenu->setAnchorPoint({0, 0.5});
        pwsMenu->updateLayout();

        m_mainLayer->addChild(portalsMenu);
        m_mainLayer->addChild(fakesMenu);
        m_mainLayer->addChild(udMenu);
        m_mainLayer->addChild(pInputsMenu);
        m_mainLayer->addChild(pwsMenu);

        auto secondTitle = CCLabelBMFont::create("Hazard Options", "goldFont.fnt");
        secondTitle->setPosition({200.f, 130.f});
        secondTitle->setScale(0.7f);
        m_mainLayer->addChild(secondTitle);

        // SPIKE SPAWNING OPT
        auto spikesMenu = CCMenu::create();
        
        auto spikesTxt = CCLabelBMFont::create("Corridor Spikes:", "bigFont.fnt");
        m_spikeWavesizeSelected = CCLabelBMFont::create("wwwwww", "bigFont.fnt");
        m_spikeWavesizeSelected->setID("jfpopt-corridor-spike-spawn");
        m_spikeWavesizeSelected->setOpacity(150);

        auto spikesChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(GravHazOptPopup::onToggle),
            1.f
        );
        spikesChk->setID("jfpopt-corridor-spikes-chk");
        spikesChk->toggle(m_spikesToggled);

        auto spikesLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        spikesLASpr->setScale(.7f);
        auto spikesLA = CCMenuItemSpriteExtra::create(
            spikesLASpr, this, menu_selector(GravHazOptPopup::onEnumDecrease)
        );
        spikesLA->setUserObject(m_spikeWavesizeSelected);

        auto spikesRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        spikesRASpr->setFlipX(true);
        spikesRASpr->setScale(.7f);
        auto spikesRA = CCMenuItemSpriteExtra::create(
            spikesRASpr, this, menu_selector(GravHazOptPopup::onEnumIncrease)
        );
        spikesRA->setUserObject(m_spikeWavesizeSelected);

        spikesMenu->setLayout(RowLayout::create()
            ->setGap(25.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        spikesMenu->addChild(spikesTxt);
        spikesMenu->addChild(spikesChk);
        spikesMenu->addChild(spikesLA);
        spikesMenu->addChild(m_spikeWavesizeSelected);
        spikesMenu->addChild(spikesRA);
        spikesMenu->setPosition({20.f, 100.f});
        spikesMenu->setAnchorPoint({0, 0.5});
        spikesMenu->setScale(0.5f);
        spikesMenu->updateLayout();

        std::string spLabelText = JFPGen::PlacementBySizeLabel.at(m_spIndex);
        m_spikeWavesizeSelected->setCString(spLabelText.c_str());

        // FUZZ OPT
        auto fuzzMenu = CCMenu::create();

        auto fuzzTxt = CCLabelBMFont::create("Fuzzy Spikes", "bigFont.fnt");

        auto fuzzChk = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(GravHazOptPopup::onToggle),
            1.f
        );
        fuzzChk->setID("jfpopt-fuzzy-spikes-chk");
        fuzzChk->toggle(m_fuzzToggled);
        
        fuzzMenu->setLayout(RowLayout::create()
            ->setGap(15.f)
            ->setAxisAlignment(AxisAlignment::Start)
        );
        fuzzMenu->addChild(fuzzTxt);
        fuzzMenu->addChild(fuzzChk);
        fuzzMenu->setPosition({20.f, 75.f});
        fuzzMenu->setScale(0.41f);
        fuzzMenu->setAnchorPoint({0, 0.5});
        fuzzMenu->updateLayout();

        m_mainLayer->addChild(spikesMenu);
        m_mainLayer->addChild(fuzzMenu);
        

        return true;
    }

void GravHazOptPopup::save(CCObject*) {
    auto* mod = Mod::get();

    // mod->setSavedValue<bool>("opt-using-set-seed", m_seedToggled);
    // mod->setSavedValue<uint32_t>("opt-seed", numFromString<int32_t>(m_seedInput->getString()).unwrapOr(0));
    // mod->setSavedValue<int8_t>("opt-corridor-rules", m_crIndex);
    // mod->setSavedValue<int32_t>("opt-length", numFromString<int32_t>(m_lengthInput->getString()).unwrapOr(0));
    // mod->setSavedValue<int16_t>("opt-corridor-height", numFromString<int16_t>(m_chInput->getString()).unwrapOr(0));

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void GravHazOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    log::info("chkID: {}", chkID);

    // if (chkID == "jfpopt-seed-chk") {
    //     m_seedInput->setEnabled(toggled);
    //     m_seedToggled = toggled;
    // } else {
    //     log::info("Unknown toggle: {}", chkID);
    // }
}

void GravHazOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    // if (lbl->getID() == "jfpopt-corridor-rules") {
    //     m_crIndex -= 1;
    //     if (m_crIndex < 0) m_crIndex = m_crIndexLen - 1;
    // } else {
    //     log::info("Unknown toggle: {}", lbl->getID());
    // }

    // std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    // lbl->setCString(crLabelText.c_str());
}

void GravHazOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    // if (lbl->getID() == "jfpopt-corridor-rules") {
    //     m_crIndex += 1;
    //     if (m_crIndex >= m_crIndexLen) m_crIndex = 0;
    // } else {
    //     log::info("Unknown toggle: {}", lbl->getID());
    // }

    // std::string crLabelText = JFPGen::CorridorRulesLabel.at(m_crIndex);
    // lbl->setCString(crLabelText.c_str());
}

GravHazOptPopup* GravHazOptPopup::create(std::string const& text) {
    auto ret = new GravHazOptPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
