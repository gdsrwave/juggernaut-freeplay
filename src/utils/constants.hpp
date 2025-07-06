#pragma once

#ifndef JFP_CONSTANTS_HPP
#define JFP_CONSTANTS_HPP

extern const int jfpSoundtrack[];
extern const int jfpSoundtrackSize;

namespace JFPGen {

enum class AutoJFP : int {
    NotInAutoJFP = 0,
    JustStarted = 1,
    JustRestarted = 2,
    PlayingLevelAtt1 = 3,
    PlayingLevel = 4,
};

extern const std::string levelBaseSeg;
extern const std::string levelStartingBase;
extern const std::string lowVis;

}
#endif // JFP_CONSTANTS_HPP
