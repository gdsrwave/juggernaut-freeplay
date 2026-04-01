#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "SoundtrackOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool SoundtrackOptPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Music Options");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // SAVE BUTTON
    auto saveBtnMenu = CCMenu::create();
    auto saveBtnS = ButtonSprite::create(
        "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
    saveBtnS->setScale(0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveBtnS, this, menu_selector(SoundtrackOptPopup::onSave));

    saveBtnMenu->addChild(saveBtn);
    saveBtnMenu->setPosition({200.f, 30.f});
    m_mainLayer->addChild(saveBtnMenu);

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(SoundtrackOptPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

    // SOUNDTRACK OPT
    auto mSrcMenu = CCMenu::create();
    
    auto mSrcTxt = CCLabelBMFont::create("Music Selection:", "bigFont.fnt");
    m_musicSourceSelected = CCLabelBMFont::create("wwwwwwwwwwwwww", "bigFont.fnt");
    m_musicSourceSelected->setID("jfpopt-music-source"_spr);

    auto mSrcLASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    mSrcLASpr->setScale(.7f);
    auto mSrcLA = CCMenuItemSpriteExtra::create(
        mSrcLASpr, this, menu_selector(SoundtrackOptPopup::onEnumDecrease)
    );
    mSrcLA->setUserObject(m_musicSourceSelected);

    auto mSrcRASpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    mSrcRASpr->setFlipX(true);
    mSrcRASpr->setScale(.7f);
    auto mSrcRA = CCMenuItemSpriteExtra::create(
        mSrcRASpr, this, menu_selector(SoundtrackOptPopup::onEnumIncrease)
    );
    mSrcRA->setUserObject(m_musicSourceSelected);

    mSrcMenu->setLayout(RowLayout::create()
        ->setGap(25.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    mSrcMenu->addChild(mSrcTxt);
    mSrcMenu->addChild(mSrcLA);
    mSrcMenu->addChild(m_musicSourceSelected);
    mSrcMenu->addChild(mSrcRA);
    mSrcMenu->setPosition({20.f, 225.f});
    mSrcMenu->setAnchorPoint({0, 0.5});
    mSrcMenu->setScale(0.6f);
    mSrcMenu->updateLayout();

    // SONG OFFSET OPT
    auto musicOffsetMenu = CCMenu::create();

    auto musicOffsetTxt = CCLabelBMFont::create("Random Song Offset", "bigFont.fnt");

    auto musicOffsetChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(SoundtrackOptPopup::onToggle),
        1.f
    );
    musicOffsetChk->setID("jfpopt-music-offset-chk"_spr);
    musicOffsetChk->toggle(m_musicOffset);
    
    musicOffsetMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    musicOffsetMenu->addChild(musicOffsetTxt);
    musicOffsetMenu->addChild(musicOffsetChk);
    musicOffsetMenu->setPosition({20.f, 200.f});
    musicOffsetMenu->setScale(0.41f);
    musicOffsetMenu->setAnchorPoint({0, 0.5});
    musicOffsetMenu->updateLayout();

    // SONG LOOP OPT
    auto musicLoopMenu = CCMenu::create();

    auto musicLoopTxt = CCLabelBMFont::create("Loop Song", "bigFont.fnt");

    auto musicLoopChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(SoundtrackOptPopup::onToggle),
        1.f
    );
    musicLoopChk->setID("jfpopt-music-loop-chk"_spr);
    musicLoopChk->toggle(m_musicLoop);
    
    musicLoopMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    musicLoopMenu->addChild(musicLoopTxt);
    musicLoopMenu->addChild(musicLoopChk);
    musicLoopMenu->setPosition({20.f, 175.f});
    musicLoopMenu->setScale(0.41f);
    musicLoopMenu->setAnchorPoint({0, 0.5});
    musicLoopMenu->updateLayout();

    std::string mSrcLabelText = JFPGen::MusicSourceLabel.at(m_msrcIndex);
    m_musicSourceSelected->setCString(mSrcLabelText.c_str());

    m_mainLayer->addChild(mSrcMenu);
    m_mainLayer->addChild(musicOffsetMenu);
    m_mainLayer->addChild(musicLoopMenu);

    return true;
}

void SoundtrackOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        SoundtrackOptPopup::save();
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void SoundtrackOptPopup::onSave(CCObject*) {
    SoundtrackOptPopup::save();
    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void SoundtrackOptPopup::save() {
    mod->setSavedValue<uint8_t>("opt-0-music-source", m_msrcIndex);
    mod->setSavedValue<bool>("opt-0-music-offset", m_musicOffset);
    mod->setSavedValue<bool>("opt-0-music-loop", m_musicLoop);
}

void SoundtrackOptPopup::onToggle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    auto chkID = chk->getID();
    bool toggled = !chk->isToggled();

    if (chkID == "jfpopt-music-offset-chk"_spr) {
        m_musicOffset = toggled;
    } else if (chkID == "jfpopt-music-loop-chk"_spr) {
        m_musicLoop = toggled;
    } else {
        log::warn("Unknown toggle: {}", chkID);
    }
}


void SoundtrackOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-music-source"_spr) {
        if (m_msrcIndex == 0) m_msrcIndex = m_msrcIndexLen;
        m_msrcIndex -= 1;
        std::string mSrcLabelText = JFPGen::MusicSourceLabel.at(m_msrcIndex);
        lbl->setCString(mSrcLabelText.c_str());
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }
}

void SoundtrackOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-music-source"_spr) {
        m_msrcIndex += 1;
        if (m_msrcIndex >= m_msrcIndexLen) m_msrcIndex = 0;
        std::string mSrcLabelText = JFPGen::MusicSourceLabel.at(m_msrcIndex);
        lbl->setCString(mSrcLabelText.c_str());
    } else {
        log::warn("Unknown toggle: {}", lbl->getID());
    }
}

void SoundtrackOptPopup::onInfo(CCObject*) {
    const char* info =
        "#### Options handling JFP's music and playlist system\n\n"
        "<cp>Music Selection</c>: Determines where the in-level songs are pulled from\n"
        "- Local Music: Shuffle existing mp3 songs in your GD Data folder\n"
        "- JFP Soundtrack: Set of music submitted by the JFP developers and playerbase. "
        "Unfortunately, this list is difficult to download; you can view the full list of song IDs "
        "[here](https://github.com/gdsrwave/juggernaut-freeplay/blob/b81949128845548b2e17adf084f0ad8c6d4dd228/src/utils/shared.cpp#L97).\n\n"
        "<co>Random Song Offset</c>: Starts the level music between 0 and 75 percent through the song\n\n"
        "<cb>Loop Song</c>: Loops the level music after song end (via song trigger)";

    auto infoLayer = MDPopup::create("Music Options Info",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
}

SoundtrackOptPopup* SoundtrackOptPopup::create(std::string const& text) {
    auto ret = new SoundtrackOptPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
