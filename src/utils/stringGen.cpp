// Copyright 2025 GDSRWave
#include "StringGen.hpp"
#include <fmt/core.h>
#include <fmt/args.h>
#include <random>
#include <string>
#include <cmath>
#include <Geode/Geode.hpp>
#include "./shared.hpp"
#include "./Theming.hpp"
#include "./Ninja.hpp"
#include "./OptionStr.hpp"

using namespace geode::prelude;

bool jfpActive = false;  // used in GameManager.cpp to check if a menu is open over JFPMenuLayer

namespace JFPGen {

// Legacy
static void orientationShift(int prevO[11], int newO) {
    for (int i = 0; i < 10; i++) {
        prevO[i] = prevO[i+1];
    }
    prevO[10] = newO;
}

void exportLvlStringGMD(std::filesystem::path const& path, std::string ld1) {
    auto lvlData = ByteVector(ld1.begin(), ld1.end());
    (void) file::writeBinary(path, lvlData);
}

std::string jfpPackString(const std::string& level, uint32_t seed,
        JFPSong song, bool compress) {
    std::string b64;
    if (compress) b64 = ZipUtils::compressString(level, true, 0);
    else b64 = level;
    std::string desc = fmt::format("ninja v1 {}", exportSettings(getSettings(JFPGen::JFPBiome::Juggernaut)));

    // double encoding might be unnecessary according to gmd-api source?
    desc = ZipUtils::base64URLEncode(ZipUtils::base64URLEncode(desc));
    b64.erase(std::find(b64.begin(), b64.end(), '\0'), b64.end());
    desc.erase(std::find(desc.begin(), desc.end(), '\0'), desc.end());

    std::string levelString = fmt::format("<k>kCEK</k><i>4</i><k>k2</k><s>JFP {title}"
        "</s><k>k3</k><s>{desc}</s><k>k4</k><s>{b64}</s><k>k45</k><i>{song}</i><k>k13"
        "</k><t/><k>k21</k><i>2</i><k>k50</k><i>35</i>",
    fmt::arg("desc", desc),
    fmt::arg("b64", b64),
    fmt::arg("title", geode::utils::numToString(seed)),
    fmt::arg("song", song.id));

    return levelString;
}

std::string jfpStringGen(bool compress) {
    kBank = defaultKbank;
    colorBank.clear();
    colorBank.insert(colorBank.end(), defaultColorBank.begin(), defaultColorBank.end());

    if (Mod::get()->getSavedValue<bool>("opt-0-theme-shuffling", false)) {
        std::mt19937 themeRNG(Mod::get()->getSavedValue<uint32_t>("global-seed", 1));

        auto modSavePath = Mod::get()->getSaveDir();
        auto themesDir = modSavePath / "themes" / "shuffle";
        if (!std::filesystem::is_directory(themesDir)) setupJFPDirectories();

        std::vector<std::string> themeFiles;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(themesDir)) {
            if (entry.is_regular_file()) {
                auto filename = geode::utils::string::pathToString(std::filesystem::relative(entry.path(), modSavePath / "themes"));
                if (filename.size() <= 5 || filename.substr(filename.size() - 5) != ".jfpt") {
                    continue;
                }
                std::string numPart = filename.substr(0, filename.size() - 5);
                themeFiles.push_back(filename);
            }
        }

        std::string newTheme = "";
        if (!themeFiles.empty()) newTheme = themeFiles[themeRNG() % themeFiles.size()];
        Mod::get()->setSavedValue<std::string>("active-theme", newTheme);
    }

    std::string themeName = Mod::get()->getSavedValue<std::string>("active-theme");
    ThemeGen::parseThemeMetaopts(themeName);

    LevelData ldata = generateJFPLevel();
    if (ldata.biomes.empty()) return "";
 
    std::string themeString = ThemeGen::parseTheme(
        Mod::get()->getSavedValue<std::string>("active-theme"), ldata);

    std::string levelString = jfpNewStringGen(ldata);
    levelString = colorStringGen() + kStringGen() + levelString + themeString;

    return jfpPackString(levelString, ldata.seed, ldata.biomes[0].song, compress);
}

std::string kStringGen() {
    std::string result = ",";
    for (const auto& [key, value] : kBank) {
        result += key + "," + value + ",";
    }
    result += ";";
    return result;
}

std::string colorStringGen() {
    std::string result = "kS38,";

    for (const auto& color : colorBank) {
        result += fmt::format(
            "1_{r}_2_{g}_3_{b}_11_255_12_255_13_255_4_-1_6_{slot}_15_1_18_0_8_1",
            fmt::arg("r", color.rgb[0]),
            fmt::arg("g", color.rgb[1]),
            fmt::arg("b", color.rgb[2]),
            fmt::arg("slot", color.slot));
        if (color.blending) result += "_5_1";
        if (color.opacity <= 1.0) {
            result += fmt::format("_7_{}", color.opacity);
        } else {
            result += "_7_1";
        }
        if (color.copyColor >= 0) {
            result += fmt::format("_9_{}", color.copyColor);
        }
        if (!color.special.empty()) {
            result += "_" + color.special;
        }
        result += "|";
    }

    return result;
}

std::string jfpNewStringGen(LevelData ldata) {
    if (ldata.biomes.empty()) return "";

    uint32_t startingDist = Mod::get()->getSavedValue<uint32_t>("opt-0-starting-dist");
    int scXoff = Mod::get()->getSavedValue<int>("opt-0-starting-sc-x", -90);

    std::string levelBuildSeg2 = fmt::format(
        "1,{speedID},2,{sd_1},3,165,13,1,57,915,64,1,67,1;",
        fmt::arg("speedID", convertSpeed(ldata.biomes[0].options.startingSpeed)),
        fmt::arg("sd_1", static_cast<int>(startingDist) + scXoff)
    );
    const bool cornerPieces = Mod::get()->getSavedValue<bool>("opt-0-add-corner-pieces");
    const bool hideIcon = Mod::get()->getSavedValue<bool>("opt-0-hide-icon");

    std::string level = levelCommonBaseSeg;
    if (!overrideBankS["base"].standard.active || overrideBankS["base"].standard.keep) level += levelBaseSeg;
    level += levelBuildSeg2;

    const auto& biome = ldata.biomes[0];
    const auto& opts = biome.options;
    int x = biome.x_initial;
    int y = biome.y_initial;
    int y_swing = 0;
    const int corridorHeight = opts.corridorHeight;
    const int corridorHeightM = biome.options.typeA
        ? corridorHeight + 30
        : corridorHeight;
    bool spikeSide = false;

    int currentCH = biome.options.startingMini ? corridorHeightM : corridorHeight;

    if (opts.startingMini) {
        if ((!overrideBankS["start"].mini.active || overrideBankS["start"].mini.keep) &&
                (!overrideBankS["slope"].mini.active || overrideBankS["slope"].mini.keep)) {
            fmt::dynamic_format_arg_store<fmt::format_context> args;
            args.push_back(fmt::arg("ch_1", 90 + currentCH));
            args.push_back(fmt::arg("ch_2", 150 + currentCH));
            args.push_back(fmt::arg("ch_3", 210 + currentCH));
            args.push_back(fmt::arg("ch_4", 270 + currentCH));
            args.push_back(fmt::arg("sd_1", startingDist - 120));
            args.push_back(fmt::arg("sd_2", startingDist - 90));
            args.push_back(fmt::arg("sd_3", startingDist - 60));
            args.push_back(fmt::arg("sd_4", startingDist - 30));
            args.push_back(fmt::arg("sd_5", startingDist));

            std::string startingConnectors = fmt::vformat(levelStartingBase2, args);
            level += startingConnectors;
        }
    } else {
        if ((!overrideBankS["start"].standard.active || overrideBankS["start"].standard.keep) &&
                (!overrideBankS["slope"].standard.active || overrideBankS["slope"].standard.keep)) {
            fmt::dynamic_format_arg_store<fmt::format_context> args;
            args.push_back(fmt::arg("ch_1", 225 + currentCH));
            args.push_back(fmt::arg("ch_2", 165 + currentCH));
            args.push_back(fmt::arg("ch_3", 195 + currentCH));
            args.push_back(fmt::arg("ch_4", 255 + currentCH));
            args.push_back(fmt::arg("ch_5", 285 + currentCH));
            args.push_back(fmt::arg("sd_1", startingDist - 120));
            args.push_back(fmt::arg("sd_2", startingDist - 90));
            args.push_back(fmt::arg("sd_3", startingDist - 60));
            args.push_back(fmt::arg("sd_4", startingDist - 30));
            args.push_back(fmt::arg("sd_5", startingDist));

            std::string startingConnectors = fmt::vformat(levelStartingBase, args);
            level += startingConnectors;
        }
    }

    for (int64_t i = 0; i < biome.segments.size(); i++) {
        const auto& seg = biome.segments[i];
        x = seg.coords.first;
        y = seg.coords.second;
        y_swing = seg.y_swing;
        bool mini = seg.options.mini;

        if (mini) currentCH = corridorHeightM;
        else currentCH = corridorHeight;

        // Slopes
        std::string mFSlope = fmt::format("1,1339,2,{x},3,{y},6,90,5,{flip},57,911,64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("y", y - 15),
            fmt::arg("flip", y_swing > 0 ? 1 : 0));

        std::string mCSlope = fmt::format("1,1339,2,{x},3,{yC},6,-90,5,{flip},57,911,64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("yC", y - 15 + corridorHeightM),
            fmt::arg("flip", y_swing > 0 ? 1 : 0));

        std::string bFSlope = fmt::format("1,1338,2,{x},3,{y},6,{rot},57,911,64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("y", y),
            fmt::arg("rot", y_swing > 0 ? 0 : 90));

        std::string bCSlope = fmt::format("1,1338,2,{x},3,{yC},6,{rot},57,911,64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("yC", y + corridorHeight),
            fmt::arg("rot", y_swing > 0 ? 180 : 270));

        bool fMini = false, cMini = false;
        if (biome.options.typeA && i < biome.segments.size() - 1 &&
                mini != biome.segments[i + 1].options.mini) {
            if ((mini && y_swing == 1) || (!mini && y_swing == -1)) {
                fMini = true;
            } else if ((mini && y_swing == -1) || (!mini && y_swing == 1)) {
                cMini = true;
            }
        } else if (mini) {
            fMini = true;
            cMini = true;
        }

        bool slopeMiniOR = overrideBankS["slope"].mini.active && !overrideBankS["slope"].mini.keep;
        bool slopeBigOR = overrideBankS["slope"].standard.active && !overrideBankS["slope"].standard.keep;
        if (fMini) {
            level += !slopeMiniOR ? mFSlope : "";
        } else {
            level += !slopeBigOR ? bFSlope : "";
        }
        if (cMini) {
            level += !slopeMiniOR ? mCSlope : "";
        } else {
            level += !slopeBigOR ? bCSlope : "";
        }

        // Corner-Pieces
        std::string cornerBuild = "";
        bool cornerOR = (overrideBankS["slope"].mini.active && !overrideBankS["slope"].mini.keep && mini) ||
            (overrideBankS["slope"].standard.active && !overrideBankS["slope"].standard.keep && !mini);
        if (i > 1 && cornerPieces && !cornerOR) {
            int yOffset = mini ? 30 : 0;
            int cnry = 0;
            std::string flip = "";

            if (biome.segments[i - 1].y_swing == 1 && y_swing == -1) {
                cnry = y + currentCH + 30;
                flip += "4,1,";
            } else if (biome.segments[i - 1].y_swing == -1 && y_swing == 1) {
                cnry = y - 30 - yOffset;
            }
            if (cnry) {
                cornerBuild = fmt::format("1,{cnrID},2,{cnr1x},3,{cnry},{flip}6,90,57,905,64,1,67,1;1,{cnrID},2,{cnr2x},3,{cnry},6,90,5,1,{flip}57,905,64,1,67,1;",
                    fmt::arg("cnrID", mini ? 474 : 473),
                    fmt::arg("cnr1x", x - 30),
                    fmt::arg("cnr2x", x),
                    fmt::arg("cnry", cnry),
                    fmt::arg("flip", flip));
                level += cornerBuild;
            }
        }

        // Gravity-Portals
        int xP, yP;
        float rPdeg, scaleP;
        if (seg.options.isPortal == Portals::Gravity ||
                seg.options.isPortal == Portals::Fake) {
            double portalFactor = (static_cast<double>(currentCH) / 60.0) * 1.414;
            int portalNormal = currentCH / 10;
            int portalPos = currentCH / 4;
            int portalID;
            if (seg.options.isPortal == Portals::Fake)
                portalID = seg.options.gravity ? 10 : 11;
            else portalID = seg.options.gravity ? 11 : 10;

            xP = x - 15 - portalNormal + portalPos;
            yP = y + (biome.segments[i - 1].y_swing == -1
                ? portalNormal + portalPos - 15
                : currentCH + 15 - portalNormal - portalPos);
            if (mini && biome.segments[i - 1].y_swing == -1) yP -= 30;
            rPdeg = (mini ? 26.565 : 45) * -biome.segments[i - 1].y_swing;
            scaleP = portalFactor / 2.5;
            if (currentCH > 60) scaleP *= 0.85;

            if (biome.segments[i - 1].y_swing == y_swing) {
                yP += y_swing * -30;
            }// } else {
            //     if (biome.segments[i - 1].y_swing == -1) yP += 1;
            //     else {yP -= 1;}
            // }

            std::string portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rPdeg},32,{scale},57,908,64,1,67,1;",
                fmt::arg("portalID", portalID),
                fmt::arg("xP", xP),
                fmt::arg("yP", yP),
                fmt::arg("rPdeg", rPdeg),
                fmt::arg("scale", scaleP));
            level += portalBuild;
        }

        // Size-Portals
        if (i > 0 && mini != biome.segments[i - 1].options.mini) {
            if (biome.options.typeA) {
                float ySP, xSP, rSP, scaleSP;
                if (seg.options.isPortal == Portals::None) {
                    xSP = x - 26;
                    ySP = biome.segments[i - 1].coords.second + 15 +
                        (corridorHeight - 30) / 2;
                    ySP += biome.segments[i - 1].y_swing * 4;
                    rSP = (std::atan((corridorHeight - 30.f) / 30.f) * 180/M_PI - 90) *
                        biome.segments[i - 1].y_swing;
                    scaleSP = std::sqrt(std::pow((corridorHeight - 30), 2) + 900) / 110.f;
                } else {
                    xSP = xP;
                    ySP = yP;
                    rSP = rPdeg;
                    scaleSP = scaleP;
                }

                int portalID = mini ? 101 : 99;
                std::string portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rP},32,{scale},57,910,64,1,67,1;",
                fmt::arg("portalID", portalID),
                fmt::arg("xP", xSP),
                fmt::arg("yP", ySP),
                fmt::arg("rP", rSP),
                fmt::arg("scale", scaleSP));
                level += portalBuild;
            } else {
                float ySP, xSP, rSP = 0, scaleSP;
                if (seg.options.isPortal == Portals::None) {
                    ySP = y + currentCH / 2.0f;
                    if (seg.y_swing == 1) {
                        if (mini) {
                            ySP -= 45;
                        } else {
                            ySP -= 15;
                        }
                    } else {
                        ySP += 15;
                    }
                    scaleSP = (currentCH / 90.f) * 0.8;
                    xSP = x - 14 + 13 * scaleSP + (currentCH / 30.f);
                } else {
                    xSP = xP;
                    ySP = yP;
                    rSP = rPdeg;
                    scaleSP = scaleP;
                }

                int portalID = mini ? 101 : 99;

                level += fmt::format("1,{portalID},2,{xP},3,{yP},6,{rP},32,{scale},57,910,64,1,67,1;",
                fmt::arg("portalID", portalID),
                fmt::arg("xP", xSP),
                fmt::arg("yP", ySP),
                fmt::arg("rP", rSP),
                fmt::arg("scale", scaleSP));
            }
        }

        // Speed-Changes
        if (seg.options.speedChange != SpeedChange::None) {
            int speedID = convertSpeed(seg.options.speedChange);

            int spY = y + currentCH/2 + 14 * (seg.y_swing == 1 ? -1 : 1);
            if (mini && seg.y_swing == 1) spY -= 30;
            int spR = (mini ? 63.435 : 45) * -seg.y_swing;
            float speedFactor = (mini ? 0.32 : 0.5) * (currentCH / 60.0) * (speedID == 200 ? 1.27 : 1.0);
            level += fmt::format("1,{speedID},2,{x},3,{y},6,{r},32,{factor},57,909,64,1,67,1;",
                    fmt::arg("speedID", speedID),
                    fmt::arg("x", x - 14),
                    fmt::arg("y", spY),
                    fmt::arg("r", spR),
                    fmt::arg("factor", speedFactor));
        }

        // Corridor-Spikes
        bool spikeOR = (overrideBankS["spike"].mini.active && !overrideBankS["spike"].mini.keep && mini) ||
            (overrideBankS["spike"].standard.active && !overrideBankS["spike"].standard.keep && !mini);
        if (seg.options.isSpikeM && !spikeOR) {
            spikeSide = seg.options.spikeSide;
            int xS = x - 6;
            if (!spikeSide) {
                xS = x + 6;
            }
            int yS = y + 6;
            if (mini) yS -= 4;
            if ((y_swing == 1 && !spikeSide) || (y_swing == -1 && spikeSide)) {
                yS = y + currentCH - 6;
                if (mini) yS += 4;
            }

            // this has to use rotations, spikes don't support being flipped
            float scaleS = 1.f;
            float rOffset = 45.f;
            uint8_t spikeID = 103;
            if (mini) {
                yS -= 15;
                rOffset = 63.435f;
                spikeID = 39;
                if (!spikeSide) xS -= 1;
                else xS += 1;
            }
            int rS = rOffset;
            if (y_swing == 1 && !spikeSide) {
                rS = 180 - rOffset;
            } else if (y_swing == 1 && spikeSide) {
                rS = -rOffset;
            } else if (y_swing == -1 && spikeSide) {
                rS = -180 + rOffset;
            }
            level += fmt::format("1,{spikeID},2,{xS},3,{yS},6,{rS},64,1,67,1,32,{scaleS},57,907;",
                fmt::arg("spikeID", spikeID),
                fmt::arg("xS", xS),
                fmt::arg("yS", yS),
                fmt::arg("rS", rS),
                fmt::arg("scaleS", scaleS));
        }

        // fuzzy spikes
        if (seg.options.isFuzzy) {
            std::string colorMod = (biome.options.colorMode == ColorMode::NightMode)
                ? "21,1004,41,1,43,0a1a0.60a0a0,"
                : "";
            std::string fFlip = "", cFlip = "";
            if (y_swing == -1 && !fMini) {
                fFlip = "4,1,";
            } else if (y_swing == 1 && fMini) {
                fFlip = "5,1,";
            }
            if (y_swing == -1 && !cMini) {
                cFlip = "4,1,";
            } else if (y_swing == 1 && cMini) {
                cFlip = "5,1,";
            }

            int yF = y - (fMini ? 15 : 0);
            int yC = y + currentCH - (cMini ? 15 : 0);
            if (biome.options.typeA && y_swing == 1) {
                if (!cMini && fMini) yC -= 30;
                if (!fMini && cMini) yC += 30;
            }

            level += fmt::format("1,{ffuzzID},2,{xF},3,{yF},{colorMod}{fFlip}{rF}57,906,64,1,67,1;1,{cfuzzID},2,{xF},3,{yC},{colorMod}{rC}{cFlip}57,906,64,1,67,1;",
                fmt::arg("ffuzzID", fMini ? 1718 : 1717),
                fmt::arg("cfuzzID", cMini ? 1718 : 1717),
                fmt::arg("xF", x),
                fmt::arg("yF", yF),
                fmt::arg("yC", yC),
                fmt::arg("rF", fMini ? "6,90," : ""),
                fmt::arg("rC", cMini ? "6,-90," : "6,180,"),
                fmt::arg("fFlip", fFlip),
                fmt::arg("cFlip", cFlip),
                fmt::arg("colorMod", colorMod));
        }
    }

    // Ending-Connectors
    auto lastSegment = biome.segments.back();
    auto& endORgroup = lastSegment.y_swing == 1 ? overrideBankS["endup"] : overrideBankS["enddown"];
    auto& endORSize = lastSegment.options.mini ? endORgroup.mini : endORgroup.standard;
    auto& endORSlopeSize = lastSegment.options.mini ? overrideBankS["slope"].mini : overrideBankS["slope"].standard;
    if ((!endORSize.active || endORSize.keep) && (!endORSlopeSize.active || endORSlopeSize.keep)) {
        int xB = lastSegment.coords.first, yB = lastSegment.coords.second;
        int xT = xB, yT = yB + currentCH;
        if (lastSegment.options.mini) {
            if (lastSegment.y_swing == 1) {
                yB += 45;
                yT -= 15;
            } else {
                yT -= 75;
                yB -= 15;
            }
            while (yT <= (opts.maxHeight + 30)) {
                xT += 30;
                yT += 60;
                level += fmt::format("1,1339,2,{x},3,{y},5,1,6,-90,64,1,67,1;", fmt::arg("x", xT), fmt::arg("y", yT));
            }
            while (yB >= (opts.minHeight)) {
                xB += 30;
                yB -= 60;
                level += fmt::format("1,1339,2,{x},3,{y},6,90,64,1,67,1;", fmt::arg("x", xB), fmt::arg("y", yB));
            }
        } else {
            if (lastSegment.y_swing == 1) {
                yB += 30;
            } else {
                yT -= 30;
            }
            while (yT <= (opts.maxHeight + 30)) {
                xT += 30;
                yT += 30;
                level += fmt::format("1,1338,2,{x},3,{y},6,180,64,1,67,1;",
                    fmt::arg("x", xT), fmt::arg("y", yT));
            }
            while (yB >= (opts.minHeight)) {
                xB += 30;
                yB -= 30;
                level += fmt::format("1,1338,2,{x},3,{y},6,90,64,1,67,1;",
                    fmt::arg("x", xB), fmt::arg("y", yB));
            }
        }
    }

    // Meter-Marks
    const uint16_t markInterval = Mod::get()->getSavedValue<uint16_t>("opt-0-mark-interval");
    std::string metermarksStr = "";
    std::string currentMark;
    if (Mod::get()->getSavedValue<bool>("opt-0-show-meter-marks") && markInterval > 0) {
        int meters = markInterval;
        double markHeight;
        uint32_t startingDist = Mod::get()->getSavedValue<uint32_t>("opt-0-starting-dist", 345);
        for (int j = 0; j < (biome.options.length / markInterval); j++) {
            currentMark = "";
            markHeight = 15.5;

            for (int i = 0; i < 10; i++) {
                currentMark += fmt::format("1,508,2,{dist},3,{markHeight},20,1,57,902,6,-90,64,1,67,1,21,1011,25,1,24,11;",
                    fmt::arg("dist", startingDist + meters * 30), fmt::arg("markHeight", markHeight));
                markHeight += 30.0;
            }

            std::string meterLabel = ZipUtils::base64URLEncode(
                fmt::format("{}m", meters));
            meterLabel.erase(
                std::find(meterLabel.begin(), meterLabel.end(), '\0'), meterLabel.end());
            currentMark += fmt::format("1,914,2,{dist},3,21,20,1,57,902,32,0.62,21,1011,31,{meterLabel},25,1,64,1,67,1,24,11;",
                fmt::arg("dist", startingDist + (meters + 1) * 30), fmt::arg("meterLabel", meterLabel));
            meters += markInterval;
            metermarksStr += currentMark;
        }
    }
    level += metermarksStr;

    // Finish Line
    if (Mod::get()->getSavedValue<bool>("opt-0-show-finish-line")) {
        fmt::dynamic_format_arg_store<fmt::format_context> args;
        args.push_back(fmt::arg("pos1", x+7.5));
        args.push_back(fmt::arg("pos2", x+22.5));
        std::string finishLineBuild = fmt::vformat(
            finishLine,
            args);
        level += finishLineBuild;
    }

    // Visibility
    if (biome.options.visibility == Visibility::Low) {
        level += lowVis;
    }

    // Upside-Start
    if (biome.options.startingGravity) {
        double portalFactor = (static_cast<double>(currentCH) / 60.0) * 1.414;
        int portalNormal = currentCH / 10;
        int portalPos = currentCH / 4;
        int xP, yP;
        float rPdeg, scaleP;
        bool startingMini = biome.options.startingMini;

        xP = 285 - portalNormal + portalPos;
        yP = 75 + portalNormal + portalPos;
        if (startingMini) {
            yP -= 60;
            xP += 15;
        }
        rPdeg = startingMini ? 26.565 : 45;
        scaleP = portalFactor / 2.5;
        if (currentCH > 60) scaleP *= 0.85;

        std::string portalBuild = fmt::format("1,11,2,{xP},3,{yP},6,{rPdeg},32,{scale},57,913,64,1,67,1;",
            fmt::arg("xP", xP),
            fmt::arg("yP", yP),
            fmt::arg("rPdeg", rPdeg),
            fmt::arg("scale", scaleP));
        level += portalBuild;
    }
    // Mini start
    if (biome.options.startingMini) level += "1,101,2,255,3,163,6,17,13,0,57,912,64,1,67,1;";
    // hide icon
    if (hideIcon) level += fmt::format("1,1612,2,{startingdist},3,285,36,1;",
        fmt::arg("startingdist", Mod::get()->getSavedValue<uint32_t>("opt-0-starting-dist", 345))
    );

    // Song options
    level += fmt::format("1,1934,2,-15,3,255,13,1,36,1,392,{songID},406,1,{loop}408,{offset},421,1,422,0.5,10,0.5;",
        fmt::arg("songID", biome.song.id),
        fmt::arg("offset", biome.song.offset),
        fmt::arg("loop", biome.song.loop ? "413,1," : "")
    );

    // log::info("LevelData: name={}", ldata.name);
    // log::info("Biomes: {}", ldata.biomes.size());
    // for (size_t i = 0; i < ldata.biomes.size(); ++i) {
    //     const auto& biome = ldata.biomes[i];
    //     log::info("  Biome {}: x_initial={}, type={}, theme={}, segments={}",
    // i, biome.x_initial, biome.type, biome.theme, biome.segments.size());
    //     log::info("    Options: length={}, corridorHeight={}, maxHeight={},
    // visibility={}, startingSpeed={}, colorMode={}",
    //         biome.options.length,
    //         biome.options.corridorHeight,
    //         biome.options.maxHeight,
    //         static_cast<int>(biome.options.visibility),
    //         static_cast<int>(biome.options.startingSpeed),
    //         static_cast<int>(biome.options.colorMode)
    //     );
    //     for (size_t j = 0; j < biome.segments.size(); ++j) {
    //         const auto& seg = biome.segments[j];
    //         // log::info("      Segment {}: coords=({}, {}), y_swing={}", j,
    // seg.coords.first, seg.coords.second, seg.y_swing);
    //     }
    // }

    return level;
}

}  // namespace JFPGen
