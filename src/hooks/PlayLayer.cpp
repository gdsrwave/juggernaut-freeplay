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
            auto fileUtils = CCFileUtils::sharedFileUtils();
            auto bgmPath = std::string(fileUtils->getWritablePath()) + "jfpLoop.mp3";
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
        int playerMeters = (static_cast<int>(m_player1->m_position.x) - 345);
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
                static_cast<float>(playerMeters) / static_cast<float>(length))
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
    }
};
