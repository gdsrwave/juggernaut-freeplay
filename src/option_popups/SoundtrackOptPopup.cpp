#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "SoundtrackOptPopup.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

bool SoundtrackOptPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Soundtrack Options");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // SAVE BUTTON
    auto saveBtnMenu = CCMenu::create();
    auto saveBtnS = ButtonSprite::create(
        "Save", "bigFont.fnt", "GJ_button_01.png", 1.f);
    saveBtnS->setScale(0.7f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveBtnS, this, menu_selector(SoundtrackOptPopup::save));

    saveBtnMenu->addChild(saveBtn);
    saveBtnMenu->setPosition({200.f, 30.f});
    m_mainLayer->addChild(saveBtnMenu);

    // SOUNDTRACK OPT
    auto mSrcMenu = CCMenu::create();
    
    auto mSrcTxt = CCLabelBMFont::create("Allowed Input Types:", "bigFont.fnt");
    m_musicSourceSelected = CCLabelBMFont::create("wwwwwwwwwwwwww", "bigFont.fnt");
    m_musicSourceSelected->setID("jfpopt-music-source");

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

    std::string mSrcLabelText = JFPGen::MusicSourceLabel.at(m_msrcIndex);
    m_musicSourceSelected->setCString(mSrcLabelText.c_str());

    m_mainLayer->addChild(mSrcMenu);

    return true;
}

void SoundtrackOptPopup::onClose(CCObject* object) {
    if (mod->getSavedValue<bool>("opt-u-save-on-close")) {
        SoundtrackOptPopup::save(object);
    }
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void SoundtrackOptPopup::save(CCObject*) {
    mod->setSavedValue<int8_t>("opt-0-music-source", m_msrcIndex);

    Notification::create("Saved Successfully",
        NotificationIcon::Success, 0.5f)->show();
}

void SoundtrackOptPopup::onEnumDecrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-music-source") {
        m_msrcIndex -= 1;
        if (m_msrcIndex < 0) m_msrcIndex = m_msrcIndexLen - 1;
        std::string mSrcLabelText = JFPGen::MusicSourceLabel.at(m_msrcIndex);
        lbl->setCString(mSrcLabelText.c_str());
    } else {
        log::info("Unknown toggle: {}", lbl->getID());
    }
}

void SoundtrackOptPopup::onEnumIncrease(CCObject* object) {
    auto arrow = typeinfo_cast<CCMenuItemSpriteExtra*>(object);
    auto lbl = typeinfo_cast<CCLabelBMFont*>(arrow->getUserObject());

    if (lbl->getID() == "jfpopt-music-source") {
        m_msrcIndex += 1;
        if (m_msrcIndex >= m_msrcIndexLen) m_msrcIndex = 0;
        std::string mSrcLabelText = JFPGen::MusicSourceLabel.at(m_msrcIndex);
        lbl->setCString(mSrcLabelText.c_str());
    } else {
        log::info("Unknown toggle: {}", lbl->getID());
    }
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
