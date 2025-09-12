#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "ThemeSelectPopup.hpp"
#include "../utils/Theming.hpp"

using namespace geode::prelude;

bool ThemeSelectPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("JFP Themes");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(ThemeSelectPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

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

    std::string themeName =
        Mod::get()->getSavedValue<std::string>("active-theme");

    auto localPath = CCFileUtils::sharedFileUtils();
    std::string fp = std::string(localPath->getWritablePath()) + "/jfp/themes/";

    for (const auto& entry : std::filesystem::directory_iterator(fp)) {
        if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();
            if (filename.size() <= 5 || filename.substr(filename.size() - 5) != ".jfpt") {
                continue;
            }
            std::string numPart = filename.substr(0, filename.size() - 5);
            m_themeFiles.push_back(numPart);
        }
    }

    std::vector<std::string> existingNames;
    m_themeUseBtns = CCArray::create();

    auto activeThemeName = Mod::get()->getSavedValue<std::string>("active-theme");

    for (size_t i = 0; i < m_themeFiles.size(); i++) {
        auto tmd = ThemeGen::parseThemeMeta(m_themeFiles.at(i));

        auto testMenu = CCLayerColor::create(ccColor4B(0, 0, 0, (i % 2 ? 130 : 90)));
        testMenu->setContentSize({scrlLayer->getContentSize().width, 85.f});
        testMenu->ignoreAnchorPointForPosition(false);
        
        // std::string tmdName = tmd.name;
        // if (std::find(existingNames.begin(), existingNames.end(), tmdName) == existingNames.end()) {
        //     existingNames.push_back(tmdName);
        // } else {
        //     // testFilename...
        // }
        auto testLabel = CCLabelBMFont::create(tmd.name.c_str(), "bigFont.fnt");
        testLabel->setAnchorPoint({0.f, 1.f});
        testLabel->setPosition({10.f, testMenu->getContentSize().height - 10.f});
        testLabel->setScale(0.6f);
        testMenu->addChild(testLabel);

        auto testFilename = CCLabelBMFont::create((m_themeFiles.at(i) + ".jfpt").c_str(), "bigFont.fnt");
        testFilename->setAnchorPoint({1.f, 1.f});
        testFilename->setPosition({
            testMenu->getContentSize().width - 6.f,
            testMenu->getContentSize().height - 6.f
        });
        testFilename->setScale(0.3f);
        testFilename->setOpacity(130);
        testMenu->addChild(testFilename);

        auto testSubheading = CCLabelBMFont::create(("by " + tmd.author).c_str(), "bigFont.fnt");
        testSubheading->setAnchorPoint({0.f, 1.f});
        testSubheading->setPosition({10.f, testMenu->getContentSize().height - 32.f});
        testSubheading->setScale(0.4f);
        testSubheading->setOpacity(200);
        testMenu->addChild(testSubheading);

        auto testDesc = CCLabelBMFont::create(tmd.description.c_str(), "bigFont.fnt");
        testDesc->setAnchorPoint({0.f, 0.f});
        testDesc->setPosition({10.f, 10.f});
        testDesc->setScale(0.3f);
        testDesc->setOpacity(200);
        testMenu->addChild(testDesc);

        // GJ_selectSongBtn_001.png
        // GJ_selectSongOnBtn_001.png

        auto testUsebtnMenu = CCMenu::create();
        auto testUsebtnSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
        auto testUsebtn = CCMenuItemSpriteExtra::create(
            testUsebtnSpr, this, menu_selector(ThemeSelectPopup::onSelectTheme)
        );
        testUsebtn->setTag(i);
        testUsebtnMenu->setAnchorPoint({0.5f, 0.5f});
        testUsebtnMenu->setPosition({testMenu->getContentSize().width - 
            testUsebtn->getContentSize().width / 2.f - 6.f, testUsebtn->getContentSize().height / 2.f + 6.f}
        );
        testUsebtnMenu->setZOrder(1);
        testUsebtnMenu->addChild(testUsebtn);
        testMenu->addChild(testUsebtnMenu);
    
        if (activeThemeName == m_themeFiles.at(i)) {
            m_activeThemeIndex = i;
            testUsebtn->setSprite(CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png"));
            testUsebtn->setEnabled(false);
        }

        scrl->m_contentLayer->addChild(testMenu);
        m_themeUseBtns->addObject(testUsebtn);
    }

    scrl->m_contentLayer->updateLayout();
    scrl->moveToTop();

    return true;
}

void ThemeSelectPopup::onSelectTheme(CCObject* object) {
    if (m_activeThemeIndex >= 0) {
        auto activeBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(m_themeUseBtns->objectAtIndex(m_activeThemeIndex));
        if (activeBtn != nullptr) {
            activeBtn->setSprite(CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png"));
            activeBtn->setEnabled(true);
        }
    }
    uint16_t newIndex = object->getTag();
    auto newActiveBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(m_themeUseBtns->objectAtIndex(newIndex));
    if (newActiveBtn != nullptr) {
        m_activeThemeIndex = newIndex;
        newActiveBtn->setSprite(CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png"));
        newActiveBtn->setEnabled(false);

        Mod::get()->setSavedValue<std::string>("active-theme", m_themeFiles.at(newIndex));
    }
}

void ThemeSelectPopup::onClose(CCObject* object) {
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void ThemeSelectPopup::onInfo(CCObject*) {
    const char* info =
        "<cp>JFP Themes</c> add decoration to your corridor!\n\n"
        "Each theme works by scanning the corridor's pattern, then placing blocks dependent on what it finds.\n\n"
        "In addition, themes can set color channels, add repeating block patterns, and change k-Values (background image, ground texture, etc.)\n\n"
        "The default themes are <co>heinous</c>, <cg>gandhi</c>, <cy>ninecircles</c> and <cl>moonmen</c>.\n\n"
        "Each theme is defined in a <cp>.JFPT</c> file and kept in the themes folder. You can easily swap JFPT files with other players.\n\n"
        "Some themes only work properly on certain options (for example, <cy>ninecircles</c>) is bigwave-only and doesn't support miniwave). "
        "When you try to use these themes, you might get a warning if your options aren't compatible.\n";

    auto infoLayer = MDPopup::create("JFP Theming System",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
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
