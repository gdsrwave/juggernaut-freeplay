#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include <random>
#include <cmath>
#include <fmt/core.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <filesystem>
#include <map>
#include <vector>
#include <string>
#include <array>
#include <utility>
#include "constants.hpp"

using namespace geode::prelude;

namespace JFPGen {

void orientationShift(int prevO[11], int newO);
void exportLvlStringGMD(std::filesystem::path const& path, std::string ld1);
int convertSpeed(const std::string& speed);

std::string jfpMainStringGen(bool compress, AutoJFP state = AutoJFP::NotInAutoJFP);
std::string jfpNewStringGen(bool compress, AutoJFP state = AutoJFP::NotInAutoJFP);

} // namespace JFPGen


#ifndef JFPSTATE
#define JFPSTATE

extern JFPGen::AutoJFP state;

extern bool jfpActive;

#endif