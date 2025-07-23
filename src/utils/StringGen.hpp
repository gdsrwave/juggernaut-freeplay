// Copyright 2025 GDSRWave
#pragma once

#include <fmt/core.h>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <filesystem>
#include <random>
#include <cmath>
#include <map>
#include <vector>
#include <string>
#include <array>
#include <utility>
#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include "./shared.hpp"
#include "./Ninja.hpp"

using namespace geode::prelude;

namespace JFPGen {

static void orientationShift(int prevO[11], int newO);
void exportLvlStringGMD(std::filesystem::path const& path, std::string ld1);
static int convertSpeed(JFPGen::SpeedChange speed);

std::string jfpStringGen(bool compress);
std::string jfpNewStringGen(LevelData ldata);
std::string colorStringGen();
std::string kStringGen();

}  // namespace JFPGen

#ifndef JFPSTATE
#define JFPSTATE

extern JFPGen::AutoJFP state;

extern bool jfpActive;

#endif
