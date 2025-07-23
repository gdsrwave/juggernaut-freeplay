// Copyright 2025 GDSRWave
#include <Geode/Geode.hpp>
#include "../utils/Ninja.hpp"
#include "../utils/StringGen.hpp"

using namespace geode::prelude;

#include <Geode/modify/EditorPauseLayer.hpp>
class $modify(EditorPauseLayer) {
    #ifndef __APPLE__
    void onExitEditor(CCObject* obj) {
        jfpActive = false;
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            state = JFPGen::AutoJFP::NotInAutoJFP;
        }
        EditorPauseLayer::onExitEditor(obj);
    }
    void onSave(CCObject* obj) {
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            FLAlertLayer::create("Error",
                "Action not permitted in Dabbink mode.", "OK")->show();
        } else {
            EditorPauseLayer::onSave(obj);
        }
    }
    #else
    void FLAlert_Clicked(FLAlertLayer* p0, bool btnTwo) {
        // determine if the FLAlertLayer being clicked on is the one from onExitNoSave or from isOnSave
        bool isOnExitNoSave = p0->getTag() == 1 && btnTwo;
        bool isOnSave = p0->getTag() == 3 && btnTwo;
        if (isOnSave && state != JFPGen::AutoJFP::NotInAutoJFP) {
            return FLAlertLayer::create("Error",
                "Action not permitted in Dabbink mode.", "OK")->show();
        }
        if (isOnExitNoSave) {
            jfpActive = false;
            if (state != JFPGen::AutoJFP::NotInAutoJFP) {
                state = JFPGen::AutoJFP::NotInAutoJFP;
            }
        }
        EditorPauseLayer::FLAlert_Clicked(p0, btnTwo);
    }
    #endif

    void onExitNoSave(CCObject* obj) {
        if (state != JFPGen::AutoJFP::NotInAutoJFP) {
            EditorPauseLayer::onExitEditor(obj);
        } else {
            EditorPauseLayer::onExitNoSave(obj);
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
