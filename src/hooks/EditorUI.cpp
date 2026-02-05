// Copyright 2025 GDSRWave
#include <string>
#include <Geode/Geode.hpp>
#include <alphalaneous.editortab_api/include/EditorTabs.hpp>
#include "../utils/Theming.hpp"

using namespace geode::prelude;

void onCopyBtns(EditorUI* ui,
        ThemeGen::OMType omType = ThemeGen::OMType::None, ThemeGen::JFPTExtraTypes specialOR = ThemeGen::JFPTExtraTypes::None) {
    auto selectedObj = ui->getSelectedObjects();
    std::string res = "# if|define #\n";
    if (specialOR == ThemeGen::JFPTExtraTypes::Pattern) res = "# pattern (starting X coordinate) (units until repeat) #\n";
    for (auto obj : CCArrayExt<GameObject*>(selectedObj)) {
        res += ThemeGen::handleRawBlock(obj->getSaveString(ui->m_editorLayer),
            omType, specialOR);
        res += ";\n";
    }
    res += specialOR == ThemeGen::JFPTExtraTypes::Pattern ? "# end pattern #" : "# end if|define #";
    clipboard::write(res);
    Notification::create(
        "Copied JFPT block to clipboard", NotificationIcon::Info, 1.f)->show();
}

void onCopyColorBtn(EditorUI* ui) {
    auto gjem = ui->m_editorLayer->m_levelSettings->m_effectManager;
    std::string res = "";
    auto colorActions = gjem->getAllColorActions();
    for (auto color : CCArrayExt<ColorAction*>(colorActions)) {
        if (color->m_colorID == 1005 || color->m_colorID == 1006) continue;
        if (!res.empty()) res += "\n\n";
        res += "# color #\n";
        res += ThemeGen::handleColor(color);
        res += "# end color #";
    }
    clipboard::write(res);
    Notification::create(
        "Copied JFPT block to clipboard", NotificationIcon::Info, 1.f)->show();
}

void onThemeInfoButton() {
    const char* message =
        "Utilities assisting in the creation of <cp>JFP Themes</c>.\n\n"
        "The <cl>base exporter</c>, listed first, exports blocks from their absolute position. Useful for the base definition, but not much else.\n\n"
        "The <co>floor</c> and <co>ceiling exporters</c>, listed second and third, export segments of the corridor.\n"
        "- These are used for both \"if\" and \"define\" sections in JFPT, so you'll have to manually specify which after pasting into your theme.\n"
        "- <cr>Move your reference point to (0,0) before using these buttons.</c> These blocks will have relative coordinates depending on your generation, "
        "so they must be exported relative to the editor's origin point.\n"
        "- Tip: The \"reference point\" you place at (0,0) for corridor segments is the center of your slope (or, for miniwave, the center of its top half).\n\n"
        "The <cp>\"special objects\" exporter</c>, listed fourth, accounts for relative coordinates, rotation and scale.\n"
        "- It is used for definitions of spikes, speed changes, gravity portals, etc. Valid definitions listed below.\n\n"
        "The <cg>pattern exporter</c>, listed fifth, exports blocks as patterns. Patterns have a relative X coordinate and an absolute Y coordinate.\n"
        "- This is used for pattern sections only; after pasting, you will have to specify starting coords and a repetition interval.\n\n"
        "The <cr>c</c><co>o</c><cy>l</c><cg>o</c><cl>r</c><cb>s</c> <cy>exporter</c>, listed sixth, exports all the colors from the opened level.\n\n"
        "There are currently no utilities for metadata or k sections. You also have to set definitions and figure out matching logic yourself.\n\n"
        "Definition Types:\n- base\n- start (-mini)\n- enddown (-mini)\n- endup (-mini)\n- slope (-mini)\n- spike (-mini)\n"
        "- blue-portal (-mini)\n- yellow-portal (-mini)\n- big-portal (-mini)\n- mini-portal (-mini)\n"
        "- speed-05x (-mini)\n- speed-1x (-mini)\n- speed-2x (-mini)\n- speed-3x (-mini)\n- speed-4x (-mini)\n- corridorblock\n- corridorblock-fill";

    auto infoLayer = MDPopup::create("Theme Creator Tools",
        message,
        "OK"
    );
    infoLayer->setID("jfpopt-info-layer"_spr);
    infoLayer->setScale(1.1f);
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

            // special override export btn
            auto specialORBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            specialORBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("specialORIcon.png"_spr),
                Anchor::Center,
                CCPointZero);

            auto specialORBtn = CCMenuItemExt::createSpriteExtra(
                specialORBtnSpr,
                [=](auto) { onCopyBtns(ui, ThemeGen::OMType::Floor, ThemeGen::JFPTExtraTypes::Special); });

            specialORBtn->setID("jfpt-specialcpy-btn");
            arr->addObject(specialORBtn);

            // pattern export btn
            auto patternBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            patternBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("patternIcon.png"_spr),
                Anchor::Center,
                CCPointZero
            );
            auto patternBtn = CCMenuItemExt::createSpriteExtra(
                patternBtnSpr,
                [=](auto) { onCopyBtns(ui, ThemeGen::OMType::Floor, ThemeGen::JFPTExtraTypes::Pattern); }
            );
            patternBtn->setID("jfpt-patterncpy-btn");
            arr->addObject(patternBtn);

            // pattern export btn
            auto colorsBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            colorsBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("colorsIcon.png"_spr),
                Anchor::Center,
                CCPointZero
            );
            auto colorsBtn = CCMenuItemExt::createSpriteExtra(
                colorsBtnSpr,
                [=](auto) { onCopyColorBtn(ui); }
            );
            colorsBtn->setID("jfpt-colorscpy-btn");
            arr->addObject(colorsBtn);

            // info btn
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
