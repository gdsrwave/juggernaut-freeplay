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

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(GravHazOptPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

    // GRAVITY PORTAL SPAWNING OPT
    auto portalsMenu = CCMenu::create();
    
    auto portalsTxt = CCLabelBMFont::create("Gravity Portals:", "bigFont.fnt");
    m_portalDiffSelected = CCLabelBMFont::create("wwwwwwwwww", "bigFont.fnt");
    m_portalDiffSelected->setID("jfpopt-gravity-portal-spawn"_spr);
    if (!m_portalsToggled) m_portalDiffSelected->setOpacity(150);

    auto portalsChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(GravHazOptPopup::onToggle),
        1.f
    );
    portalsChk->setID("jfpopt-portals-chk"_spr);
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
    m_fakesMenu = CCMenu::create();

    auto fakesTxt = CCLabelBMFont::create("Fake Portals", "bigFont.fnt");

    auto fakesChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(GravHazOptPopup::onToggle),
        1.f
    );
    fakesChk->setID("jfpopt-fake-portals-chk"_spr);
    fakesChk->toggle(m_fakesToggled);
    
    m_fakesMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    m_fakesMenu->addChild(fakesTxt);
    m_fakesMenu->addChild(fakesChk);
    m_fakesMenu->setPosition({20.f, 210.f});
    m_fakesMenu->setScale(0.41f);
    m_fakesMenu->setAnchorPoint({0, 0.5});
    m_fakesMenu->updateLayout();
    if (!m_portalsToggled) m_fakesMenu->setOpacity(150);

    // START UPSIDE DOWN OPT
    auto udMenu = CCMenu::create();

    auto udTxt = CCLabelBMFont::create("Start Upside Down", "bigFont.fnt");

    auto udChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(GravHazOptPopup::onToggle),
        1.f
    );
    udChk->setID("jfpopt-upside-down-chk"_spr);
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
    m_pInputsMenu = CCMenu::create();
    
    auto pInputsTxt = CCLabelBMFont::create("Allowed Input Types:", "bigFont.fnt");
    m_portalInputSelected = CCLabelBMFont::create("wwwwwwww", "bigFont.fnt");
    m_portalInputSelected->setID("jfpopt-portal-input-type"_spr);

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

    m_pInputsMenu->setLayout(RowLayout::create()
        ->setGap(25.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    m_pInputsMenu->addChild(pInputsTxt);
    m_pInputsMenu->addChild(pInputsLA);
    m_pInputsMenu->addChild(m_portalInputSelected);
    m_pInputsMenu->addChild(pInputsRA);
    m_pInputsMenu->setPosition({20.f, 185.f});
    m_pInputsMenu->setAnchorPoint({0, 0.5});
    m_pInputsMenu->setScale(0.5f);
    m_pInputsMenu->updateLayout();
    if (!m_portalsToggled) m_pInputsMenu->setOpacity(150);

    std::string piLabelText = JFPGen::PortalInputsLabel.at(m_piIndex);
    m_portalInputSelected->setCString(piLabelText.c_str());

    // PORTALS WITHIN SPAMS OPT
    m_pwsMenu = CCMenu::create();

    auto pwsTxt = CCLabelBMFont::create("Spawn within spams", "bigFont.fnt");

    auto pwsChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(GravHazOptPopup::onToggle),
        1.f
    );
    pwsChk->setID("jfpopt-portals-in-spams-chk"_spr);
    pwsChk->toggle(m_portalsWithinSpams);
    
    m_pwsMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    m_pwsMenu->addChild(pwsTxt);
    m_pwsMenu->addChild(pwsChk);
    m_pwsMenu->setPosition({20.f, 160.f});
    m_pwsMenu->setScale(0.41f);
    m_pwsMenu->setAnchorPoint({0, 0.5});
    m_pwsMenu->updateLayout();
    if (!m_portalsToggled) m_pwsMenu->setOpacity(150);

    m_mainLayer->addChild(portalsMenu);
    m_mainLayer->addChild(m_fakesMenu);
    m_mainLayer->addChild(udMenu);
    m_mainLayer->addChild(m_pInputsMenu);
    m_mainLayer->addChild(m_pwsMenu);

    auto secondTitle = CCLabelBMFont::create("Hazard Options", "goldFont.fnt");
    secondTitle->setPosition({200.f, 130.f});
    secondTitle->setScale(0.7f);
    m_mainLayer->addChild(secondTitle);

    // SPIKE SPAWNING OPT
    auto spikesMenu = CCMenu::create();
    
    auto spikesTxt = CCLabelBMFont::create("Corridor Spikes:", "bigFont.fnt");
    m_spikeWavesizeSelected = CCLabelBMFont::create("wwwwww", "bigFont.fnt");
    m_spikeWavesizeSelected->setID("jfpopt-corridor-spike-spawn"_spr);
    if (!m_spikesToggled) m_spikeWavesizeSelected->setOpacity(150);

    auto spikesChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(GravHazOptPopup::onToggle),
        1.f
    );
    spikesChk->setID("jfpopt-corridor-spikes-chk"_spr);
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
    fuzzChk->setID("jfpopt-fuzzy-spikes-chk"_spr);
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

void GravHazOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        GravHazOptPopup::save(object);
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void GravHazOptPopup::save(CCObject*) {
    mod->setSavedValue<bool>("opt-0-using-grav-portals", m_portalsToggled);
    mod->setSavedValue<bool>("opt-0-grav-portal-start", m_upsideDownToggled);
    mod->setSavedValue<bool>("opt-0-fake-grav-portals", m_fakesToggled);
    mod->setSavedValue<bool>("opt-0-remove-portals-in-spams", m_portalsWithinSpams);
    mod->setSavedValue<uint8_t>("opt-0-grav-portals-diff", m_gpIndex);
    mod->setSavedValue<uint8_t>("opt-0-portal-input-types", m_piIndex);

    mod->setSavedValue<bool>("opt-0-using-corridor-spikes", m_spikesToggled);
    mod->setSavedValue<bool>("opt-0-corridor-fuzz", m_fuzzToggled);
    mod->setSavedValue<uint8_t>("opt-0-spike-placement-types", m_spIndex);

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void GravHazOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    if (chkID == "jfpopt-portals-chk"_spr) {
        m_portalsToggled = toggled;
        if (toggled) {
            m_portalDiffSelected->setOpacity(255);
            m_fakesMenu->setOpacity(255);
            m_pInputsMenu->setOpacity(255);
            m_pwsMenu->setOpacity(255);
        } else {
            m_portalDiffSelected->setOpacity(150);
            m_fakesMenu->setOpacity(150);
            m_pInputsMenu->setOpacity(150);
            m_pwsMenu->setOpacity(150);
        }
    } else if (chkID == "jfpopt-fake-portals-chk"_spr) {
        m_fakesToggled = toggled;
    } else if (chkID == "jfpopt-upside-down-chk"_spr) {
        m_upsideDownToggled = toggled;
    } else if (chkID == "jfpopt-portals-in-spams-chk"_spr) {
        m_portalsWithinSpams = toggled;
    } else if (chkID == "jfpopt-corridor-spikes-chk"_spr) {
        m_spikesToggled = toggled;
        if (toggled) {
            m_spikeWavesizeSelected->setOpacity(255);
        } else {
            m_spikeWavesizeSelected->setOpacity(150);
        }
    } else if (chkID == "jfpopt-fuzzy-spikes-chk"_spr) {
        m_fuzzToggled = toggled;
    } else {
        log::warn("Unknown toggle: {}", chkID);
    }
}

void GravHazOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());
    std::string labelText;

    if (lbl->getID() == "jfpopt-gravity-portal-spawn"_spr) {
        if (m_gpIndex == 0) m_gpIndex = m_gpIndexLen;
        m_gpIndex -= 1;
        labelText = JFPGen::DifficultiesLabel.at(m_gpIndex);
    } else if (lbl->getID() == "jfpopt-portal-input-type"_spr) {
        if (m_piIndex == 0) m_piIndex = m_piIndexLen;
        m_piIndex -= 1;
        labelText = JFPGen::PortalInputsLabel.at(m_piIndex);
    } else if (lbl->getID() == "jfpopt-corridor-spike-spawn"_spr) {
        if (m_spIndex == 0) m_spIndex = m_spIndexLen;
        m_spIndex -= 1;
        labelText = JFPGen::PlacementBySizeLabel.at(m_spIndex);
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }

    lbl->setCString(labelText.c_str());
}

void GravHazOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());
    std::string labelText;

    if (lbl->getID() == "jfpopt-gravity-portal-spawn"_spr) {
        m_gpIndex += 1;
        if (m_gpIndex >= m_gpIndexLen) m_gpIndex = 0;
        labelText = JFPGen::DifficultiesLabel.at(m_gpIndex);
    } else if (lbl->getID() == "jfpopt-portal-input-type"_spr) {
        m_piIndex += 1;
        if (m_piIndex >= m_piIndexLen) m_piIndex = 0;
        labelText = JFPGen::PortalInputsLabel.at(m_piIndex);
    } else if (lbl->getID() == "jfpopt-corridor-spike-spawn"_spr) {
        m_spIndex += 1;
        if (m_spIndex >= m_spIndexLen) m_spIndex = 0;
        labelText = JFPGen::PlacementBySizeLabel.at(m_spIndex);
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }

    lbl->setCString(labelText.c_str());
}

void GravHazOptPopup::onInfo(CCObject*) {
    const char* info =
        "#### Options controlling gravity portals\n\n"
        "<cp>Gravity Portals</c>: Enables gravity portals in JFP, with 3 spawning difficulties available\n\n"
        "<co>Fake Portals</c>: Spawn portals not of the opposite gravity\n- Dependent on <cp>Gravity Portals</c>\n\n"
        "<cb>Start Upside Down</c>: Adds a yellow portal to the corridor's entrance\n\n"
        "<cg>Allowed Input Types</c>: Allows only generating certain gameplay situations, such as holding into portals or falling into portals\n- Dependent on <cp>Gravity Portals</c>\n\n"
        "<cy>Spawn Within Spams</c>: Unchecking prevents portals from appearing inside long spam corridors. Good to turn off in Unrestricted CR\n- Dependent on <cp>Gravity Portals</c>\n\n\n"
        "#### Options controlling hazards (spikes)\n\n"
        "<cp>Corridor Spikes</c>: Enables spike spawning on corridor sides. Defaults to both bigwave and miniwave, but allows targeting either.\n\n"
        "<co>Fuzzy Spikes</c>: Enables fuzz";

    auto infoLayer = MDPopup::create("Gravity Portal/Hazard Options Info",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
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
