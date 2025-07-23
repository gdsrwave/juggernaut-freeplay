// Copyright 2025 GDSRWave
#include <fmt/core.h>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <random>
#include <cmath>
#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include "utils/StringGen.hpp"
#include "ui/JFPMenuLayer.hpp"
#include "utils/shared.hpp"

using namespace geode::prelude;
using namespace gmd;

$on_mod(Loaded) {
    setupJFPDirectories();
    setupJFPMusic();
}
