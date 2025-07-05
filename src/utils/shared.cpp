#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
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

// bring used namespaces to scope
using namespace geode::prelude;

GJGameLevel* commonLevel = nullptr;

void setupJFPMusic() {
    std::string appdataDir = CCFileUtils::sharedFileUtils()->getWritablePath();
    std::filesystem::path srcPath = Mod::get()->getResourcesDir() / "jfpLoop.mp3";
    std::string dstPath = appdataDir + "jfpLoop.mp3";
    if (!std::filesystem::exists(dstPath)) {
        std::filesystem::copy_file(srcPath, dstPath);
        log::info("Copied JFP loop: {}", dstPath);
    };
}

void setupJFPDirectories(bool bypass = false) {
    auto localPath = CCFileUtils::sharedFileUtils();
    std::string jfpDir = localPath->getWritablePath() + "jfp\\";
    if (!std::filesystem::is_directory(jfpDir)) {
        log::info("Creating JFP directory: {}", jfpDir);
        (void)file::createDirectory(jfpDir);
    }
    std::string themesDir = localPath->getWritablePath() + "jfp\\themes\\";
    // Create the directory if it doesn't exist
    bool contFlag = bypass;
    if (!std::filesystem::is_directory(themesDir)) {
        (void)file::createDirectory(themesDir);
        contFlag = true;
    }
    if (contFlag) {
        log::info("Loading .jfpt files into themes directory: {}", themesDir);
        // Source directory for .jfpt files
        std::filesystem::path srcDir = Mod::get()->getResourcesDir();

        for (const auto& fileName : std::filesystem::directory_iterator(srcDir)) {
            auto fileStr = fileName.path().filename().string();
            if (fileStr.size() >= 5 && fileStr.substr(fileStr.size() - 5) == ".jfpt") {
                std::filesystem::path srcPath = srcDir / fileStr;
                std::string dstPath = themesDir + fileStr;

                try {
                    if (std::filesystem::exists(dstPath)) {
                        if (bypass) {
                            std::filesystem::copy_file(
                                srcPath, dstPath,
                                std::filesystem::copy_options::overwrite_existing
                            );
                            log::info("Overwrote existing theme file: {}", dstPath);
                        } else {
                            log::info("Theme file already exists and bypass is false: {}", dstPath);
                        }
                    } else {
                        std::filesystem::copy_file(srcPath, dstPath);
                        log::info("Copied theme file: {}", dstPath);
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    log::error("Failed to copy theme file {}: {}", dstPath, e.what());
                }
            }
        }
    }
}