// Copyright 2025 GDSRWave
#include <algorithm>
#include <string>
#include <Geode/Geode.hpp>
#include "../utils/StringGen.hpp"
#include "CreatorLayer.cpp"
#include "../ui/JFPMenuLayer.hpp"

using namespace geode::prelude;

// https://github.com/RayDeeUx/Decimal-Percentages/blob/3bf08d0f06a9ef2d73c63ea8fa82e7a379a94dc3/src/main.cpp#L12
#define PREFERRED_HOOK_PRIO (-3999)

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority(
            "PlayLayer::updateProgressbar", PREFERRED_HOOK_PRIO);
    }

    void onQuit() {
        if (state == JFPGen::AutoJFP::NotInAutoJFP) {
            PlayLayer::onQuit();
            return;
        }

        auto storedState = state;
        att1 = false;
        if (state == JFPGen::AutoJFP::JustRestarted) {
            state = JFPGen::AutoJFP::InAutoTransition;
        } else {
            state = JFPGen::AutoJFP::NotInAutoJFP;
        }

        PlayLayer::onQuit();

        if (storedState != JFPGen::AutoJFP::NotInAutoJFP &&
            !GameManager::sharedState()->getGameVariable("0122") &&
            CCDirector::sharedDirector()->getRunningScene() != nullptr) {
            auto bgmPath = (Mod::get()->getResourcesDir() / "jfpLoop.mp3").string();
            auto* engine = FMODAudioEngine::get();
            if (engine) {
                engine->playMusic(bgmPath, true, 1.0f, 1);
            }
        }
    }

    void resetLevel() {
        if (state == JFPGen::AutoJFP::NotInAutoJFP) {
            return PlayLayer::resetLevel();
        } else if (state == JFPGen::AutoJFP::InAutoTransition) {
            if (att1) state = JFPGen::AutoJFP::InAutoTransitionAtt1;
            return PlayLayer::resetLevel();
        } else if (state == JFPGen::AutoJFP::PlayingLevel || state == JFPGen::AutoJFP::InAutoTransitionAtt1) {
            state = JFPGen::AutoJFP::JustRestarted;
        }

        globalAtt = m_attempts;

        PlayLayer::onQuit();
    }

    void startGame() {
        if (state == JFPGen::AutoJFP::NotInAutoJFP) {
            PlayLayer::startGame();
            return;
        }
        // prevents lag from this func being called twice when restarting
        if (state == JFPGen::AutoJFP::PlayingLevel) return;
        PlayLayer::startGame();
        state = JFPGen::AutoJFP::PlayingLevel;
    }

    void updateProgressbar() {
        if (state == JFPGen::AutoJFP::NotInAutoJFP) {
            PlayLayer::updateProgressbar();
            return;
        }

        const std::string& progressText = m_percentageLabel->getString();
        int playerMeters = (static_cast<int>(m_player1->m_position.x) - Mod::get()->getSavedValue<uint32_t>("opt-0-starting-dist", 345));
        int length = Mod::get()->getSavedValue<int>("opt-0-length");

        std::string meterLabel;
        if (playerMeters < -160) {
            meterLabel = "Ready...";
            playerMeters = 0;
        } else if (playerMeters < 0) {
            meterLabel = "GO!!";
            playerMeters = 0;
        } else {
            playerMeters = playerMeters / 30;
            if (playerMeters < 0) playerMeters = 0;
            if (playerMeters > length) {
                playerMeters = length;
            }
            meterLabel = fmt::format("{}m", playerMeters);
        }

        m_progressFill->setTextureRect({
            0.0f,
            0.0f,
            std::min(m_progressWidth, std::max(
                0.001f,
                length > 0 ? static_cast<float>(playerMeters) / static_cast<float>(length) : 0.0f)
            * m_progressWidth),
            m_progressHeight
        });

        m_percentageLabel->setString(meterLabel.c_str());
    }

    void onEnterTransitionDidFinish() {
        PlayLayer::onEnterTransitionDidFinish();
        if (state == JFPGen::AutoJFP::NotInAutoJFP || att1) {
            return;
        }

        PlayLayer::startGame();
        m_attempts = globalAtt;
        PlayLayer::updateAttempts();
        m_level->m_normalPercent = 0;
    }

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        if (state == JFPGen::AutoJFP::NotInAutoJFP) {
            PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
            return;
        }

        int playerMeters = (static_cast<int>(m_player1->m_position.x) - static_cast<int>(Mod::get()->getSavedValue<uint32_t>("opt-0-starting-dist", 345))) / 30;
        int length = Mod::get()->getSavedValue<int>("opt-0-length");

        if (playerMeters < 0) playerMeters = 0;
        if (playerMeters > length) {
            playerMeters = length;
        }

        if (playerMeters > globalBestM) {
            globalBestM = playerMeters;
            PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);

            // https://github.com/RayDeeUx/Decimal-Percentages/blob/3bf08d0f06a9ef2d73c63ea8fa82e7a379a94dc3/src/main.cpp#L251
            queueInMainThread([this, playerMeters] {
                for (auto obj : CCArrayExt<CCNode*>(this->getChildren())) {
                    // new best popup is Z 100; this may not be the safest check
                    if (obj->getZOrder() != 100) continue;

                    for (auto newBestObj : CCArrayExt<CCNode*>(obj->getChildren())) {
                        CCLabelBMFont* label = typeinfo_cast<CCLabelBMFont*>(newBestObj);

                        if (!label) continue;
                        const auto& labelString = static_cast<std::string>(label->getString());

                        // add seed and options after finding label
                        auto windowDim = CCDirector::sharedDirector()->getWinSize();

                        uint32_t globalSeed = Mod::get()->getSavedValue<uint32_t>("global-seed", 0);
                        std::string globalOptStr = Mod::get()->getSavedValue<std::string>("global-option-str");
                        std::string displaySeed = (globalSeed ? geode::utils::numToString(globalSeed) : "Unknown") +
                            ", " + (globalOptStr != "" ? globalOptStr : "Unknown");
                        auto seedTxt = CCLabelBMFont::create(displaySeed.c_str(), "bigFont.fnt");
                        seedTxt->setAnchorPoint({0.5f, 0.5f});
                        seedTxt->setPosition({0, -5});
                        seedTxt->setScale(0.3f);
                        seedTxt->setOpacity(127);
                        obj->addChild(seedTxt);

                        return label->setString(fmt::format("{m}m", fmt::arg("m", playerMeters)).c_str());
                    }
                }
            });
        }
    }
};
