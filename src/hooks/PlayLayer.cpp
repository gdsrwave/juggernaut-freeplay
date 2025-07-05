#include <Geode/Geode.hpp>
#include "../utils/StringGen.hpp"
#include "CreatorLayer.cpp"

using namespace geode::prelude;

#define PREFERRED_HOOK_PRIO (-3999) // https://github.com/RayDeeUx/Decimal-Percentages/blob/3bf08d0f06a9ef2d73c63ea8fa82e7a379a94dc3/src/main.cpp#L12

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("PlayLayer::updateProgressbar", PREFERRED_HOOK_PRIO);
	}

	void onQuit() {
		state = JFPGen::AutoJFP::NotInAutoJFP;
		PlayLayer::onQuit();
	}

	void resetLevel() {
		if (state == JFPGen::AutoJFP::NotInAutoJFP) return PlayLayer::resetLevel();
		else if (state == JFPGen::AutoJFP::JustStarted) {
			state = JFPGen::AutoJFP::PlayingLevelAtt1;
			return PlayLayer::resetLevel();
		}
		else if (state == JFPGen::AutoJFP::JustRestarted) {
			this->m_freezeStartCamera = false; // this var controls whether the camera follows the player at the start
			return PlayLayer::resetLevel();
		}

		auto pl = PlayLayer::get();
		int atts = pl->m_attempts;
		int best = pl->m_level->m_normalPercent;

		auto dir = CCDirector::sharedDirector();
		auto scene = CCScene::create();
		auto size = dir->getWinSize();

		// take screenshot, use this as filler during the 1 frame of transition between playlayers
		// without this there would be an annoying black flash for 1 frame
		auto rt = CCRenderTexture::create(size.width, size.height);
		rt->setPosition(size / 2); // middle of screen
		rt->begin();
		dir->getRunningScene()->visit();
		rt->end();

		scene->addChild(rt);
		dir->replaceScene(scene);
		
		// the original playlayer isnt destroyed until later in the frame when u call replacescene so we have to queue this for the next frame
		queueInMainThread([=]() {
			auto level = GenerateLevelLayer::createGameLevel();
			if (!level) return; // idk what to do here

			state = JFPGen::AutoJFP::JustRestarted;
			
			// important note: resetLevel gets called somewhere within PlayLayer::scene()
			// so its important that the state is JustRestarted by this point
			auto scene = PlayLayer::scene(level, false, false);
			dir->replaceScene(scene);
			
			auto pl = PlayLayer::get();
			pl->startGame(); // gotta call this instantly to prevent the attempt 1 delay
			pl->m_attempts = atts;
			pl->updateAttempts();
			pl->m_level->m_normalPercent = best;
				
			state = JFPGen::AutoJFP::PlayingLevel;

			// necessary to prevent cursor flashing on respawn except it sometimes doesnt work idk
			if (!GameManager::get()->getGameVariable("0024")) PlatformToolbox::hideCursor(); 

			// necessary for compat with mh restart key for some reason
			queueInMainThread([]() {
				if (!GameManager::get()->getGameVariable("0024")) PlatformToolbox::hideCursor(); 
			});
		});
	}

	void startGame() {
		if (state == JFPGen::AutoJFP::PlayingLevel) return; // prevents lag from this func being called twice when restarting
		PlayLayer::startGame();
	}

	void updateProgressbar() {
		if (state == JFPGen::AutoJFP::NotInAutoJFP) {
			PlayLayer::updateProgressbar();
			return;
		}
		const std::string& progressText = m_percentageLabel->getString();
		int playerMeters = (static_cast<int>(m_player1->m_position.x) - 345);
		int length = Mod::get()->getSettingValue<int>("length");

		std::string meterLabel;
		if (playerMeters < -160) {
			meterLabel = "Ready...";
			playerMeters = 0;
		} else if (playerMeters < 0) {
			meterLabel = "GO!!";
			playerMeters = 0;
		} else {
			playerMeters = playerMeters / 30;
			if (playerMeters < 0) playerMeters = 0; // prevent negative meters
			// temp solution
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
				static_cast<float>(playerMeters) / static_cast<float>(length)
			) * m_progressWidth),
			m_progressHeight
		});
		
		m_percentageLabel->setString(meterLabel.c_str());
	}
};