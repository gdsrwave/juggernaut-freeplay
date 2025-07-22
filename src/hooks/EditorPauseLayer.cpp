// Copyright 2025 GDSRWave
#include <Geode/Geode.hpp>
#include "../utils/Ninja.hpp"
#include "../utils/StringGen.hpp"

using namespace geode::prelude;

#include <Geode/modify/EditorPauseLayer.hpp>
class $modify(EditorPauseLayer) {
    void onExitEditor(CCObject* obj) {
        jfpActive = false;
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            state = JFPGen::AutoJFP::NotInAutoJFP;
        }
        EditorPauseLayer::onExitEditor(obj);
    }

    void onExitNoSave(CCObject* obj) {
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            EditorPauseLayer::onExitEditor(obj);
        } else {
            EditorPauseLayer::onExitNoSave(obj);
        }
    }

    void onSave(CCObject* obj) {
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            FLAlertLayer::create("Error",
                "Action not permitted in Dabbink mode.", "OK")->show();
        } else {
            EditorPauseLayer::onSave(obj);
        }
    }

    void onSaveAndPlay(CCObject* obj) {
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            FLAlertLayer::create("Error",
                "Action not permitted in Dabbink mode.", "OK")->show();
        } else {
            EditorPauseLayer::onSaveAndPlay(obj);
        }
    }

    void onSaveAndExit(CCObject* obj) {
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            FLAlertLayer::create("Error",
                "Action not permitted in Dabbink mode.", "OK")->show();
        } else {
            EditorPauseLayer::onSaveAndExit(obj);
        }
    }
};
