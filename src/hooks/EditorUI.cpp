// Copyright 2025 GDSRWave
#include <string>
#include <Geode/Geode.hpp>
#include <alphalaneous.editortab_api/include/EditorTabs.hpp>
#include "../utils/Theming.hpp"

using namespace geode::prelude;

void onCopyBtns(EditorUI* ui,
        ThemeGen::OMType omType = ThemeGen::OMType::None) {
    auto selectedObj = ui->getSelectedObjects();
    std::string res = "# <if|define> #\n";
    for (auto obj : CCArrayExt<GameObject*>(selectedObj)) {
        res += ThemeGen::handleRawBlock(obj->getSaveString(ui->m_editorLayer),
            omType);
        res += ";\n";
    }
    res += "# end <if|define> #";
    clipboard::write(res);
    Notification::create(
        "Copied JFPT block to clipboard", NotificationIcon::Info, 1.f)->show();
}

void onThemeInfoButton() {
    std::string message = "Definition Types:\n"
            "base\nenddown\nenddown-mini\nendup\nendup-mini\nstart\nstart-mini";
    auto infoLayer = FLAlertLayer::create(nullptr, "JFP",
        message.c_str(),
        "OK",
        nullptr,
        200.f);
    infoLayer->setID("jfp-themeinfo-layer"_spr);
    infoLayer->show();
}

#include <Geode/modify/EditorUI.hpp>
class $modify(EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        bool ttb = Mod::get()->getSavedValue<bool>("opt-u-theme-creator-tools");
        if (!ttb) return true;

        EditorTabs::addTab(this, TabType::EDIT, "themetools"_spr,
                [](EditorUI* ui, CCMenuItemToggler* toggler) -> CCNode* {
            auto arr = CCArray::create();
            // make basic copy selected btn
            auto basicBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            basicBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("copyIcon.png"_spr),
                Anchor::Center,
                CCPointZero);

            auto basicBtn = CCMenuItemExt::createSpriteExtra(
                basicBtnSpr,
                [=](auto) { onCopyBtns(ui); });
            basicBtn->setID("jfpt-copy-btn");
            arr->addObject(basicBtn);

            // floor seg export btn
            auto floorBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            floorBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("floorIcon.png"_spr),
                Anchor::Center,
                CCPointZero);

            auto floorBtn = CCMenuItemExt::createSpriteExtra(
                floorBtnSpr,
                [=](auto) { onCopyBtns(ui, ThemeGen::OMType::Floor); });
            floorBtn->setID("jfpt-floorcpy-btn");
            arr->addObject(floorBtn);

            // ceiling seg export btn
            auto ceilBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            ceilBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("ceilingIcon.png"_spr),
                Anchor::Center,
                CCPointZero);

            auto ceilBtn = CCMenuItemExt::createSpriteExtra(
                ceilBtnSpr,
                [=](auto) { onCopyBtns(ui, ThemeGen::OMType::Ceiling); });

            ceilBtn->setID("jfpt-ceilcpy-btn");
            arr->addObject(ceilBtn);

            // // pattern export btn
            // auto ceilBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            // ceilBtnSpr->addChildAtPosition(
            //     CCSprite::createWithSpriteFrameName("patternIcon.png"_spr),
            //     Anchor::Center,
            //     CCPointZero
            // );
            // auto ceilBtn = CCMenuItemExt::createSpriteExtra(
            //     ceilBtnSpr,
            //     [=](auto) { onCopyBtns(ui, ThemeGen::OMType::Ceiling); }
            // );
            // ceilBtn->setID("jfpt-ceilcpy-btn");
            // arr->addObject(ceilBtn);

            // ceiling seg export btn
            auto infoBtnSpr = CCSprite::createWithSpriteFrameName(
                "blueBg.png"_spr);
            infoBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("eInfoIcon.png"_spr),
                Anchor::Center,
                CCPointZero);

            auto infoBtn = CCMenuItemExt::createSpriteExtra(
                infoBtnSpr,
                [=](auto) { onThemeInfoButton(); });

            infoBtn->setID("jfpt-theme-info-btn");
            arr->addObject(infoBtn);

            CCLabelBMFont* textLabel = CCLabelBMFont::create(
                "JFP", "bigFont.fnt");
            textLabel->setScale(0.4f);

            EditorTabUtils::setTabIcon(toggler, textLabel);

            return EditorTabUtils::createEditButtonBar(arr, ui);
        }, [](EditorUI*, bool state, CCNode*) {
            // nothing necessary here yet.. -M
        });

        return true;
    }
};
