#include <Geode/Geode.hpp>
#include "../utils/StringGen.hpp"
#include "../ui/JFPMenuLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/GameManager.hpp>
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* p0) {
        if (jfpActive) {
            CCDirector::sharedDirector()->replaceScene(JFPMenuLayer::scene());
            jfpActive = false;
        } else {
            GameManager::returnToLastScene(p0);
        }
    }
};