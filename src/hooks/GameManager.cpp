// Copyright 2025 GDSRWave
#include <Geode/Geode.hpp>
#include "../utils/StringGen.hpp"
#include "../ui/JFPMenuLayer.hpp"
#include "../ui/JFPScreenshotLayer.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

#include <Geode/modify/GameManager.hpp>
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* p0) {
        if (jfpActive) {
            // auto jlTransition = CCTransitionFade::create(
            //     0.5, JFPScreenshotLayer::scene());
            jfpActive = false;
            CCDirector::sharedDirector()->replaceScene(JFPScreenshotLayer::scene());
        } else {
            GameManager::returnToLastScene(p0);
        }
    }
};
