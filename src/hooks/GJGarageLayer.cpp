// Copyright 2025 GDSRWave
#include <Geode/Geode.hpp>
#include "../utils/StringGen.hpp"
#include "../ui/JFPMenuLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/GJGarageLayer.hpp>
class $modify(GJGarageLayer) {
    void onBack(CCObject* sender) {
        if (jfpActive) {
            CCDirector::sharedDirector()->popSceneWithTransition(
                0.5f, PopTransition::kPopTransitionFade);
            jfpActive = false;
        } else {
            GJGarageLayer::onBack(sender);
        }
    }
};
