#pragma once


// Sets up the JFP directories and copies .jfpt files to the themes directory if needed.
// If bypass is true, always copy .jfpt files.
void setupJFPDirectories(bool bypass = false);
void setupJFPMusic();

extern GJGameLevel* commonLevel;

#ifndef JFP_CONSTANTS_HPP
#define JFP_CONSTANTS_HPP

extern const int jfpSoundtrack[];
extern const int jfpSoundtrackSize;

extern const std::map<std::string, std::string> defaultKbank;

namespace JFPGen {

enum class AutoJFP : int {
    NotInAutoJFP = 0,
    JustStarted = 1,
    JustRestarted = 2,
    PlayingLevelAtt1 = 3,
    PlayingLevel = 4,
};

extern const std::string levelCommonBaseSeg;
extern const std::string levelBaseSeg;
extern const std::string levelStartingBase;
extern const std::string levelStartingBase2;
extern const std::string lowVis;
extern const std::string finishLine;


}
#endif // JFP_CONSTANTS_HPP
