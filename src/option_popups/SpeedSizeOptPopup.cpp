#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "SpeedSizeOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool SpeedSizeOptPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Speed Options");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // SAVE BUTTON
    auto saveBtnMenu = CCMenu::create();
    auto saveBtnS = ButtonSprite::create(
        "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
    saveBtnS->setScale(0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveBtnS, this, menu_selector(SpeedSizeOptPopup::save));

    saveBtnMenu->addChild(saveBtn);
    saveBtnMenu->setPosition({200.f, 30.f});
    m_mainLayer->addChild(saveBtnMenu);

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(SpeedSizeOptPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

    // STARTING SPEED OPT
    auto sspeedMenu = CCMenu::create();
    auto sspeedTxt = CCLabelBMFont::create("Starting Speed:", "bigFont.fnt");
    sspeedTxt->setID("jfpopt-starting-speed");
    sspeedMenu->setLayout(RowLayout::create()
        ->setGap(45.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    sspeedMenu->addChild(sspeedTxt);

    // Yes, I know about loops and arrays, believe it or not... -M
    m_sspeedItems = CCArray::create();
    for (int i = 0; i < 5; i++) {
        auto sspeedSpr = CCSprite::createWithSpriteFrameName(fmt::format("boost_0{}_001.png", i + 1).c_str());
        sspeedSpr->setScale(1.4f);
        auto sspeedItem = CCMenuItemSpriteExtra::create(
            sspeedSpr, this, menu_selector(SpeedSizeOptPopup::onEnumSelect)
        );
        sspeedItem->setUserObject(sspeedTxt);
        sspeedItem->setTag(i);
        if (m_sspeedIndex != i) sspeedItem->setOpacity(127);
        m_sspeedItems->addObject(sspeedItem);
        sspeedMenu->addChild(sspeedItem);
    }

    sspeedMenu->setPosition({20.f, 225.f});
    sspeedMenu->setAnchorPoint({0, 0.5});
    sspeedMenu->setScale(0.6f);
    sspeedMenu->updateLayout();

    // SPAWN SPEEDCHANGES OPT
    auto cspeedMenu = CCMenu::create();

    auto cspeedTxt = CCLabelBMFont::create("Speed Changes", "bigFont.fnt");

    auto cspeedChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(SpeedSizeOptPopup::onToggle),
        1.f
    );
    cspeedChk->setID("jfpopt-changing-speed-chk");
    cspeedChk->toggle(m_changingSpeed);

    m_speedDiffSelected = CCLabelBMFont::create("wwwwwwwwww", "bigFont.fnt");
    m_speedDiffSelected->setID("jfpopt-speed-change-diff");
    if (!m_changingSpeed) m_speedDiffSelected->setOpacity(150);

    auto cspeedLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    cspeedLASpr->setScale(.7f);
    auto cspeedLA = CCMenuItemSpriteExtra::create(
        cspeedLASpr, this, menu_selector(SpeedSizeOptPopup::onEnumDecrease)
    );
    cspeedLA->setUserObject(m_speedDiffSelected);

    auto cspeedRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    cspeedRASpr->setFlipX(true);
    cspeedRASpr->setScale(.7f);
    auto cspeedRA = CCMenuItemSpriteExtra::create(
        cspeedRASpr, this, menu_selector(SpeedSizeOptPopup::onEnumIncrease)
    );
    cspeedRA->setUserObject(m_speedDiffSelected);
    
    cspeedMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    cspeedMenu->addChild(cspeedTxt);
    cspeedMenu->addChild(cspeedChk);
    cspeedMenu->addChild(cspeedLA);
    cspeedMenu->addChild(m_speedDiffSelected);
    cspeedMenu->addChild(cspeedRA);
    cspeedMenu->setPosition({20.f, 195.f});
    cspeedMenu->setScale(0.5);
    cspeedMenu->setAnchorPoint({0, 0.5});
    cspeedMenu->updateLayout();

    std::string cspeedLabelText = JFPGen::DifficultiesLabel.at(m_cspeedIndex);
    m_speedDiffSelected->setCString(cspeedLabelText.c_str());

    // MIN SPEED OPT
    m_schangesMenu = CCMenu::create();
    auto minSpeedTxt = CCLabelBMFont::create("Selected Speeds:", "bigFont.fnt");
    minSpeedTxt->setID("jfpopt-selected-speeds");
    m_schangesMenu->setLayout(RowLayout::create()
        ->setGap(45.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    m_schangesMenu->addChild(minSpeedTxt);

    m_minSpeedItems = CCArray::create();
    for (int i = 0; i < 5; i++) {
        auto minSpeedSpr = CCSprite::createWithSpriteFrameName(fmt::format("boost_0{}_001.png", i + 1).c_str());
        minSpeedSpr->setScale(1.4f);
        auto minSpeedItem = CCMenuItemSpriteExtra::create(
            minSpeedSpr, this, menu_selector(SpeedSizeOptPopup::onEnumSelect)
        );
        minSpeedItem->setUserObject(minSpeedTxt);
        minSpeedItem->setTag(i);
        if (((m_speedChangesData >> i) & 1) == 0) minSpeedItem->setOpacity(127);
        m_minSpeedItems->addObject(minSpeedItem);
        m_schangesMenu->addChild(minSpeedItem);
    }

    m_schangesMenu->setPosition({20.f, 165.f});
    m_schangesMenu->setAnchorPoint({0, 0.5});
    m_schangesMenu->setScale(0.6f);
    m_schangesMenu->updateLayout();
    if (!m_changingSpeed) m_schangesMenu->setOpacity(127);

    m_mainLayer->addChild(sspeedMenu);
    m_mainLayer->addChild(cspeedMenu);
    m_mainLayer->addChild(m_schangesMenu);

    auto secondTitle = CCLabelBMFont::create("Wave Size Options", "goldFont.fnt");
    secondTitle->setPosition({200.f, 130.f});
    secondTitle->setScale(0.7f);
    m_mainLayer->addChild(secondTitle);

    // SPAWN SIZEPORTALS OPT
    auto csizeMenu = CCMenu::create();

    auto csizeTxt = CCLabelBMFont::create("Spawn Size Portals", "bigFont.fnt");

    auto csizeChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(SpeedSizeOptPopup::onToggle),
        1.f
    );
    csizeChk->setID("jfpopt-changing-size-chk");
    csizeChk->toggle(m_changingSize);
    
    csizeMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    csizeMenu->addChild(csizeTxt);
    csizeMenu->addChild(csizeChk);
    csizeMenu->setPosition({225.f, 105.f});
    csizeMenu->setScale(0.41f);
    csizeMenu->setAnchorPoint({0, 0.5});
    csizeMenu->updateLayout();

    // STARTING SIZE OPT
    auto ssizeMenu = CCMenu::create();
    
    auto ssizeTxt = CCLabelBMFont::create("Start as:", "bigFont.fnt");
    m_ssizeSelected = CCLabelBMFont::create("wwwwww", "bigFont.fnt");
    m_ssizeSelected->setID("jfpopt-starting-size");

    auto ssizeLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    ssizeLASpr->setScale(.7f);
    auto ssizeLA = CCMenuItemSpriteExtra::create(
        ssizeLASpr, this, menu_selector(SpeedSizeOptPopup::onEnumDecrease)
    );
    ssizeLA->setUserObject(m_ssizeSelected);

    auto ssizeRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    ssizeRASpr->setFlipX(true);
    ssizeRASpr->setScale(.7f);
    auto ssizeRA = CCMenuItemSpriteExtra::create(
        ssizeRASpr, this, menu_selector(SpeedSizeOptPopup::onEnumIncrease)
    );
    ssizeRA->setUserObject(m_ssizeSelected);

    ssizeMenu->setLayout(RowLayout::create()
        ->setGap(20.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    ssizeMenu->addChild(ssizeTxt);
    ssizeMenu->addChild(ssizeLA);
    ssizeMenu->addChild(m_ssizeSelected);
    ssizeMenu->addChild(ssizeRA);
    ssizeMenu->setPosition({20.f, 105.f});
    ssizeMenu->setAnchorPoint({0, 0.5});
    ssizeMenu->setScale(0.41f);
    ssizeMenu->updateLayout();

    std::string ssizeLabelText = JFPGen::StartingSizeLabel.at(m_ssizeIndex);
    m_ssizeSelected->setCString(ssizeLabelText.c_str());

    // TRANSITION TYPE OPT
    auto tTypeMenu = CCMenu::create();
    
    auto tTypeTxt = CCLabelBMFont::create("Size transition type:", "bigFont.fnt");
    m_tTypeSelected = CCLabelBMFont::create("wwwww", "bigFont.fnt");
    m_tTypeSelected->setID("jfpopt-transition-type");

    auto tTypeLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    tTypeLASpr->setScale(.7f);
    auto tTypeLA = CCMenuItemSpriteExtra::create(
        tTypeLASpr, this, menu_selector(SpeedSizeOptPopup::onEnumDecrease)
    );
    tTypeLA->setUserObject(m_tTypeSelected);

    auto tTypeRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    tTypeRASpr->setFlipX(true);
    tTypeRASpr->setScale(.7f);
    auto tTypeRA = CCMenuItemSpriteExtra::create(
        tTypeRASpr, this, menu_selector(SpeedSizeOptPopup::onEnumIncrease)
    );
    tTypeRA->setUserObject(m_tTypeSelected);

    tTypeMenu->setLayout(RowLayout::create()
        ->setGap(25.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    tTypeMenu->addChild(tTypeTxt);
    tTypeMenu->addChild(tTypeLA);
    tTypeMenu->addChild(m_tTypeSelected);
    tTypeMenu->addChild(tTypeRA);
    tTypeMenu->setPosition({20.f, 80.f});
    tTypeMenu->setAnchorPoint({0, 0.5});
    tTypeMenu->setScale(0.41f);
    tTypeMenu->updateLayout();

    std::string tTypeLabelText = JFPGen::SizeTransitionsLabel.at(m_tTypeIndex);
    m_tTypeSelected->setCString(tTypeLabelText.c_str());

    m_mainLayer->addChild(csizeMenu);
    m_mainLayer->addChild(ssizeMenu);
    m_mainLayer->addChild(tTypeMenu);

    return true;
}

void SpeedSizeOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        SpeedSizeOptPopup::save(object);
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void SpeedSizeOptPopup::save(CCObject*) {
    mod->setSavedValue<uint8_t>("opt-0-starting-speed", m_sspeedIndex);
    mod->setSavedValue<uint8_t>("opt-0-speed-changes", m_speedChangesData);
    mod->setSavedValue<bool>("opt-0-using-speed-changes", m_changingSpeed);
    mod->setSavedValue<bool>("opt-0-using-size-changes", m_changingSize);
    mod->setSavedValue<uint8_t>("opt-0-starting-size", m_ssizeIndex);
    mod->setSavedValue<uint8_t>("opt-0-size-transition-type", m_tTypeIndex);
    mod->setSavedValue<uint8_t>("opt-0-speed-changes-diff", m_cspeedIndex);

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void SpeedSizeOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    if (chkID == "jfpopt-changing-speed-chk") {
        m_changingSpeed = toggled;
        if (toggled) {
            m_schangesMenu->setOpacity(255);
            m_speedDiffSelected->setOpacity(255);
        } else {
            m_schangesMenu->setOpacity(127);
            m_speedDiffSelected->setOpacity(150);
        }
    } else if (chkID == "jfpopt-changing-size-chk") {
        m_changingSize = toggled;
    } else {
        log::info("Unknown toggle: {}", chkID);
    }
}

void SpeedSizeOptPopup::onEnumSelect(CCObject* object) {
    log::info("{}", m_sspeedItems->count());
    auto selection = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    std::string titleID = typeinfo_cast<CCLabelBMFont*>(selection->getUserObject())->getID();

    if (titleID == "jfpopt-starting-speed") {
        CCObject* obj2;
        CCARRAY_FOREACH(m_sspeedItems, obj2) {
            if (obj2 == nullptr) continue;
            static_cast<CCMenuItemSpriteExtra*>(obj2)->setOpacity(127);
        }
        m_sspeedIndex = selection->getTag();
        selection->setOpacity(255);
        log::info("ssid: {}", m_sspeedIndex);
    } else if (titleID == "jfpopt-selected-speeds") {
        int8_t selectedIndex = selection->getTag();
        bool speedEnabled = ((m_speedChangesData >> selectedIndex) & 1);
        if (speedEnabled) {
            m_speedChangesData &= ~(1 << selectedIndex);
            selection->setOpacity(127);
        } else {
            m_speedChangesData |= (1 << selectedIndex);
            selection->setOpacity(255);
        }
    } else {
        log::info("Unknown toggle: {}", titleID);
    }
}

void SpeedSizeOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());
    std::string labelText;

    if (lbl->getID() == "jfpopt-starting-size") {
        if (m_ssizeIndex == 0) m_ssizeIndex = m_ssizeIndexLen;
        m_ssizeIndex -= 1;
        labelText = JFPGen::StartingSizeLabel.at(m_ssizeIndex);
    } else if (lbl->getID() == "jfpopt-transition-type") {
        if (m_tTypeIndex == 0) m_tTypeIndex = m_tTypeIndexLen;
        m_tTypeIndex -= 1;
        labelText = JFPGen::SizeTransitionsLabel.at(m_tTypeIndex);
    } else if (lbl->getID() == "jfpopt-speed-change-diff") {
        if (m_cspeedIndex == 0) m_cspeedIndex = m_cspeedIndexLen;
        m_cspeedIndex -= 1;
        labelText = JFPGen::DifficultiesLabel.at(m_cspeedIndex);
    } else {
        log::info("Unknown toggle: {}", lbl->getID());
    }

    lbl->setCString(labelText.c_str());
}

void SpeedSizeOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());
    std::string labelText;

    if (lbl->getID() == "jfpopt-starting-size") {
        m_ssizeIndex += 1;
        if (m_ssizeIndex >= m_ssizeIndexLen) m_ssizeIndex = 0;
        labelText = JFPGen::StartingSizeLabel.at(m_ssizeIndex);
    } else if (lbl->getID() == "jfpopt-transition-type") {
        m_tTypeIndex += 1;
        if (m_tTypeIndex >= m_tTypeIndexLen) m_tTypeIndex = 0;
        labelText = JFPGen::SizeTransitionsLabel.at(m_tTypeIndex);
    } else if (lbl->getID() == "jfpopt-speed-change-diff") {
        m_cspeedIndex += 1;
        if (m_cspeedIndex >= m_cspeedIndexLen) m_cspeedIndex = 0;
        labelText = JFPGen::DifficultiesLabel.at(m_cspeedIndex);
    } else {
        log::info("Unknown toggle: {}", lbl->getID());
    }

    lbl->setCString(labelText.c_str());
}

void SpeedSizeOptPopup::onInfo(CCObject*) {
    const char* info =
        "#### Options controlling gameplay speed\n\n"
        "<cp>Starting Speed</c>: Controls initial speed present at the corridor entrance\n\n"
        "<co>Speed Changes</c>: Enables speed changes in JFP, with 3 spawning difficulties available\n\n"
        "<cb>Selected Speeds</c>: Speed changes that can spawn in JFP (multi-select)\n- Dependent on <co>Speed Changes</c>\n\n\n"
        "#### Options controlling wave size\n\n"
        "<cp>Start As</c>: The size (big or miniwave) at which the JFP corridors start\n\n"
        "<cg>Spawn Size Portals</c>: Enables size portal spawning in JFP\n\n"
        "<cy>Size Transition Type</c>: Controls the transition types between bigwave and miniwave.\n"
        "- Type A spawns size portals diagonally and adds 1 block of height to miniwave, as seen in **Random Wave**\n"
        "- Type B spawns size portals upright and adds no extra miniwave height, as seen in **CorroZ**";

    auto infoLayer = MDPopup::create("Speed/Wave Size Options Info",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
}

SpeedSizeOptPopup* SpeedSizeOptPopup::create(std::string const& text) {
    auto ret = new SpeedSizeOptPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
