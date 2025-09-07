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
    InAutoTransitionAtt1 = 4,
};

extern const char* levelCommonBaseSeg;
extern const char* levelBaseSeg;
extern const char* levelStartingBase;
extern const char* levelStartingBase2;
extern const char* lowVis;
extern const char* finishLine;

extern const std::map<int, std::string> SpeedChangeLabel;
extern const std::map<int, std::string> VisibilityLabel;
extern const std::map<int, std::string> ColorModeLabel;
extern const std::map<int, std::string> CorridorRulesLabel;
extern const std::map<int, std::string> DifficultiesLabel;
extern const std::map<int, std::string> PortalsLabel;
extern const std::map<int, std::string> PortalInputsLabel;
extern const std::map<int, std::string> PlacementBySizeLabel;
extern const std::map<int, std::string> StartingSizeLabel;
extern const std::map<int, std::string> MusicSourceLabel;
extern const std::map<int, std::string> SizeTransitionsLabel;
extern const std::map<int, std::string> JFPBiomeLabel;

}  // namespace JFPGen

#endif  // JFP_CONSTANTS_HPP
