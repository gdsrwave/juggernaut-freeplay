#include <Geode/Geode.hpp>
#include <alphalaneous.editortab_api/include/EditorTabs.hpp>
#include "../utils/Theming.hpp"

using namespace geode::prelude;

#include <Geode/modify/EditorUI.hpp>
class $modify(EditorUI) {

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        EditorTabs::addTab(this, TabType::EDIT, "themetools"_spr,
                [](EditorUI* ui, CCMenuItemToggler* toggler) -> CCNode* {

            auto arr = CCArray::create();
            // make basic copy selected btn
            auto basicBtnSpr = CCSprite::create("GJ_button_01-uhd.png");
            basicBtnSpr->addChildAtPosition(
                CCSprite::createWithSpriteFrameName("copyIcon.png"_spr),
                Anchor::Center,
                CCPointZero
            );
            auto basicBtn = CCMenuItemExt::createSpriteExtra(
                basicBtnSpr,
                [=](auto) {
                    auto selectedObj = ui->getSelectedObjects();
                    std::string res = "# <definition or condition> #\n";
                    for (int i = 0; i < selectedObj->count(); i++) {
                        GameObject* obj = static_cast<GameObject*>(selectedObj->objectAtIndex(i));
                        res += ThemeGen::nuke155(obj->getSaveString(ui->m_editorLayer));
                        res += ";\n";
                    }
                    res += "# end define #";
                    clipboard::write(res);
                }
            );
            basicBtn->setID("jfpt-copy-btn");
            arr->addObject(basicBtn);
            

            // make advanced export btn
            // auto label = CCLabelBMFont::create("meow", "bigFont.fnt");
            // arr->addObject(label);


            CCLabelBMFont* textLabel = CCLabelBMFont::create("JFP", "bigFont.fnt");
            textLabel->setScale(0.4f);

            EditorTabUtils::setTabIcon(toggler, textLabel);

            return EditorTabUtils::createEditButtonBar(arr, ui);
        }, [](EditorUI*, bool state, CCNode*) { 
            // nothing necessary here yet.. -M
        });

        return true;
    }
};


            // auto onCopyBtnClicked = [](EditorUI* ui) {
            //     auto selectedObj = ui->getSelectedObjects();
            //     std::string res;
            //     for (int i = 0; i < selectedObj->count(); i++) {
            //         GameObject* obj = static_cast<GameObject*>(selectedObj->objectAtIndex(i));
            //         if (!res.empty()) res += "\n";
            //         res += obj->getSaveString(ui->m_editorLayer);
            //     }
            //     clipboard::write(res);
            // };

            // auto basicBtn = CCMenuItemExt::createSpriteExtra(
            //     basicBtnSpr,
            //     onCopyBtnClicked(ui) }
            // );