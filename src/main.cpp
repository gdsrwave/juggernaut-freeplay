#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/cocos/platform/CCFileUtils.h>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include <random>
#include <cmath>
#include <filesystem>
#include <fmt/core.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include "utils/constants.hpp"
#include "utils/StringGen.hpp"
#include "ui/JFPMenuLayer.hpp"

// bring used namespaces to scope
using namespace geode::prelude;
using namespace gmd;


$on_mod(Loaded) {
    auto localPath = CCFileUtils::sharedFileUtils();
    std::string jfpDir = localPath->getWritablePath() + "jfp\\";
    if (!std::filesystem::is_directory(jfpDir)) {
        log::info("Creating JFP directory: {}", jfpDir);
        file::createDirectory(jfpDir);
    }
    std::string themesDir = localPath->getWritablePath() + "jfp\\themes\\";
    log::info("Themes directory: {}", themesDir);
    // Create the directory if it doesn't exist
    if (!std::filesystem::is_directory(themesDir)) {
        file::createDirectory(themesDir);

        // Source directory for .jfpt files
        std::filesystem::path srcDir = Mod::get()->getResourcesDir();

        for (const auto& fileName : std::filesystem::directory_iterator(srcDir)) {
            auto fileStr = fileName.path().filename().string();
            if (fileStr.size() >= 5 && fileStr.substr(fileStr.size() - 5) == ".jfpt") {
                std::filesystem::path srcPath = srcDir / fileStr;
                std::string dstPath = themesDir + fileStr;
                std::filesystem::copy(srcPath, dstPath);
            }
        }
    }
}