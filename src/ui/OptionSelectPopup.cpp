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

    OptionSelectPopup::refreshLayout();

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
    auto addOptsBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(
        "GJ_newBtn_001.png"),
        this, menu_selector(OptionSelectPopup::onAddOptions));
    addOptsMenu->setPosition({m_mainLayer->getContentSize().width/2.f, 10.f});
    addOptsMenu->addChild(addOptsBtn);
    addOptsBtn->setScale(0.9f);
    m_mainLayer->addChild(addOptsMenu);

    // SHUFFLE 
    auto shuffleMenu = CCMenu::create();
    auto shuffleSpr = CircleButtonSprite::createWithSpriteFrameName(
        "shuffleIcon.png"_spr, 1.f,
        CircleBaseColor::DarkPurple, CircleBaseSize::Medium);
    auto shuffleButton = CCMenuItemSpriteExtra::create(
        shuffleSpr,
        this, menu_selector(OptionSelectPopup::onShuffle));
    
    shuffleMenu->setPosition({m_mainLayer->getContentSize().width - 60.f, 10.f});
    shuffleMenu->addChild(shuffleButton);
    shuffleButton->setScale(0.9f);
    m_mainLayer->addChild(shuffleMenu);

    // QUICK IMPORT
    auto restoreMenu = CCMenu::create();
    auto restoreSpr = CircleButtonSprite::createWithSpriteFrameName(
        "importIcon.png"_spr, 1.f,
        CircleBaseColor::Green, CircleBaseSize::Medium);
    auto restoreButton = CCMenuItemSpriteExtra::create(
        restoreSpr,
        this, menu_selector(OptionSelectPopup::onQuickImport));
    
    restoreMenu->setPosition({m_mainLayer->getContentSize().width - 10.f, 10.f});
    restoreMenu->addChild(restoreButton);
    restoreButton->setScale(0.9f);
    m_mainLayer->addChild(restoreMenu);

    // SHUFFLE CHECKBOX
    auto tShufflingMenu = CCMenu::create();

    auto tShufflingTxt = CCLabelBMFont::create("Shuffling", "bigFont.fnt");

    auto tShufflingChk = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(OptionSelectPopup::onToggleShuffle),
        1.f
    );
    tShufflingChk->toggle(Mod::get()->getSavedValue<bool>("opt-0-shuffling", false));
    
    tShufflingMenu->setLayout(RowLayout::create()
        ->setGap(15.f)
        ->setAxisAlignment(AxisAlignment::Start)
    );
    tShufflingMenu->addChild(tShufflingTxt);
    tShufflingMenu->addChild(tShufflingChk);
    tShufflingMenu->setPosition({55.f, 22.f});
    tShufflingMenu->setScale(0.41f);
    tShufflingMenu->setAnchorPoint({0, 0.5});
    tShufflingMenu->updateLayout();
    m_mainLayer->addChild(tShufflingMenu);

    return true;
}

void OptionSelectPopup::refreshLayout() {

    for (auto node : CCArrayExt<CCNode*>(m_optionSlots)) {
        m_scrl->m_contentLayer->removeChild(node);
    }
    m_optionSlots->removeAllObjects();

    m_optionStrs = defaultOptBank;
    const auto loadedOpts = Mod::get()->getSavedValue<SavedOptionStrs>("user-option-codes", SavedOptionStrs{}).juggernaut;
    m_optionStrs.insert(m_optionStrs.end(), loadedOpts.begin(), loadedOpts.end());
    std::sort(m_optionStrs.begin(), m_optionStrs.end(),
        [](const OptionString& a, const OptionString& b) {
            return std::lexicographical_compare(
                a.name.begin(), a.name.end(),
                b.name.begin(), b.name.end(),
                [](char ca, char cb) { return std::tolower(ca) < std::tolower(cb); }
            );
        }
    );

    for (size_t i = 0; i < m_optionStrs.size(); i++) {

        auto testMenu = CCLayerColor::create(ccColor4B(0, 0, static_cast<GLubyte>(m_optionStrs[i].isPreset ? 17 : 0), static_cast<GLubyte>(i % 2 ? 130 : 90)));
        testMenu->setContentSize({m_mainLayer->getContentSize().width, 50.f});
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
        auto testSubheadBtn = CCMenuItemSpriteExtra::create(
            testSubheading,
            this,
            menu_selector(OptionSelectPopup::onCopyOpt));
        testSubheadBtn->setTag(i);
        auto testSubheadBtnMenu = CCMenu::create();
        testSubheadBtnMenu->addChild(testSubheadBtn);
        testSubheadBtnMenu->setAnchorPoint({0.f, 0.f});
        testSubheadBtn->setAnchorPoint({0.f, 1.f});
        testSubheadBtnMenu->setPosition({10.f, testMenu->getContentSize().height - 30.f});
        testSubheadBtnMenu->setScale(0.35f);
        testSubheadBtnMenu->setOpacity(200);
        testMenu->addChild(testSubheadBtnMenu);

        // GJ_selectSongBtn_001.png
        // GJ_selectSongOnBtn_001.png
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
    }

    m_scrl->m_contentLayer->updateLayout();
    m_scrl->moveToTop();
}

void OptionSelectPopup::onToggle(CCObject* object) {
    // nothing needed here at the moment -M
}

void OptionSelectPopup::onCopyOpt(CCObject* object) {
    uint16_t newIndex = object->getTag();
    auto selectedOption = m_optionStrs[newIndex];
    clipboard::write(selectedOption.optExported);
    Notification::create("Copied to clipboard",
        NotificationIcon::None, 0.5f)->show();
}

void OptionSelectPopup::onSelectOption(CCObject* object) {
    uint16_t newIndex = object->getTag();
    auto selectedOption = m_optionStrs[newIndex];
    importSettings(selectedOption.optExported, true);

    // we calculate the option code again incase the option functions made a mistake
    if (menuOptText != nullptr) {
        std::string displayOptStr = "Options: " + exportSettings(getSettings(JFPGen::JFPBiome::Juggernaut));
        menuOptText->setCString(displayOptStr.c_str());
    }
}

void OptionSelectPopup::onClickFolder(CCObject*) {
    file::openFolder(Mod::get()->getSaveDir() / "themes");
}

void OptionSelectPopup::onQuickImport(CCObject* object) {
    OptionStrPopup::create("")->show();
    OptionSelectPopup::onClose(object);
}

void OptionSelectPopup::onShuffle(CCObject* object) {
    std::vector<std::string> codePlaylist;

    for (int i = m_optionSlots->count() - 1; i >= 0; i--) {
        auto targetSection = typeinfo_cast<CCLayerColor*>(m_optionSlots->objectAtIndex(i));
        auto targetChkMenu = typeinfo_cast<CCMenu*>(targetSection->getChildByTag(1));
        if (!targetChkMenu) continue;
        auto targetCheckbox = typeinfo_cast<CCMenuItemToggler*>(targetChkMenu->getChildren()->objectAtIndex(0));
        if (targetCheckbox->isToggled()) {
            codePlaylist.push_back(m_optionStrs[i].optExported);
        };
    }

    if (!codePlaylist.empty()) {
        Mod::get()->setSavedValue<std::vector<std::string>>("opt-0-options-playlist", codePlaylist);

        Notification::create("Shuffle playlist saved successfully",
            NotificationIcon::None, 0.5f)->show();
    } else {
        FLAlertLayer::create("Error", "No presets selected.", "OK")->show();
    }
}

void OptionSelectPopup::onToggleShuffle(CCObject* object) {
    auto chk = typeinfo_cast<CCMenuItemToggler*>(object);
    bool toggled = !chk->isToggled();

    Mod::get()->setSavedValue<bool>("opt-0-shuffling", toggled);
}

void OptionSelectPopup::onAddOptions(CCObject* object) {
    OptionStrPopupDeluxe::create(this)->show();
}

void OptionSelectPopup::onTrash(CCObject* object) {
    auto jfpConfirm = createQuickPopup(
        "JFP",
        "Permanently <cr>delete</c> all selected presets?",
        "No", "Yes",
        [this](bool b1, bool b2) {
            if (b2) {
                auto savedOpts = Mod::get()->getSavedValue<SavedOptionStrs>("user-option-codes", SavedOptionStrs{});
                bool couldNotDeleteSome = false;

                for (int i = m_optionSlots->count() - 1; i >= 0; i--) {
                    auto targetSection = typeinfo_cast<CCLayerColor*>(m_optionSlots->objectAtIndex(i));
                    auto targetChkMenu = typeinfo_cast<CCMenu*>(targetSection->getChildByTag(1));
                    if (!targetChkMenu) continue;
                    auto targetCheckbox = typeinfo_cast<CCMenuItemToggler*>(targetChkMenu->getChildren()->objectAtIndex(0));
                    if (targetCheckbox->isToggled()) {
                        auto& optToRemove = m_optionStrs[i];
                        auto it = std::find_if(savedOpts.juggernaut.begin(), savedOpts.juggernaut.end(),
                            [&optToRemove](const OptionString& opt) { return opt.name == optToRemove.name; });
                        if (it != savedOpts.juggernaut.end()) {
                            savedOpts.juggernaut.erase(it);
                        } else {
                            couldNotDeleteSome = true;
                        }
                        // m_scrl->m_contentLayer->removeChild(targetSection, true);
                        // m_checkboxes.erase(m_checkboxes.begin() + i);
                        // m_optionSlots->removeObjectAtIndex(i);
                    };

                }
                Mod::get()->setSavedValue<SavedOptionStrs>("user-option-codes", savedOpts);
                this->refreshLayout();

                if (couldNotDeleteSome) FLAlertLayer::create("Error", "Default presets cannot be deleted!", "OK")->show();
            }
        });
}

void OptionSelectPopup::onClose(CCObject* object) {
    
    
    this->removeFromParentAndCleanup(true);
}

void OptionSelectPopup::onInfo(CCObject*) {
    const char* info =
        "<cp>Juggernaut Presets</c> are used to save options for quick use.\n\n"
        "The active option code can be refreshed and viewed on the JFP menu screen. Option codes can be copied and shared with friends by clicking on them.\n\n"
        "If you encounter an option code failing to load or made for an old version of JFP, please reconfigure your options and make a new preset.\n\n"
        "The <cb>Shuffle Button</c> creates a playlist containing all checked presets, which is then shuffled through while playing. Setting a new shuffle list overrides your existing list, so be careful before clicking!\n\n"
        "To import a code without adding it to your list, use the <cy>Quick Import</c> button in the bottom right.\n";

    auto infoLayer = MDPopup::create("JFP Presets",
        info,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
    infoLayer->show();
}

OptionSelectPopup* OptionSelectPopup::create(std::string const& text) {
    auto ret = new OptionSelectPopup();
    if (ret->init(400.f, 280.f, "GJ_square05.png") && ret->setup(text)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
