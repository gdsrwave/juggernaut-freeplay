#include <Geode/Geode.hpp>
#include "../utils/StringGen.hpp"
#include "../ui/JFPMenuLayer.hpp"
#include "../utils/shared.hpp"

using namespace geode::prelude;

#include <Geode/modify/GameManager.hpp>
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* p0) {
        if (jfpActive) {
		    auto jlTransition = CCTransitionFade::create(0.5, JFPMenuLayer::scene());
            CCDirector::sharedDirector()->replaceScene(jlTransition);

            jfpActive = false;
        } else {
            GameManager::returnToLastScene(p0);
        }
    }
};