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
#include <fmt/args.h>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include "constants.hpp"
#include "StringGen.hpp"
#include "Theming.hpp"
#include "Ninja.hpp"

using namespace geode::prelude;

bool jfpActive = false; // used in GameManager.cpp to check if JFP is active

namespace JFPGen {

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

static int convertSpeed(SpeedChange speed) {
    switch (speed) {
        case SpeedChange::Speed05x: return 200;
        case SpeedChange::Speed1x:  return 201;
        case SpeedChange::Speed2x:  return 202;
        case SpeedChange::Speed4x:  return 1334;
        default: return 203;
    }
}

std::string jfpPackString(const std::string& level, unsigned int seed, int song, bool compress) {

    std::string b64;
    if (compress) b64 = ZipUtils::compressString(level, true, 0);
    else b64 = level;
    std::string desc = fmt::format("ninja v1");
    desc = ZipUtils::base64URLEncode(ZipUtils::base64URLEncode(desc)); // double encoding might be unnecessary according to gmd-api source?
    b64.erase(std::find(b64.begin(), b64.end(), '\0'), b64.end());
    desc.erase(std::find(desc.begin(), desc.end(), '\0'), desc.end());

    std::string levelString = fmt::format("<k>kCEK</k><i>4</i><k>k2</k><s>JFP {title}</s><k>k3</k><s>{desc}</s><k>k4</k><s>{b64}</s><k>k45</k><i>{song}</i><k>k13</k><t/><k>k21</k><i>2</i><k>k50</k><i>35</i>",
    fmt::arg("desc", desc),
    fmt::arg("b64", b64),
    fmt::arg("title", std::to_string(seed)),
    fmt::arg("song", song));

    return levelString;
}

std::string jfpStringGen(bool compress) {
    LevelData ldata = generateJFPLevel();
    if (ldata.biomes.empty()) return "";

    std::string themeString = ThemeGen::parseTheme(Mod::get()->getSettingValue<std::string>("active-theme"), ldata);
    std::string levelString = jfpNewStringGen(ldata);;
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
            fmt::arg("slot", color.slot)
        );
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

    std::string levelBuildSeg2 = fmt::format(
        "1,{speedID},2,255,3,165,13,1,64,1,67,1;",
        fmt::arg("speedID", convertSpeed(ldata.biomes[0].options.startingSpeed))
    );
    const bool cornerPieces = Mod::get()->getSettingValue<bool>("corners");
    const bool hideIcon = Mod::get()->getSettingValue<bool>("hide-icon");
    
    std::string level;
    if(!overrideBank["override-base"]) level += levelBaseSeg;
    level += levelBuildSeg2;

    const auto& biome = ldata.biomes[0];
    const auto& opts = biome.options;
    int x = biome.x_initial;
    int y = biome.y_initial;
    int y_swing = 0;
    const int corridorHeight = opts.corridorHeight;
    const int corridorHeightM = biome.options.sizeTransitionTypeA ? corridorHeight + 30 : corridorHeight;
    bool spikeSide = false;

    int currentCH = biome.options.startingMini ? corridorHeightM : corridorHeight;

    if(!overrideBank["override-base"]) {
        fmt::dynamic_format_arg_store<fmt::format_context> args;
        if (opts.startingMini) {
            args.push_back(fmt::arg("ch_1", 90 + currentCH));
            args.push_back(fmt::arg("ch_2", 150 + currentCH));
            args.push_back(fmt::arg("ch_3", 210 + currentCH));
        } else {
            args.push_back(fmt::arg("ch_1", 225 + currentCH));
            args.push_back(fmt::arg("ch_2", 165 + currentCH));
            args.push_back(fmt::arg("ch_3", 195 + currentCH));
            args.push_back(fmt::arg("ch_4", 255 + currentCH));
            args.push_back(fmt::arg("ch_5", 285 + currentCH));
        }
        std::string startingConnectors = fmt::vformat(
            opts.startingMini ? levelStartingBase2 : levelStartingBase,
            args
        );
        level += startingConnectors;
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
        std::string mFSlope = fmt::format("1,1339,2,{x},3,{y},6,90,5,{flip},64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("y", y - 15),
            fmt::arg("flip", y_swing > 0 ? 1 : 0)
        );
        std::string mCSlope = fmt::format("1,1339,2,{x},3,{yC},6,-90,5,{flip},64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("yC", y - 15 + corridorHeightM),
            fmt::arg("flip", y_swing > 0 ? 1 : 0)
        );
        std::string bFSlope = fmt::format("1,1338,2,{x},3,{y},6,{rot},64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("y", y),
            fmt::arg("rot", y_swing > 0 ? 0 : 90)
        );
        std::string bCSlope = fmt::format("1,1338,2,{x},3,{yC},6,{rot},64,1,67,1;",
            fmt::arg("x", x),
            fmt::arg("yC", y + corridorHeight),
            fmt::arg("rot", y_swing > 0 ? 180 : 270)
        );

        if (i < biome.segments.size() - 1 && biome.options.sizeTransitionTypeA && mini != biome.segments[i + 1].options.mini) {
            if ((mini && y_swing == 1) || (!mini && y_swing == -1)) {
                level += mFSlope;
                level += bCSlope;
            } else if ((mini && y_swing == -1) || (!mini && y_swing == 1)) {
                level += bFSlope;
                level += mCSlope;
            }
        } else {
            if (mini) {
                level += mFSlope;
                level += mCSlope;
            } else {
                level += bFSlope;
                level += bCSlope;
            }
        }

        // if (mini) {
        //     level += fmt::format("1,1339,2,{x},3,{y},6,90,5,{flip},64,1,67,1;",
        //         fmt::arg("x", x),
        //         fmt::arg("y", y - 15),
        //         fmt::arg("flip", y_swing > 0 ? 1 : 0)
        //     );
        // } else if (mini) {
        //     level += fmt::format("1,1339,2,{x},3,{yC},6,-90,5,{flip},64,1,67,1;",
        //         fmt::arg("x", x),
        //         fmt::arg("yC", y - 15 + corridorHeightM),
        //         fmt::arg("flip", y_swing > 0 ? 1 : 0)
        //     );
        // } else if () {
        //     level += fmt::format("1,1338,2,{x},3,{y},6,{rot},64,1,67,1;",
        //         fmt::arg("x", x),
        //         fmt::arg("y", y),
        //         fmt::arg("rot", y_swing > 0 ? 0 : 90)
        //     );
            
        //     level += fmt::format("1,1338,2,{x},3,{yC},6,{rot},64,1,67,1;",
        //         fmt::arg("x", x),
        //         fmt::arg("yC", y + corridorHeight),
        //         fmt::arg("rot", y_swing > 0 ? 180 : 270)
        //     );
        // }

        // Corner-Pieces
        std::string cornerBuild = "";
        if (i > 1 && cornerPieces) {
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
                cornerBuild = fmt::format("1,{cnrID},2,{cnr1x},3,{cnry},{flip}6,90;1,{cnrID},2,{cnr2x},3,{cnry},6,90,5,1,{flip}64,1,67,1;",
                    fmt::arg("cnrID", mini ? 474 : 473),
                    fmt::arg("cnr1x", x - 30),
                    fmt::arg("cnr2x", x),
                    fmt::arg("cnry", cnry),
                    fmt::arg("flip", flip)
                );
                level += cornerBuild;
            }
        }

        // Gravity-Portals
        int xP, yP;
        float rPdeg, scaleP;
        if (seg.options.isPortal == Portals::Gravity || seg.options.isPortal == Portals::Fake) {
            double portalFactor = ((double)currentCH / 60.0) * 1.414;
            int portalNormal = currentCH / 10;
            int portalPos = currentCH / 4;
            int portalID;
            if (seg.options.isPortal == Portals::Fake) portalID = seg.options.gravity ? 10 : 11;
            else portalID = seg.options.gravity ? 11 : 10;

            xP = x - 15 - portalNormal + portalPos;
            yP = y + (y_swing == 1 ? portalNormal + portalPos - 15 : currentCH + 15 - portalNormal - portalPos);
            if (mini && y_swing == 1) yP -= 30;
            rPdeg = (mini ? 26.565 : 45) * y_swing;
            scaleP = portalFactor / 2.5;
            if (currentCH > 60) scaleP *= 0.85;

            std::string portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rPdeg},32,{scale},64,1,67,1;",
            fmt::arg("portalID", portalID),
            fmt::arg("xP", x-15-portalNormal+portalPos),
            fmt::arg("yP", yP),
            fmt::arg("rPdeg", rPdeg),
            fmt::arg("scale", scaleP));
            level += portalBuild;
        }

        // Size-Portals
        if (i > 0 && mini != biome.segments[i - 1].options.mini) {
            if (biome.options.sizeTransitionTypeA) {
                float ySP, xSP, rSP, scaleSP;
                if (seg.options.isPortal == Portals::None) {
                    xSP = x - 26;
                    ySP = biome.segments[i - 1].coords.second + 15 + (corridorHeight - 30) / 2;
                    rSP = (std::atan((corridorHeight - 30.f) / 30.f) * 180/3.1415 - 90) * biome.segments[i - 1].y_swing;
                    scaleSP = std::sqrt(std::pow((corridorHeight - 30), 2) + 900) / 110.f;
                } else {
                    xSP = xP;
                    ySP = yP;
                    rSP = rPdeg;
                    scaleSP = scaleP;
                }
                
                int portalID = mini ? 101 : 99;
                
                std::string portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rP},32,{scale},64,1,67,1;",
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
                    xSP = x - 15 + 13 * scaleSP;
                } else {
                    xSP = xP;
                    ySP = yP;
                    rSP = rPdeg;
                    scaleSP = scaleP;
                }
                
                int portalID = mini ? 101 : 99;
                
                std::string portalBuild = fmt::format("1,{portalID},2,{xP},3,{yP},6,{rP},32,{scale},64,1,67,1;",
                fmt::arg("portalID", portalID),
                fmt::arg("xP", xSP),
                fmt::arg("yP", ySP),
                fmt::arg("rP", rSP),
                fmt::arg("scale", scaleSP));
                level += portalBuild;
            }
        }

        // Speed-Changes
        if (seg.options.speedChange != SpeedChange::None) {
            int speedID = convertSpeed(seg.options.speedChange);

            int spY = y + currentCH/2 + 15 * (seg.y_swing == 1 ? -1 : 1);
            int spR = seg.y_swing == 1 ? -45 : 45;
            double speedFactor = 0.5 * (currentCH / 60.0);
            level += fmt::format("1,{speedID},2,{x},3,{y},6,{r},32,{factor},64,1,67,1;",
                    fmt::arg("speedID", speedID),
                    fmt::arg("x", x - 15),
                    fmt::arg("y", spY),
                    fmt::arg("r", spR),
                    fmt::arg("factor", speedFactor)
            );
        }

        // Corridor-Spikes
        if (seg.options.isSpikeM) {
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
            if (mini) {
                yS -= 15;
                scaleS = 0.7f;
                rOffset = 63.435f;
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
            std::string spikeBuild = fmt::format("1,103,2,{xS},3,{yS},6,{rS},64,1,67,1,32,{scaleS};",
                fmt::arg("xS", xS),
                fmt::arg("yS", yS),
                fmt::arg("rS", rS),
                fmt::arg("scaleS", scaleS)
            );
            level += spikeBuild;
        }

        if (seg.options.isFuzzy) {
            std::string colorMod = (biome.options.colorMode == ColorMode::NightMode) ? "21,1004,41,1,43,0a1a0.60a0a0," : "";
            std::string flip = "";
            if (y_swing == -1 && !mini) {
                flip = "4,1,";
            } else if (y_swing == 1 && mini) {
                flip = "5,1,";
            }

            level += fmt::format("1,{fuzzID},2,{xF},3,{yF},{colorMod}{flip}{rF}64,1,67,1;1,{fuzzID},2,{xF},3,{yC},{colorMod}{rF2}{flip}64,1,67,1;",
                fmt::arg("fuzzID", mini ? 1718 : 1717),
                fmt::arg("xF", x),
                fmt::arg("yF", y - (mini ? 15 : 0)),
                fmt::arg("yC", y + currentCH - (mini ? 15 : 0)),
                fmt::arg("rF", mini ? "6,90," : ""),
                fmt::arg("rF2", mini ? "6,-90," : "6,180,"),
                fmt::arg("flip", flip),
                fmt::arg("colorMod", colorMod)
            );
        }
    }
    
    // Ending-Connectors
    bool endingMini = biome.segments.back().options.mini;
    if (
        biome.segments.back().y_swing == 1 && !overrideBank["override-endup"] ||
        biome.segments.back().y_swing == -1 && !overrideBank["override-enddown"]
    ) {
        auto lastSegment = biome.segments[biome.segments.size() - 1];
        int xB = lastSegment.coords.first, yB = lastSegment.coords.second, xT = xB, yT = yB + currentCH;
        if (endingMini) {
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
                level += fmt::format("1,1338,2,{x},3,{y},6,180,64,1,67,1;", fmt::arg("x", xT), fmt::arg("y", yT));
            }
            while (yB >= (opts.minHeight)) {
                xB += 30;
                yB -= 30;
                level += fmt::format("1,1338,2,{x},3,{y},6,90,64,1,67,1;", fmt::arg("x", xB), fmt::arg("y", yB));
            }
        }
    }

    // Meter-Marks
    const bool marks = Mod::get()->getSettingValue<bool>("marks");
    const int64_t markInterval = Mod::get()->getSettingValue<int64_t>("marker-interval");
    std::string metermarksStr = "";
    std::string currentMark;
    if (marks && markInterval > 0) {
        int meters = markInterval;
        double markHeight;
        for (int j = 0; j < (biome.options.length / markInterval); j++) {
            currentMark = "";
            markHeight = 15.5;

            for (int i = 0; i < 10; i++) {
                currentMark += fmt::format("1,508,2,{dist},3,{markHeight},20,1,57,2,6,-90,64,1,67,1,21,1011,25,1,24,11;", fmt::arg("dist", 345+meters*30), fmt::arg("markHeight", markHeight));
                markHeight += 30.0;
            }

            std::string meterLabel = ZipUtils::base64URLEncode(fmt::format("{}m", meters));
            meterLabel.erase(std::find(meterLabel.begin(), meterLabel.end(), '\0'), meterLabel.end());
            currentMark += fmt::format("1,914,2,{dist},3,21,20,1,57,2,32,0.62,21,1011,31,{meterLabel},25,1,64,1,67,1,24,11;", fmt::arg("dist", 375+meters*30), fmt::arg("meterLabel", meterLabel));
            meters += markInterval;
            metermarksStr += currentMark;
        }
    }
    level += metermarksStr;

    // Visibility
    if (biome.options.visibility == Visibility::Low) {
        level += lowVis;
    }

    // Upside-Start
    if (biome.options.startingGravity) level += "1,11,2,299,3,99,6,45,32,0.57;";
    // Mini start
    if (biome.options.startingMini) level += "1,101,2,255,3,163,6,17,13,0,64,1,67,1;";
    // hide icon
    if (hideIcon) level += "1,1612,2,375,3,285,36,1;";
    
    // log::info("LevelData: name={}", ldata.name);
    // log::info("Biomes: {}", ldata.biomes.size());
    // for (size_t i = 0; i < ldata.biomes.size(); ++i) {
    //     const auto& biome = ldata.biomes[i];
    //     log::info("  Biome {}: x_initial={}, type={}, theme={}, segments={}", i, biome.x_initial, biome.type, biome.theme, biome.segments.size());
    //     log::info("    Options: length={}, corridorHeight={}, maxHeight={}, visibility={}, startingSpeed={}, colorMode={}",
    //         biome.options.length,
    //         biome.options.corridorHeight,
    //         biome.options.maxHeight,
    //         static_cast<int>(biome.options.visibility),
    //         static_cast<int>(biome.options.startingSpeed),
    //         static_cast<int>(biome.options.colorMode)
    //     );
    //     for (size_t j = 0; j < biome.segments.size(); ++j) {
    //         const auto& seg = biome.segments[j];
    //         // log::info("      Segment {}: coords=({}, {}), y_swing={}", j, seg.coords.first, seg.coords.second, seg.y_swing);
    //     }
    // }

    return level;
}

}