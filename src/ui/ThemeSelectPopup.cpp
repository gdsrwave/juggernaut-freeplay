#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "ThemeSelectPopup.hpp"

using namespace geode::prelude;

bool ThemeSelectPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("JFP Themes");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    auto const selectorSize = CCSize(346, 200);

    auto scrl = ScrollLayer::create(selectorSize);
    scrl->setTouchEnabled(true);

    auto scrlLayer = CCLayerColor::create({0, 0, 0, 130});
    scrlLayer->setContentSize(selectorSize);
    scrlLayer->ignoreAnchorPointForPosition(false);

    // https://github.com/geode-sdk/geode/blob/a6f215fc86c991a305d7c4aef3fc01c3b2bc174f/loader/src/ui/mods/settings/ModSettingsPopup.cpp#L44
    scrl->m_contentLayer->setLayout(
        ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAutoGrowAxis(scrl->getContentSize().height)
        ->setCrossAxisOverflow(false)
        ->setAxisAlignment(AxisAlignment::End)
        ->setGap(0)
    );

    m_mainLayer->addChildAtPosition(scrlLayer, Anchor::Center);
    scrlLayer->addChildAtPosition(scrl, Anchor::BottomLeft);

    auto scrollbar = Scrollbar::create(scrl);
    m_mainLayer->addChildAtPosition(
        scrollbar,
        Anchor::Center,
        {scrlLayer->getContentSize().width / 2.f + 10.f, 0.f}
    );

    auto testMenu = CCLayerColor::create({0, 0, 0, 130});
    testMenu->setContentSize({scrlLayer->getContentSize().width, 85.f});
    testMenu->ignoreAnchorPointForPosition(false);
    
    auto testLabel = CCLabelBMFont::create("Heinous Theme", "bigFont.fnt");
    testLabel->setAnchorPoint({0.f, 1.f});
    testLabel->setPosition({10.f, testMenu->getContentSize().height - 10.f});
    testLabel->setScale(0.6f);
    testMenu->addChild(testLabel);

    auto testSubheading = CCLabelBMFont::create("by Nyan Cat", "bigFont.fnt");
    testSubheading->setAnchorPoint({0.f, 1.f});
    testSubheading->setPosition({10.f, testMenu->getContentSize().height - 32.f});
    testSubheading->setScale(0.4f);
    testSubheading->setOpacity(200);
    testMenu->addChild(testSubheading);

    auto testDesc = CCLabelBMFont::create("A JFP theme based on Heinous Wave.", "bigFont.fnt");
    testDesc->setAnchorPoint({0.f, 0.f});
    testDesc->setPosition({10.f, 10.f});
    testDesc->setScale(0.3f);
    testDesc->setOpacity(200);
    testMenu->addChild(testDesc);

    // GJ_selectSongBtn_001.png
    // GJ_selectSongOnBtn_001.png

    auto testUsebtnSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png");
    auto testUsebtn = CCMenuItemSpriteExtra::create(
        testUsebtnSpr, this, menu_selector(ThemeSelectPopup::onSelectTheme)
    );
    testUsebtn->setAnchorPoint({0.5f, 0.5f});
    testUsebtn->setPosition({testMenu->getContentSize().width - 
        testUsebtn->getContentSize().width / 2.f - 10.f, testUsebtn->getContentSize().height / 2.f + 10.f}
    );
    testMenu->addChild(testUsebtn);

    scrl->m_contentLayer->addChild(testMenu);
    scrl->m_contentLayer->updateLayout();

    return true;
}

void ThemeSelectPopup::onSelectTheme(CCObject* object) {
    return;
}

void ThemeSelectPopup::onClose(CCObject* object) {
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

ThemeSelectPopup* ThemeSelectPopup::create(std::string const& text) {
    auto ret = new ThemeSelectPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
