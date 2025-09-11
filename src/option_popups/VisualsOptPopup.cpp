#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "VisualsOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool VisualsOptPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Visual Options");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // SAVE BUTTON
    auto saveBtnMenu = CCMenu::create();
    auto saveBtnS = ButtonSprite::create(
        "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
    saveBtnS->setScale(0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveBtnS, this, menu_selector(VisualsOptPopup::save));

    saveBtnMenu->addChild(saveBtn);
    saveBtnMenu->setPosition({200.f, 30.f});
    m_mainLayer->addChild(saveBtnMenu);

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(VisualsOptPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

    // METERS OPT
    auto metersMenu = CCMenu::create();

    auto metersTxt = CCLabelBMFont::create("Show Meter Marks", "bigFont.fnt");

    auto metersChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(VisualsOptPopup::onToggle),
        1.f
    );
    metersChk->setID("jfpopt-meter-marks-chk"_spr);
    metersChk->toggle(m_meterMarks);
    
    metersMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    metersMenu->addChild(metersTxt);
    metersMenu->addChild(metersChk);
    metersMenu->setPosition({20.f, 235.f});
    metersMenu->setScale(0.41f);
    metersMenu->setAnchorPoint({0, 0.5});
    metersMenu->updateLayout();

    // MARK INTERVAL OPT
    m_markIntMenu = CCMenu::create();
    auto markIntTxt = CCLabelBMFont::create("Mark Interval (m):", "bigFont.fnt");
    std::string markIntLoad = numToString(Mod::get()->getSavedValue<uint32_t>("opt-0-mark-interval"));
    m_markIntInput = TextInput::create(
        100.f,
        markIntLoad.size() > 0 ? markIntLoad : "100",
        "bigFont.fnt"
    );
    m_markIntInput->setString(markIntLoad);
    m_markIntInput->setID("jfpopt-mark-interval-input"_spr);
    m_markIntInput->setScale(1.f);
    m_markIntInput->setMaxCharCount(10);
    m_markIntInput->setCommonFilter(CommonFilter::Uint);
    
    m_markIntMenu->setLayout(RowLayout::create()
        ->setGap(0.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    m_markIntMenu->addChild(markIntTxt);
    m_markIntMenu->addChild(m_markIntInput);
    m_markIntMenu->setPosition({5.f, 215.f});
    m_markIntMenu->setScale(0.5f);
    m_markIntMenu->setAnchorPoint({0, 0.5});
    m_markIntMenu->updateLayout();
    if(!m_meterMarks) m_markIntMenu->setOpacity(127);

    markIntTxt->setScale(0.82f);

    // LOW VIS OPT
    auto lowVisMenu = CCMenu::create();

    auto lowVisTxt = CCLabelBMFont::create("Low Visibility", "bigFont.fnt");

    auto lowVisChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(VisualsOptPopup::onToggle),
        1.f
    );
    lowVisChk->setID("jfpopt-low-visibility-chk"_spr);
    lowVisChk->toggle(m_lowVisiblity);
    
    lowVisMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    lowVisMenu->addChild(lowVisTxt);
    lowVisMenu->addChild(lowVisChk);
    lowVisMenu->setPosition({20.f, 195.f});
    lowVisMenu->setScale(0.41f);
    lowVisMenu->setAnchorPoint({0, 0.5});
    lowVisMenu->updateLayout();

    // CORNER PIECES OPT
    auto cornersMenu = CCMenu::create();

    auto cornersTxt = CCLabelBMFont::create("Add Corner Pieces", "bigFont.fnt");

    auto cornersChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(VisualsOptPopup::onToggle),
        1.f
    );
    cornersChk->setID("jfpopt-corner-pieces-chk"_spr);
    cornersChk->toggle(m_cornerPieces);
    
    cornersMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    cornersMenu->addChild(cornersTxt);
    cornersMenu->addChild(cornersChk);
    cornersMenu->setPosition({225.f, 235.f});
    cornersMenu->setScale(0.41f);
    cornersMenu->setAnchorPoint({0, 0.5});
    cornersMenu->updateLayout();

    // HIDE ICON OPT
    auto hideIconMenu = CCMenu::create();

    auto hideIconTxt = CCLabelBMFont::create("Hide icon", "bigFont.fnt");

    auto hideIconChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(VisualsOptPopup::onToggle),
        1.f
    );
    hideIconChk->setID("jfpopt-hide-icon-chk"_spr);
    hideIconChk->toggle(m_hideIcon);
    
    hideIconMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    hideIconMenu->addChild(hideIconTxt);
    hideIconMenu->addChild(hideIconChk);
    hideIconMenu->setPosition({291.f, 215.f});
    hideIconMenu->setScale(0.41f);
    hideIconMenu->setAnchorPoint({0, 0.5});
    hideIconMenu->updateLayout();

    // FINISH LINE OPT
    auto finishLineMenu = CCMenu::create();

    auto finishLineTxt = CCLabelBMFont::create("Show Finish Line", "bigFont.fnt");

    auto finishLineChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(VisualsOptPopup::onToggle),
        1.f
    );
    finishLineChk->setID("jfpopt-finish-line-chk"_spr);
    finishLineChk->toggle(m_finishLine);
    
    finishLineMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    finishLineMenu->addChild(finishLineTxt);
    finishLineMenu->addChild(finishLineChk);
    finishLineMenu->setPosition({240.f, 195.f});
    finishLineMenu->setScale(0.41f);
    finishLineMenu->setAnchorPoint({0, 0.5});
    finishLineMenu->updateLayout();

    m_mainLayer->addChild(metersMenu);
    m_mainLayer->addChild(m_markIntMenu);
    m_mainLayer->addChild(lowVisMenu);
    m_mainLayer->addChild(cornersMenu);
    m_mainLayer->addChild(finishLineMenu);
    m_mainLayer->addChild(hideIconMenu);

    // COLOR MODE OPT
    auto colorModeMenu = CCMenu::create();
    auto cmTitleTxt = CCLabelBMFont::create("Color Mode", "bigFont.fnt");
    cmTitleTxt->setPosition({0, 110});
    cmTitleTxt->setScale(0.9f);

    m_mmPicSpr = CCSprite::createWithSpriteFrameName("mark.png"_spr);
    m_mmPicSpr->setScale(180.f / m_mmPicSpr->getContentSize().height);
    m_mmPicSpr->setZOrder(2);
    if (!m_meterMarks) m_flPicSpr->setVisible(false);

    m_flPicSpr = CCSprite::createWithSpriteFrameName("finishline.png"_spr);
    m_flPicSpr->setScale(180.f / m_flPicSpr->getContentSize().height);
    m_flPicSpr->setZOrder(2);
    m_flPicSpr->setOpacity(170);
    if (!m_finishLine) m_flPicSpr->setVisible(false);

    m_colorModePicSpr = CCSprite::createWithSpriteFrameName(
        fmt::format("jfp_cm_{}.png"_spr, m_colorModeIndex).c_str()
    );
    m_colorModePicSpr->setScale(180.f / m_colorModePicSpr->getContentSize().height);

    m_colorModeSelected = CCLabelBMFont::create(
        JFPGen::ColorModeLabel.at(m_colorModeIndex).c_str(), "bigFont.fnt"
    );
    m_colorModeSelected->setID("jfpopt-color-mode"_spr);
    m_colorModeSelected->setPosition({0, -110});
    m_colorModeSelected->setScale(0.75f);

    auto colorModeLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    colorModeLASpr->setScale(.7f);
    auto colorModeLA = CCMenuItemSpriteExtra::create(
        colorModeLASpr, this, menu_selector(VisualsOptPopup::onEnumDecrease)
    );
    colorModeLA->setUserObject(m_colorModeSelected);
    colorModeLA->setPosition({-125, 0});

    auto colorModeRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    colorModeRASpr->setFlipX(true);
    colorModeRASpr->setScale(.7f);
    auto colorModeRA = CCMenuItemSpriteExtra::create(
        colorModeRASpr, this, menu_selector(VisualsOptPopup::onEnumIncrease)
    );
    colorModeRA->setUserObject(m_colorModeSelected);
    colorModeRA->setPosition({125, 0});

    colorModeMenu->setLayout(AnchorLayout::create());
    colorModeMenu->addChild(cmTitleTxt);
    colorModeMenu->addChild(m_colorModePicSpr);
    colorModeMenu->addChild(colorModeLA);
    colorModeMenu->addChild(colorModeRA);
    colorModeMenu->addChild(m_colorModeSelected);
    colorModeMenu->addChild(m_mmPicSpr);
    colorModeMenu->addChild(m_flPicSpr);
    colorModeMenu->setPosition({100.f, 110.f});
    colorModeMenu->setAnchorPoint({0.f, 0.f});
    colorModeMenu->setScale(0.5f);
    colorModeMenu->updateLayout();

    // BG TEXTURE OPT
    auto bgTextureMenu = CCMenu::create();
    auto bgtTitleTxt = CCLabelBMFont::create("Background Texture", "bigFont.fnt");
    bgtTitleTxt->setPosition({0, 110});
    bgtTitleTxt->setScale(0.9f);

    m_bgTexturePicSpr = CCSprite::create(
        fmt::format("game_bg_{:02}_001.png", m_bgTextureIndex == 0 ? 13 : m_bgTextureIndex).c_str()
    );
    m_bgTexturePicSpr->setScale(180.f / m_bgTexturePicSpr->getContentSize().height);
    m_bgTexturePicSpr->setColor({29, 120, 86});
    if (m_bgTextureIndex == 0) m_bgTexturePicSpr->setColor({200, 200, 200});

    m_bgTextureSelected = CCLabelBMFont::create(
        fmt::format("#{:02}", m_bgTextureIndex).c_str(), "bigFont.fnt"
    );
    if (m_bgTextureIndex == 0) m_bgTextureSelected->setCString("Random");
    m_bgTextureSelected->setID("jfpopt-bg-texture"_spr);
    m_bgTextureSelected->setPosition({0, -110});
    m_bgTextureSelected->setScale(0.75f);

    auto bgTextureLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    bgTextureLASpr->setScale(.7f);
    auto bgTextureLA = CCMenuItemSpriteExtra::create(
        bgTextureLASpr, this, menu_selector(VisualsOptPopup::onEnumDecrease)
    );
    bgTextureLA->setUserObject(m_bgTextureSelected);
    bgTextureLA->setPosition({-110, 0});

    auto bgTextureRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    bgTextureRASpr->setFlipX(true);
    bgTextureRASpr->setScale(.7f);
    auto bgTextureRA = CCMenuItemSpriteExtra::create(
        bgTextureRASpr, this, menu_selector(VisualsOptPopup::onEnumIncrease)
    );
    bgTextureRA->setUserObject(m_bgTextureSelected);
    bgTextureRA->setPosition({110, 0});

    bgTextureMenu->setLayout(AnchorLayout::create());
    bgTextureMenu->addChild(bgtTitleTxt);
    bgTextureMenu->addChild(m_bgTexturePicSpr);
    bgTextureMenu->addChild(bgTextureLA);
    bgTextureMenu->addChild(bgTextureRA);
    bgTextureMenu->addChild(m_bgTextureSelected);
    bgTextureMenu->setPosition({285.f, 110.f});
    bgTextureMenu->setAnchorPoint({0.f, 0.f});
    bgTextureMenu->setScale(0.5f);
    bgTextureMenu->updateLayout();

    m_mainLayer->addChild(colorModeMenu);
    m_mainLayer->addChild(bgTextureMenu);

    return true;
}

void VisualsOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        VisualsOptPopup::save(object);
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void VisualsOptPopup::save(CCObject*) {

    mod->setSavedValue<bool>("opt-0-show-meter-marks", m_meterMarks);
    uint32_t markIntParsed = numFromString<uint32_t>(m_markIntInput->getString()).unwrapOr(0);
    if (markIntParsed > 0 && markIntParsed < 100000) mod->setSavedValue<uint32_t>(
        "opt-0-mark-interval", markIntParsed
    );
    mod->setSavedValue<bool>("opt-0-low-visibility", m_lowVisiblity);
    mod->setSavedValue<bool>("opt-0-add-corner-pieces", m_cornerPieces);
    mod->setSavedValue<bool>("opt-0-show-finish-line", m_finishLine);
    mod->setSavedValue<bool>("opt-0-hide-icon", m_hideIcon);
    mod->setSavedValue<uint8_t>("opt-0-color-mode", m_colorModeIndex);
    mod->setSavedValue<uint8_t>("opt-0-background-texture", m_bgTextureIndex);

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void VisualsOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    if (chkID == "jfpopt-meter-marks-chk"_spr) {
        m_meterMarks = toggled;
        if (toggled) {
            m_markIntMenu->setOpacity(255);
            m_mmPicSpr->setVisible(true);
        } else {
            m_markIntMenu->setOpacity(127);
            m_mmPicSpr->setVisible(false);
        }
    } else if (chkID == "jfpopt-low-visibility-chk"_spr) {
        m_lowVisiblity = toggled;
    } else if (chkID == "jfpopt-corner-pieces-chk"_spr) {
        m_cornerPieces = toggled;
    } else if (chkID == "jfpopt-finish-line-chk"_spr) {
        m_finishLine = toggled;
        if (toggled) {
            m_flPicSpr->setVisible(true);
        } else {
            m_flPicSpr->setVisible(false);
        }
    } else if (chkID == "jfpopt-hide-icon-chk"_spr) {
        m_hideIcon = toggled;
    } else {
        log::warn("Unknown toggle: {}", chkID);
    }
}

void VisualsOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());
    std::string labelText;
    auto* frameCache = CCSpriteFrameCache::get();

    if (lbl->getID() == "jfpopt-bg-texture"_spr) {
        if (m_bgTextureIndex == 0) m_bgTextureIndex = m_bgTextureIndexLen;
        m_bgTextureIndex -= 1;
        labelText = fmt::format("#{:02}", m_bgTextureIndex);
        if (m_bgTextureIndex == 0) {
            labelText = "Random";
            m_bgTexturePicSpr->setColor({200, 200, 200});
        } else {
            m_bgTexturePicSpr->setColor({29, 120, 86});
        }
        
        auto* newBgTexture = CCSprite::create(
            fmt::format("game_bg_{:02}_001.png", m_bgTextureIndex == 0 ? 13 : m_bgTextureIndex).c_str()
        )->displayFrame();
        m_bgTexturePicSpr->setDisplayFrame(newBgTexture);
    } else if (lbl->getID() == "jfpopt-color-mode"_spr) {
        if (m_colorModeIndex == 0) m_colorModeIndex = m_colorModeIndexLen;
        m_colorModeIndex -= 1;
        labelText = JFPGen::ColorModeLabel.at(m_colorModeIndex);
        const char* newSprite = fmt::format("jfp_cm_{}.png"_spr, m_colorModeIndex).c_str();
        m_colorModePicSpr->setDisplayFrame(
            frameCache->spriteFrameByName(newSprite)
        );
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }

    lbl->setCString(labelText.c_str());
}

void VisualsOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());
    std::string labelText;
    auto* frameCache = CCSpriteFrameCache::get();

    if (lbl->getID() == "jfpopt-bg-texture"_spr) {
        m_bgTextureIndex += 1;
        if (m_bgTextureIndex >= m_bgTextureIndexLen) m_bgTextureIndex = 0;
        labelText = fmt::format("#{:02}", m_bgTextureIndex);
        if (m_bgTextureIndex == 0) {
            labelText = "Random";
            m_bgTexturePicSpr->setColor({200, 200, 200});
        } else {
            m_bgTexturePicSpr->setColor({29, 120, 86});
        }

        auto* newBgTexture = CCSprite::create(
            fmt::format("game_bg_{:02}_001.png", m_bgTextureIndex == 0 ? 13 : m_bgTextureIndex).c_str()
        )->displayFrame();
        m_bgTexturePicSpr->setDisplayFrame(newBgTexture);
    } else if (lbl->getID() == "jfpopt-color-mode"_spr) {
        m_colorModeIndex += 1;
        if (m_colorModeIndex >= m_colorModeIndexLen) m_colorModeIndex = 0;
        labelText = JFPGen::ColorModeLabel.at(m_colorModeIndex);
        const char* newSprite = fmt::format("jfp_cm_{}.png"_spr, m_colorModeIndex).c_str();
        m_colorModePicSpr->setDisplayFrame(
            frameCache->spriteFrameByName(newSprite)
        );
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }

    lbl->setCString(labelText.c_str());
}

void VisualsOptPopup::onInfo(CCObject*) {
    const char* info =
        "#### Options handling JFP's appearance and visuals\n\n"
        "<cp>Show Meter Marks</c>: Shows dashed marking lines after so many meters (blocks) have passed\n\n"
        "<co>Mark Interval (m)</c>: The interval, in meters, at which to spawn mark lines\n- Dependent on <cp>Show Meter Marks</c>\n\n"
        "<cg>Color Mode</c>: Basic coloring options, separate from complete themes\n"
        "- Washed: washed out random background color\n"
        "- All Colors: any random background color\n"
        "- Classic Mode: classic Juggernaut gray color\n"
        "- Night Mode: black background with bright randomly colored lines\n"
        "- Random\n\n"
        "<cl>Background Texture</c>: Background to use in standard visuals JFP. Can be overridden by themes\n\n"
        "<cb>Low Visibility</c>: Adds a dark fog to the right of the screen, significantly lowering FOV\n\n"
        "<cp>Add Corner Pieces</c>: Place corner pieces connecting slopes at their edges\n\n"
        "<cy>Hide Icon</c>: Hides wave icon during gameplay\n\n"
        "<cj>Show Finish Line</c>: Shows checkered vertical finish at the corridor's mouth\n\n";

    auto infoLayer = MDPopup::create("Visual Options Info",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
}

VisualsOptPopup* VisualsOptPopup::create(std::string const& text) {
    auto ret = new VisualsOptPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
