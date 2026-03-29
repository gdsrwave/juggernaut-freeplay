#pragma once
#include <string>
#include <Geode/Geode.hpp>
#include "OptionSelectPopup.hpp"
#include "../utils/OptionStr.hpp"
#include "./OptionStrPopup.hpp"
#include "./OptionStrPopupDeluxe.hpp"

using namespace geode::prelude;

bool OptionSelectPopup::setup(std::string const& value) {
    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Juggernaut Presets");
    auto windowDim = CCDirector::sharedDirector()->getWinSize();

    // INFO BUTTON
    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(OptionSelectPopup::onInfo));
    infoBtn->setScale(0.8f);
    auto infoBtnMenu = CCMenu::create();
    infoBtnMenu->setPosition({380.f, 260.f});
    infoBtnMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoBtnMenu);

    auto const selectorSize = CCSize(346, 200);

    m_scrl = ScrollLayer::create(selectorSize);
    m_scrl->setTouchEnabled(true);

    auto scrlLayer = CCLayerColor::create({0, 0, 0, 130});
    scrlLayer->setContentSize(selectorSize);
    scrlLayer->ignoreAnchorPointForPosition(false);

    // https://github.com/geode-sdk/geode/blob/a6f215fc86c991a305d7c4aef3fc01c3b2bc174f/loader/src/ui/mods/settings/ModSettingsPopup.cpp#L44
    m_scrl->m_contentLayer->setLayout(
        ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAutoGrowAxis(m_scrl->getContentSize().height)
        ->setCrossAxisOverflow(false)
        ->setAxisAlignment(AxisAlignment::End)
        ->setGap(0)
    );

    m_mainLayer->addChildAtPosition(scrlLayer, Anchor::Center);
    scrlLayer->addChildAtPosition(m_scrl, Anchor::BottomLeft);

    auto scrollbar = Scrollbar::create(m_scrl);
    m_mainLayer->addChildAtPosition(
        scrollbar,
        Anchor::Center,
        {scrlLayer->getContentSize().width / 2.f + 10.f, 0.f}
    );

    std::vector<std::string> existingNames;
    m_optionSlots = CCArray::create();

    auto activeThemeName = Mod::get()->getSavedValue<std::string>("active-theme");

    std::string currentBiome = "Juggernaut";
    
    const std::vector<OptionString> loadedOpts = {};

    m_optionStrs = defaultOptBank;
    m_optionStrs.insert(m_optionStrs.end(), loadedOpts.begin(), loadedOpts.end());
    std::sort(m_optionStrs.begin(), m_optionStrs.end(),
        [](const OptionString& a, const OptionString& b) { return a.name < b.name; }
    );

    for (size_t i = 0; i < m_optionStrs.size(); i++) {

        m_checkboxes.push_back(false);

        auto testMenu = CCLayerColor::create(ccColor4B(0, 0, 0, (i % 2 ? 130 : 90)));
        testMenu->setContentSize({scrlLayer->getContentSize().width, 50.f});
        testMenu->ignoreAnchorPointForPosition(false);
        
        // std::string tmdName = tmd.name;
        // if (std::find(existingNames.begin(), existingNames.end(), tmdName) == existingNames.end()) {
        //     existingNames.push_back(tmdName);
        // } else {
        //     // testFilename...
        // }
        auto testLabel = CCLabelBMFont::create(m_optionStrs[i].name.c_str(), "bigFont.fnt");
        testLabel->setAnchorPoint({0.f, 1.f});
        testLabel->setPosition({10.f, testMenu->getContentSize().height - 5.f});
        testLabel->setScale(0.6f);
        testMenu->addChild(testLabel);

        auto testSubheading = CCLabelBMFont::create(m_optionStrs[i].optExported.c_str(), "bigFont.fnt");
        testSubheading->setAnchorPoint({0.f, 1.f});
        testSubheading->setPosition({10.f, testMenu->getContentSize().height - 30.f});
        testSubheading->setScale(0.35f);
        testSubheading->setOpacity(200);
        testMenu->addChild(testSubheading);

        // GJ_selectSongBtn_001.png
        // GJ_selectSongOnBtn_001.png
        if (m_optionStrs[i].isPreset) {
            auto optionChkMenu = CCMenu::create();
            auto optionChk = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(OptionSelectPopup::onToggle),
                1.f
            );

            optionChkMenu->setTag(1);
            optionChkMenu->setAnchorPoint({0.f, 0.f});
            optionChkMenu->setPosition({testMenu->getContentSize().width - 
                optionChk->getContentSize().width / 2.f - 45.f, optionChk->getContentSize().height / 2.f + 8.f}
            );
            optionChkMenu->setScale(0.65f);
            optionChk->setZOrder(3);
            optionChkMenu->addChild(optionChk);
            testMenu->addChild(optionChkMenu);
        }

        auto testUsebtnMenu = CCMenu::create();
        auto testUsebtnSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
        auto testUsebtn = CCMenuItemSpriteExtra::create(
            testUsebtnSpr, this, menu_selector(OptionSelectPopup::onSelectOption)
        );
        testUsebtn->setTag(i);
        testUsebtnMenu->setAnchorPoint({0.5f, 0.5f});
        testUsebtnMenu->setPosition({testMenu->getContentSize().width - 
            testUsebtn->getContentSize().width / 2.f - 6.f, testUsebtn->getContentSize().height / 2.f + 6.f}
        );
        testUsebtnMenu->setZOrder(1);
        testUsebtnMenu->addChild(testUsebtn);
        testMenu->addChild(testUsebtnMenu);

        m_scrl->m_contentLayer->addChild(testMenu);
        m_optionSlots->addObject(testMenu);
        //m_themeUseBtns->addObject(testUsebtn);
    }

    m_scrl->m_contentLayer->updateLayout();
    m_scrl->moveToTop();

    // DELETE OPTION CODES
    auto trashMenu = CCMenu::create();
    auto trashBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "trashbin.png"_spr),
        this, menu_selector(OptionSelectPopup::onTrash));
    trashMenu->setPosition({10.f, 10.f});
    trashMenu->addChild(trashBtn);
    m_mainLayer->addChild(trashMenu);

    // ADD OPTION CODE
    auto addOptsMenu = CCMenu::create();
    auto addOptsSpr = CircleButtonSprite::createWithSpriteFrameName(
        "restorefolder.png"_spr, 1.f,
        CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    auto addOptsBtn = CCMenuItemSpriteExtra::create(
        addOptsSpr,
        this, menu_selector(OptionSelectPopup::onAddOptions));
    
    addOptsMenu->setPosition({m_mainLayer->getContentSize().width/2.f, 10.f});
    addOptsMenu->addChild(addOptsBtn);
    m_mainLayer->addChild(addOptsMenu);

    // QUICK IMPORT
    auto restoreMenu = CCMenu::create();
    auto restoreSpr = CircleButtonSprite::createWithSpriteFrameName(
        "restorefolder.png"_spr, 1.f,
        CircleBaseColor::DarkAqua, CircleBaseSize::Medium);
    auto restoreButton = CCMenuItemSpriteExtra::create(
        restoreSpr,
        this, menu_selector(OptionSelectPopup::onQuickImport));
    
    restoreMenu->setPosition({m_mainLayer->getContentSize().width - 10.f, 10.f});
    restoreMenu->addChild(restoreButton);
    m_mainLayer->addChild(restoreMenu);

    return true;
}

void OptionSelectPopup::onToggle(CCObject* object) {
    // nothing needed here at the moment -M
}

void OptionSelectPopup::onSelectOption(CCObject* object) {
    uint16_t newIndex = object->getTag();
    auto selectedOption = m_optionStrs[newIndex];
    importSettings(selectedOption.optExported);
}

void OptionSelectPopup::onClickFolder(CCObject*) {
    file::openFolder(Mod::get()->getSaveDir() / "themes");
}

void OptionSelectPopup::onQuickImport(CCObject* object) {
    OptionStrPopup::create("")->show();
    OptionSelectPopup::onClose(object);
}

void OptionSelectPopup::onAddOptions(CCObject* object) {
    OptionStrPopupDeluxe::create(this)->show();
}

void OptionSelectPopup::onTrash(CCObject* object) {
    std::filesystem::path srcDir = Mod::get()->getResourcesDir();
    std::vector<std::string> themeFiles;
    for (const auto& fileName :
            std::filesystem::directory_iterator(srcDir)) {
        auto fileStr = fileName.path().filename().string();
        if (fileStr.size() >= 5 &&
                fileStr.substr(fileStr.size() - 5) == ".jfpt") {
            themeFiles.push_back(fileStr);
        }
    }

    auto jfpConfirm = createQuickPopup(
        "JFP",
        "Permanently <cr>delete</c> all selected option codes?",
        "No", "Yes",
        [this](bool b1, bool b2) {
            if (b2) {
                for (int i = m_checkboxes.size() - 1; i >= 0; i--) {
                    auto targetSection = typeinfo_cast<CCLayerColor*>(m_optionSlots->objectAtIndex(i));
                    auto targetChkMenu = typeinfo_cast<CCMenu*>(targetSection->getChildByTag(1));
                    auto targetCheckbox = typeinfo_cast<CCMenuItemToggler*>(targetChkMenu->getChildren()->objectAtIndex(0));
                    if (targetCheckbox->isToggled()) {
                        log::info("{}", i);
                        m_scrl->m_contentLayer->removeChild(targetSection, true);
                        m_checkboxes.erase(m_checkboxes.begin() + i);
                        m_optionSlots->removeObjectAtIndex(i);
                    };
                }
                m_scrl->m_contentLayer->updateLayout();
            }
        });
}

void OptionSelectPopup::onClose(CCObject* object) {
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void OptionSelectPopup::onInfo(CCObject*) {
    const char* info =
        "<cp>Juggernaut Themes</c> add decoration to your corridor!\n\n"
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

OptionSelectPopup* OptionSelectPopup::create(std::string const& text) {
    auto ret = new OptionSelectPopup();
    if (ret->initAnchored(400.f, 280.f, text, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
