// Copyright 2025 GDSRWave
#include <string>
#include <Geode/Geode.hpp>
#include "../utils/Ninja.hpp"

using namespace geode::prelude;

#include <Geode/modify/EndLevelLayer.hpp>
class $modify(EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();
        if (state == JFPGen::AutoJFP::NotInAutoJFP) return;

        auto timeLabel = static_cast<CCLabelBMFont*>(
            this->getChildByIDRecursive("jumps-label"));

        auto* playLayer = GameManager::sharedState()->getPlayLayer();
        int playerMeters = (
            static_cast<int>(playLayer->m_player1->m_position.x) - 345);
        int length = Mod::get()->getSavedValue<int>("opt-0-length");

        std::string meterLabel;
        playerMeters = playerMeters / 30;
        if (playerMeters < 0) playerMeters = 0;
        if (playerMeters > length) {
            playerMeters = length;
        }
        meterLabel = fmt::format("Distance: {}m", playerMeters);
        timeLabel->setString(meterLabel.c_str());
    }
};
