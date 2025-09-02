// Copyright 2025 GDSRWave
#pragma once
#include <map>
#include <string>
#include <vector>

// Sets up the JFP directories and copies .jfpt files to the themes directory
// if needed. If bypass is true, always copy .jfpt files.
void setupJFPDirectories(bool bypass = false);
std::vector<int> getUserSongs();
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
    InAutoTransition = 1,
    JustRestarted = 2,
    PlayingLevel = 3,
};

extern const char* levelCommonBaseSeg;
extern const char* levelBaseSeg;
extern const char* levelStartingBase;
extern const char* levelStartingBase2;
extern const char* lowVis;
extern const char* finishLine;

}  // namespace JFPGen

#endif  // JFP_CONSTANTS_HPP
