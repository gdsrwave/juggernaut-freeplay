// Copyright 2025 GDSRWave
#include "Theming.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <Geode/Geode.hpp>
#include "./Ninja.hpp"
#include "./shared.hpp"

using namespace geode::prelude;

// the theme parsing in JFP uses a fully sandboxed script and
// evaluates a few simple, controlled arithmetic operations
// per each added block. there is no arbitrary code being installed
// or run by this mod; JFPT language is the closest you'll find. -M
namespace ThemeGen {

std::array<int, 3> hexToColor(const std::string& hex) {
    std::array<int, 3> color = {255, 255, 255};
    if (hex.size() == 7 && hex[0] == '#') {
        color[0] = geode::utils::numFromString<int>(hex.substr(1, 2), 16).unwrapOr(0);
        color[1] = geode::utils::numFromString<int>(hex.substr(3, 2), 16).unwrapOr(0);
        color[2] = geode::utils::numFromString<int>(hex.substr(5, 2), 16).unwrapOr(0);
    }
    return color;
}

// orientationMatch, but modified to account for miniwave and transitions
// this function has been heavily souped up to account for extra connector segments, will be cleaned up sometime soon -M
bool strictOM(const std::vector<JFPGen::Segment>& segments, int idx, int offset,
        const std::vector<int>& pattern, OMType omType, bool typeA) {
    bool extraSegments = false;
    int idxOff = idx + offset + 1;
    int pSize = static_cast<int>(pattern.size());
    int sSize = static_cast<int>(segments.size());
    if (idxOff < pSize) {
         auto startORSize = segments[0].options.mini ? overrideBankS["start"].mini : overrideBankS["start"].standard;
         if (!startORSize.active) {
            extraSegments = true;
            //log::info("------");
            //log::info("matchin {} {}", startORSize.active, startORSize.blocks.size());
         }
    } else if (idxOff > sSize) {
        OverrideStatic endORSize;
        if (segments.back().y_swing == 1) {
            endORSize = segments.back().options.mini ? overrideBankS["endup"].mini : overrideBankS["endup"].standard;
        } else {
            endORSize = segments.back().options.mini ? overrideBankS["enddown"].mini : overrideBankS["enddown"].standard;
        }
        if (!endORSize.active) extraSegments = true;
    }
    
    if (extraSegments && omType != OMType::Corridor) {
        if (idxOff <= -10 || idxOff < pSize - 10 || idxOff > sSize + 10 || idxOff > sSize + pSize + 10) {
            return false;
        }
    } else {
        if (idxOff <= 0 || idxOff < pSize || idxOff > sSize || idxOff > sSize + pSize) {
            return false;
        }
    }
    int y_swing = 0;
    bool mini = false;
    bool isTransition = false;
    for (int i = 0; i < pattern.size(); i++) {
        int j = idxOff - static_cast<int>(pattern.size()) + i;
        // if (idx == segments.size() - 1) {
        //     log::info("{} {} {} {} {} {}", idx, offset, j, j > static_cast<int>(segments.size())-1, extraSegments, omType != OMType::Corridor);
        // }
        if (extraSegments && omType != OMType::Corridor && j < 0) {
            if (omType == OMType::Floor) {
                y_swing = 1;
            } else {
                y_swing = -1;
            }
            mini = segments[0].options.mini;
            isTransition = false;
            
        } else if (extraSegments && omType != OMType::Corridor && j > static_cast<int>(segments.size())-1) {
            if (omType == OMType::Floor) {
                y_swing = -1;
            } else {
                y_swing = 1;
            }
            mini = segments.back().options.mini;
            isTransition = false;
        } else {
            auto seg = segments[j];
            y_swing = seg.y_swing;
            mini = seg.options.mini;
            isTransition = seg.options.isTransition;
        }
        //if (extraSegments) log::info("j: {}, yswing: {}", j, y_swing);

        if (typeA && isTransition) {
            if (omType == OMType::Floor &&
                    ((mini && y_swing == 1) || (!mini && y_swing == -1))) {
                y_swing *= 2;
            } else if (omType == OMType::Ceiling &&
                    ((mini && y_swing == -1) || (!mini && y_swing == 1))) {
                y_swing *= 2;
            } else if (omType == OMType::Corridor) {
                return false;
            }
        } else if (mini) {
            y_swing *= 2;
        }
        if (y_swing != pattern[i]) {
            return false;
        }
    }
    return true;
}

std::string handleColor(ColorAction* color) {
    auto rgbv = color->m_color;
    std::string res = "";

    res += fmt::format("Slot: {}\n", color->m_colorID);
    uint16_t copyID = color->m_copyID;
    if (copyID) {
        res += fmt::format("Color: Copy {}\n", color->m_copyID);
    } else {

        char rgbBuf[8];
        auto rgbv = color->m_fromColor;
        snprintf(rgbBuf, sizeof(rgbBuf), "#%02X%02X%02X", rgbv.r, rgbv.g, rgbv.b);
        res += fmt::format("Color: {}\n", std::string(rgbBuf));
    }

    if (color->m_blending) res += fmt::format("Blending: {}\n", color->m_blending);

    const float ep = 0.00005;
    if(std::abs(color->m_fromOpacity - 1) > ep) {
        res += fmt::format("Opacity: {}\n", color->m_fromOpacity);
    }

    auto hsv = color->m_copyHSV;
    bool usingSat = (std::abs(hsv.s - 1) > ep && !hsv.absoluteSaturation) || ((std::abs(hsv.s) > ep && hsv.absoluteSaturation));
    bool usingVal = (std::abs(hsv.v - 1) > ep && !hsv.absoluteBrightness) || ((std::abs(hsv.v) > ep && hsv.absoluteBrightness));
    if (std::abs(hsv.h) > ep || usingSat || usingVal) {
        std::string hsvString = fmt::format("{}a{}a{}a{}a{}",
            hsv.h, hsv.s, hsv.v, static_cast<int>(hsv.absoluteBrightness), static_cast<int>(hsv.absoluteSaturation));
        res += fmt::format("Special: 10_{}\n", hsvString);
    }

    return res;
}

std::string handleRawBlock(std::string addBlockLine, OMType omType, JFPTExtraTypes extra) {
    std::string res;
    size_t start = 0;

    if (!addBlockLine.empty() && addBlockLine.back() == ';') addBlockLine.pop_back();

    bool addedR = false;
    bool addedS = false;
    while (start < addBlockLine.size()) {
        size_t comma = addBlockLine.find(',', start);
        std::string k = addBlockLine.substr(start, comma - start);
        start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

        comma = addBlockLine.find(',', start);
        std::string v = addBlockLine.substr(start, comma - start);
        start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

        int key = 0;
        key = geode::utils::numFromString<int>(k).unwrapOr(-1);
        if (key == -1) {
            continue;
        }

        if (key == 6) {
            addedR = true;
        } else if (key == 32 || key == 129) {
            addedS = true;
        }

    }

    start = 0;
    while (start < addBlockLine.size()) {
        size_t comma = addBlockLine.find(',', start);
        std::string k = addBlockLine.substr(start, comma - start);
        start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

        comma = addBlockLine.find(',', start);
        std::string v = addBlockLine.substr(start, comma - start);
        start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

        int key = 0;
        key = geode::utils::numFromString<int>(k).unwrapOr(-1);
        if (key == -1) {
            continue;
        }

        // in 2.2, block/"GameObject"s' data will always have attributes 155
        // and 156 applied to them. these are automatically generated by GD
        // using a currently unknown technique, and when they are mismatched,
        // issues such as block flickering, incorrectly set colors, incorrectly
        // changing colors etc. will appear when a level is played. because of
        // this, JFP will strip these values. if you face issues with removing
        // these values, I unfortunately don't have a great answer; all I can
        // do is suggest you create theme segments in 2.1 instead. thanks for
        // understanding. -M
        if (key == 155 || key == 156) continue;

        if (key == 2) {
            float xv = geode::utils::numFromString<float>(v).unwrapOr(0.f);
            if (std::abs(xv - std::round(xv)) < 0.1f && extra == JFPTExtraTypes::None) {
                xv = std::round(xv);
                v = geode::utils::numToString(static_cast<int>(xv));
            }
            if (omType != OMType::None) v = "[X+" + v + "]";
        } else if (key == 3) {
            float yv = geode::utils::numFromString<float>(v).unwrapOr(0.f);
            // num = geode::utils::numFromString<float>(numStr).unwrapOr(0.f);
            if (std::abs(yv - std::round(yv)) < 0.1f && extra == JFPTExtraTypes::None) {
                yv = std::round(yv);
                v = geode::utils::numToString(static_cast<int>(yv));
            }
            if (omType == OMType::Ceiling) v = "[Y+C+" + v + "]";
            else if (omType != OMType::None && extra != JFPTExtraTypes::Pattern) v = "[Y+" + v + "]";
        } else if (key == 6 && extra == JFPTExtraTypes::Special) {
            v = "[R+" + v + "]";
        } else if ((key == 32 || key == 129) && extra == JFPTExtraTypes::Special) {
            v = "[S*" + v + "]";
        }
        if (!res.empty()) res += ",";
        res += k + "," + v;

        if (key == 1) {
            if (!addedR && extra == JFPTExtraTypes::Special) res += ",6,[R]";
            if (!addedS && extra == JFPTExtraTypes::Special) res += ",32,[S]";
        }
    }

    return res;
}

std::string parseAddBlock(std::string addBlockLine, float X, float Y,
        int maxHeight, int minHeight, int corridorHeight, bool isForPattern, float rotation, float scale) {
    // remove legacy prefix
    if (addBlockLine.empty()) return "";
    const std::string prefix = "Add Block";
    auto pos = addBlockLine.find(prefix);
    if (pos != std::string::npos) {
        addBlockLine = addBlockLine.substr(pos + prefix.length());
    }
    char validVars[] = {'X', 'Y', 'C', 'R', 'S'};

    addBlockLine.erase(0, addBlockLine.find_first_not_of(" \t\r\n"));
    addBlockLine.erase(addBlockLine.find_last_not_of(" \t\r\n") + 1);

    size_t repeatPos = addBlockLine.rfind(" repeat ");
    int repeatDist = 0;
    if (repeatPos != std::string::npos && repeatPos < addBlockLine.size()-8) {
        repeatDist = geode::utils::numFromString<int>(addBlockLine.substr(repeatPos + 8)).unwrapOr(0);
        addBlockLine = addBlockLine.substr(0, repeatPos);
    }

    // evaluate bracketed arithmetic
    const size_t repetitions = repeatDist != 0 ? 25 : 1;
    std::string res;
    for (size_t j = 0; j < repetitions; j++) {
        std::map<int, float> arithValues;
        size_t start = 0;
        while (start < addBlockLine.size()) {
            size_t comma = addBlockLine.find(',', start);
            std::string k = addBlockLine.substr(start, comma - start);
            start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

            comma = addBlockLine.find(',', start);
            std::string valuePair = addBlockLine.substr(start, comma - start);
            start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

            int key = 0;
            key = geode::utils::numFromString<int>(k).unwrapOr(-1);
            if (key == -1) {
                continue;
            }

            size_t i = 0;
            while (i < valuePair.size()) {
                if (valuePair[i] == '[') {
                    size_t end = valuePair.find(']', i);
                    if (end != std::string::npos) {
                        std::string expr = valuePair.substr(i + 1, end - i - 1);

                        expr.erase(remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
                        float value = 0.f;

                        // throw in xpos, ypos, corridorheight values
                        std::string parsedExpr;
                        bool preserveFloat = false;
                        char primary = 0;
                        for (size_t j = 0; j < expr.size(); ++j) {
                            if (expr[j] == 'X') {
                                parsedExpr += geode::utils::numToString(X);
                                if (primary == 0) primary = expr[j];
                            } else if (expr[j] == 'Y') {
                                parsedExpr += geode::utils::numToString(Y);
                                if (primary == 0) primary = expr[j];
                            } else if (expr[j] == 'C') {
                                parsedExpr += geode::utils::numToString(corridorHeight);
                                if (primary == 0) primary = expr[j];
                            } else if (expr[j] == 'R') {
                                parsedExpr += geode::utils::numToString(rotation);
                                if (primary == 0) primary = expr[j];
                            } else if (expr[j] == 'S') {
                                parsedExpr += geode::utils::numToString(scale);
                                if (primary == 0) primary = expr[j];
                            } else {
                                parsedExpr += expr[j];
                            }
                        }

                        // evaluate the parsedExpr as a left-to-right sum/diff/prod/quot/mod
                        float acc = 0.f;
                        char lastOp = 0;
                        size_t idx = 0;
                        while (idx < parsedExpr.size()) {
                            size_t nextOp = parsedExpr.find_first_of("+-*/%", idx);
                            std::string numStr = parsedExpr.substr(idx, nextOp - idx);
                            float num = geode::utils::numFromString<float>(numStr).unwrapOr(0.f);
                            if (lastOp == 0) {
                                acc = num;
                            } else if (lastOp == '+') {
                                acc += num;
                            } else if (lastOp == '-') {
                                acc -= num;
                            } else if (lastOp == '*') {
                                acc *= num;
                            } else if (lastOp == '/') {
                                acc /= num;
                            } else if (lastOp == '%') {
                                acc = std::fmod(acc, num);
                            }
                            if (nextOp == std::string::npos) break;
                            lastOp = parsedExpr[nextOp];
                            idx = nextOp + 1;
                        }
                        value = acc;
                        arithValues[key] = value;
                        
                        i = end + 1;
                    } else {
                        i++;
                    }
                } else {
                    i++;
                }
            }
        }

        if (!isForPattern && arithValues.contains(3) && (arithValues[3] > 375 || arithValues[3] < -15)) {
            continue;
        }
    
        // morph positions which are relative to scale and rotation
        if (scale < 0.9995 || scale > 1.0005) {
            float xdiff = arithValues[2] - X;
            float ydiff = arithValues[3] - Y;
            if (arithValues[2] != X) {
                arithValues[3] = X + (xdiff) * scale;
            } else if (arithValues[3] != Y) {
                arithValues[3] = Y + (ydiff) * scale;
            }
        }
        if (rotation > 0.0005 || rotation < -0.0005) {
            float xdiff = arithValues[2] - X;
            float ydiff = arithValues[3] - Y;
            if (arithValues[2] != X || arithValues[3] != Y) {
                float rotationRad = rotation * M_PI/180;
                arithValues[2] = (xdiff * std::cos(rotationRad) + ydiff * std::sin(rotationRad)) + X;
                arithValues[3] = (ydiff * std::cos(rotationRad) - xdiff * std::sin(rotationRad)) + Y;
                
            }
        }

        // replace bracketed values
        std::string localRes;
        start = 0;
        while (start < addBlockLine.size()) {
            size_t comma = addBlockLine.find(',', start);
            std::string k = addBlockLine.substr(start, comma - start);
            start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

            comma = addBlockLine.find(',', start);
            std::string valuePair = addBlockLine.substr(start, comma - start);
            start = (comma == std::string::npos) ? addBlockLine.size() : comma + 1;

            int key = 0;
            key = geode::utils::numFromString<int>(k).unwrapOr(-1);
            if (key == -1) {
                continue;
            }

            if (arithValues.contains(key)) {
                if (key == 2 || key == 3) {
                    valuePair = geode::utils::numToString<float>(arithValues[key]);
                } else {
                    valuePair = geode::utils::numToString<float>(arithValues[key]);
                }
            }

            if (!localRes.empty()) localRes += ",";
            localRes += k + "," + valuePair;
        }

        if (!localRes.empty() && localRes.back() != ';') {
            localRes += ';';
        }
        res += localRes;
        Y += repeatDist;
    }
    return res;
}

std::string parseTheme(const std::string& name, const JFPGen::LevelData& ldata) {
    bool inMetadata = false;
    bool inAddcolor = false;
    bool inKValues = false;
    bool inMatches = false;
    bool inPattern = false;
    bool inOverride = false;
    bool inOverrideMini = false;

    RepeatingPattern patternGen = RepeatingPattern();
    ThemeMatch cMP = ThemeMatch();
    ThemeMetadata metadata;
    std::vector<std::vector<ThemeMatch>> matchConditions;
    std::vector<ThemeMatch> cMPList;
    std::vector<RepeatingPattern> repeatingPatterns;
    std::string themeGen = "";
    std::vector<std::string> corridorBlocks;
    std::vector<std::string> corridorBlocksFill;
    std::string cmd;

    for (const auto& cmd : allowedDef) {
        overrideBankS[cmd] = OverrideGroups{
            .standard = {
                .active = false,
                .keep = false
            },
            .mini = {
                .active = false,
                .keep = false
            }
        };
    }

    JFPGen::Color sColor = JFPGen::Color();

    auto biome = ldata.biomes[0];

    std::string themeName = name;
    if (themeName.size() < 5 || themeName.substr(themeName.size() - 5) != ".jfpt") {
        themeName += ".jfpt";
    }

    std::string themeRaw = file::readString(Mod::get()->getSaveDir() / "themes" / themeName).unwrapOrDefault();
    std::istringstream iss(themeRaw);
    
    std::vector<std::string> lines;
    std::string line;
    uint32_t ctr = 0;
    while (std::getline(iss, line)) {
        line.erase(0, line.find_first_not_of("\t\r\n"));
        line.erase(line.find_last_not_of("\t\r\n") + 1);
        lines.push_back(line);
        ctr++;
        if (ctr > 20000) break; 
    }

    for (const auto& l : lines) {
        if (l.length() > 1000) {
            continue;
        }

        if (l.find("# define") == 0 && l.back() == '#') {
            bool keepOrig = false;
            std::string cmdLine = l.substr(9, l.size() - 11);
            cmdLine.erase(0, cmdLine.find_first_not_of(" \t\r\n"));
            cmdLine.erase(cmdLine.find_last_not_of(" \t\r\n") + 1);

            size_t keepPos = cmdLine.rfind(" keep");
            if (keepPos != std::string::npos && keepPos == cmdLine.size() - 5) {
                keepOrig = true;
                cmdLine = cmdLine.substr(0, keepPos);
                cmdLine.erase(cmdLine.find_last_not_of(" \t\r\n") + 1);
            }
            cmd = cmdLine;

            if (cmd.size() > 5 && cmd.substr(cmd.size() - 5) == "-mini") {
                cmd = cmd.substr(0, cmd.size() - 5);
                inOverrideMini = true;
            } else {
                inOverrideMini = false;
            }
            if (std::find(allowedDef.begin(), allowedDef.end(), cmd) != allowedDef.end()) {
                inOverride = true;
                auto& group = inOverrideMini
                    ? overrideBankS[cmd].mini : overrideBankS[cmd].standard;
                group.active = true;
                group.keep = keepOrig;
            }
            continue;
        } else if (l == "# end define #") {
            inOverride = false;
            continue;
        }

        if (inOverride) {
            if (inOverrideMini) {
                overrideBankS[cmd].mini.blocks.push_back(l);
            } else {
                overrideBankS[cmd].standard.blocks.push_back(l);
            }
        }

        if (l == "# k #") {
            inKValues = true;
            continue;
        } else if (l == "# end k #") {
            inKValues = false;
            continue;
        }
        if (inKValues) {
            auto pos = l.find(':');
            if (pos != std::string::npos) {
                std::string key = l.substr(0, pos);
                std::string value = l.substr(pos + 1);

                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                kBank[key] = value;
            }
            continue;
        }

        if (l == "# color #") {
            inAddcolor = true;
            continue;
        } else if (l == "# end color #") {
            inAddcolor = false;
            pushColor(sColor);
            sColor = JFPGen::Color();
            continue;
        }
        if (inAddcolor) {
            auto pos = l.find(':');
            if (pos != std::string::npos) {
                std::string key = l.substr(0, pos);
                std::string value = l.substr(pos + 1);

                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                if (key == "Color") {
                    if (value.size() > 4 && value.substr(0, 4) == "Copy") {
                        sColor.copyColor = geode::utils::numFromString<int>(value.substr(5)).unwrapOr(-1);
                    } else {
                        sColor.rgb = hexToColor(value);
                    }
                } else if (key == "Slot") {
                    sColor.slot = geode::utils::numFromString<int>(value).unwrapOr(13);
                } else if (key == "Blending") {
                    sColor.blending = (value == "true" || value == "1");
                } else if (key == "Opacity") {
                    sColor.opacity = geode::utils::numFromString<float>(value).unwrapOr(1.0f);
                } else if (key == "Special") {
                    sColor.special = value;
                }
            }
            continue;
        }
    }

    if (overrideBankS["base"].standard.active) {
        for (const auto& baseBlock : overrideBankS["base"].standard.blocks) {
            themeGen += parseAddBlock(
                baseBlock,
                0,
                0,
                biome.options.maxHeight,
                biome.options.minHeight,
                biome.options.corridorHeight);
        }
    }

    auto& startSize = biome.options.startingMini ? overrideBankS["start"].mini : overrideBankS["start"].standard;
    if (startSize.active) {
        for (const auto& startBlock : startSize.blocks) {
            int passedCH = biome.options.corridorHeight;
            if (biome.options.startingMini && biome.options.typeA) passedCH += 30;
            themeGen += parseAddBlock(
                startBlock,
                0,
                0,
                biome.options.maxHeight,
                biome.options.minHeight,
                passedCH);
        }
    }

    auto lastSegment = biome.segments.back();
    auto& endORgroup = lastSegment.y_swing == 1 ? overrideBankS["endup"] : overrideBankS["enddown"];
    auto& endORSize = lastSegment.options.mini ? endORgroup.mini : endORgroup.standard;
    if (endORSize.active) {
        for (const auto& endBlock : endORSize.blocks) {
            int passedCH = biome.options.corridorHeight;
            if (lastSegment.options.mini && biome.options.typeA) passedCH += 30;
            themeGen += parseAddBlock(
                endBlock,
                lastSegment.coords.first,
                lastSegment.coords.second,
                biome.options.maxHeight,
                biome.options.minHeight,
                passedCH);
        }
    }

    for (const auto& l : lines) {
        if (l.length() > 1000) {
            continue;
        }

        if (!l.empty() &&
                ((l.find("# if ") == 0 && l.back() == '#') ||
                (l.find("# else if ") == 0 && l.back() == '#'))) {
            inMatches = true;
            if (l.find("# if ") == 0) {
                cMP = ThemeMatch();
            } else if (l.find("# else if ") == 0) {
                if (!cMP.pattern.empty() || !cMP.notPatterns.empty()) {
                    cMPList.push_back(cMP);
                    cMP = ThemeMatch();
                }
            }

            std::string condition = l.substr(2, l.size() - 4);
            std::vector<std::string> tokens;
            size_t start = 0, end = 0;
            while ((end = condition.find(" and ", start)) != std::string::npos) {
                tokens.push_back(condition.substr(start, end - start));
                start = end + 5;
            }
            tokens.push_back(condition.substr(start));

            for (const auto& token : tokens) {
                std::string t = token;

                t.erase(0, t.find_first_not_of(" \t\r\n"));
                t.erase(t.find_last_not_of(" \t\r\n") + 1);

                size_t corridorPos = t.find("corridor");
                size_t floorPos = t.find("floor");
                size_t ceilingPos = t.find("ceiling");
                OMType omType;
                if (corridorPos != std::string::npos) omType = OMType::Corridor;
                else if (floorPos != std::string::npos) omType = OMType::Floor;
                else if (ceilingPos != std::string::npos) omType = OMType::Ceiling;
                else continue;

                bool isNot = false;
                size_t notPos = t.find("not matches");
                size_t matchesPos = t.find("matches");
                if (notPos != std::string::npos) {
                    isNot = true;
                    t = t.substr(notPos + 12);
                } else if (matchesPos != std::string::npos) {
                    t = t.substr(matchesPos + 7);
                } else {
                    continue;
                }

                t.erase(0, t.find_first_not_of(" \t\r\n"));
                t.erase(t.find_last_not_of(" \t\r\n") + 1);

                // parse y_swing from brackets
                std::vector<int> pattern;
                int offset = 0;
                size_t bracketStart = t.find('[');
                size_t bracketEnd = t.find(']');
                std::string symbols = t;
                if (bracketStart != std::string::npos &&
                        bracketEnd != std::string::npos && bracketEnd > bracketStart) {
                    symbols = t.substr(0, bracketStart);
                    std::string offsetStr = t.substr(
                        bracketStart + 1, bracketEnd - bracketStart - 1);

                    int displacement = 0;
                    for (size_t idx = bracketEnd + 1; idx < t.size(); ++idx) {
                        if (t[idx] == '+' || t[idx] == '-' ||
                                t[idx] == '/' || t[idx] == '\\') {
                            displacement++;
                        }
                    }
                    offset = displacement;
                }

                t.erase(remove_if(t.begin(), t.end(), ::isspace), t.end());
                for (char ch : t) {
                    if (ch == '+') pattern.push_back(1);
                    else if (ch == '-') pattern.push_back(-1);
                    else if (ch == '/') pattern.push_back(2);
                    else if (ch == '\\') pattern.push_back(-2);
                }

                std::string patternStr;
                for (size_t pi = 0; pi < pattern.size(); ++pi) {
                    patternStr += geode::utils::numToString(pattern[pi]);
                    if (pi + 1 < pattern.size()) patternStr += ",";
                }

                if (!isNot) {
                    cMP.omType = omType;
                    cMP.pattern = pattern;
                    cMP.offset = offset;
                } else {
                    cMP.notTypes.push_back(omType);
                    cMP.notPatterns.push_back(pattern);
                    cMP.notOffsets.push_back(offset);
                }
            }
            continue;
        }
        if (l == "# else #") {
            if (!cMP.pattern.empty() || !cMP.notPatterns.empty()) {
                cMPList.push_back(cMP);
                cMP = ThemeMatch();
                cMP.omType = OMType::Corridor;
                cMP.notTypes = {};
                cMP.pattern = {};
                cMP.offset = 0;
                cMP.notPatterns = {};
                cMP.notOffsets = {};
                cMP._else = true;
            }
            continue;
        } else if (l == "# end if #") {
            inMatches = false;

            if (cMP._else || !cMP.pattern.empty() || !cMP.notPatterns.empty()) {
                cMPList.push_back(cMP);
                cMP = ThemeMatch();
            }
            matchConditions.push_back(cMPList);
            cMPList = std::vector<ThemeMatch>();
            continue;
        }
        if (inMatches) {
            cMP.commands.push_back(l);
            continue;
        }

        if (l.find("# pattern") == 0 && l.back() == '#') {
            int pStart = 0, pRepeat = 0;
            std::istringstream iss(l.substr(9, l.size() - 10));
            if (iss >> pStart >> pRepeat) {
                patternGen.start = pStart;
                patternGen.repeat = pRepeat;
                inPattern = true;
            }
            continue;
        } else if (l == "# end pattern #") {
            inPattern = false;
            if (!patternGen.data.empty()) {
                repeatingPatterns.push_back(patternGen);
                patternGen = RepeatingPattern();
            }
            continue;
        }
        if (inPattern) {
            patternGen.data.push_back(l);
            continue;
        }
    }


    const int trueLength = (biome.options.length * 30);
    for (const auto& pattern : repeatingPatterns) {
        int patternRepeat = pattern.repeat >= 1 ? pattern.repeat : 1;
        int loopCount = std::min(((trueLength + 345) / pattern.repeat) + 1, 10000);
        int n = pattern.start;
        for (int i = 0; i < loopCount; ++i) {
            for (const auto blockData : pattern.data) {
                themeGen += parseAddBlock(
                    blockData,
                    n, 0,
                    biome.options.maxHeight,
                    biome.options.minHeight,
                    biome.options.corridorHeight,
                    true);
            }
            n += pattern.repeat;
        }
    }

    // conditional pattern matching runs -10 outside bounds; if no floor override is given,
    // jfp will attempt to theme the starting and ending connectors automatically.
    for (int i = -10; i < static_cast<int>(biome.segments.size()) + 10; i++) {
        bool mini;
        int passedCH = biome.options.corridorHeight;
        int y_swing;
        bool isTransition;
        int x, y;

        if (i < 0) {
            mini = biome.segments[0].options.mini;
            y_swing = 0;
            isTransition = false;
        } else if (i >= static_cast<int>(biome.segments.size())) {
            mini = biome.segments.back().options.mini;
            y_swing = 0;
            isTransition = false;
        } else {
            const auto& seg = biome.segments[i];
            
            y_swing = seg.y_swing;
            mini = seg.options.mini;
            isTransition = seg.options.isTransition;
        }

        if (mini && biome.options.typeA) passedCH += 30;
        
        if (isTransition) {
            if (y_swing == 1 && !mini) {
                passedCH += 30;
            } else if (y_swing == 1 && mini) {
                passedCH -= 30;
            }
            const auto& seg = biome.segments[i];
        }

        for (const auto& condition : matchConditions) {
            for (const auto& match : condition) {
                bool successfulMatch = false;

                bool notPatternsOk = true;
                for (int np = 0; np < match.notPatterns.size(); ++np) {
                    bool notsom = strictOM(
                            biome.segments,
                            i,
                            match.notOffsets[np],
                            match.notPatterns[np],
                            match.notTypes[np],
                            biome.options.typeA);
                    if (notsom) {
                        notPatternsOk = false;
                        break;
                    }
                }


                bool som = strictOM(
                        biome.segments,
                        i,
                        match.offset,
                        match.pattern,
                        match.omType,
                        biome.options.typeA) ||
                      (match._else && match.pattern.empty());
                // if (i == biome.segments.size() - 1 && notPatternsOk && som) log::info("this just passed");
                if (notPatternsOk &&
                    (som)) {
                    for (const auto& block : match.commands) {
                        // calculate extra segment coords
                        if (i < 0) {
                            const auto& seg = biome.segments[0];
                            x = seg.coords.first + 30 * i;
                            if (match.omType == OMType::Ceiling) {
                                y = seg.coords.second - 30 * (seg.options.mini ? 2 : 1) * (i + 1);
                            } else {
                                y = seg.coords.second + 30 * (seg.options.mini ? 2 : 1) * i;
                            }
                        } else if (i > static_cast<int>(biome.segments.size()) - 1) {
                            const auto& seg = biome.segments.back();
                            int distFromEnd = i - (static_cast<int>(biome.segments.size()) - 1);
                            x = seg.coords.first + 30 * distFromEnd;
                            int direction = 1;
                            if (match.omType == OMType::Floor) {
                                direction = -1;
                            }

                            if ((match.omType == OMType::Ceiling && seg.y_swing == -1) ||
                                    (match.omType == OMType::Floor && seg.y_swing == 1)) {
                                y = seg.coords.second + 30 * (seg.options.mini ? 2 : 1) * (distFromEnd - 1) * direction;
                            } else {
                                y = seg.coords.second + 30 * (seg.options.mini ? 2 : 1) * distFromEnd * direction;
                            }
                        } else {
                            const auto& seg = biome.segments[i];
                            x = seg.coords.first;
                            y = seg.coords.second;
                            
                        }

                        std::string parsed = parseAddBlock(
                            block, x, y,
                            biome.options.maxHeight, biome.options.minHeight, passedCH);
                        if (!parsed.empty()) {
                            themeGen += parsed;
                        }
                    }
                    successfulMatch = true;
                }

                if (successfulMatch) {
                    break;
                }
            }
        }
    }

    auto& corridorBlocksSize = overrideBankS["corridorblock"].standard;
    auto& corridorBlocksFillSize = overrideBankS["corridorblock-fill"].standard;
    for (int i = 0; i < biome.segments.size(); i++) {
        const auto& seg = biome.segments[i];

        int passedCH = biome.options.corridorHeight;
        if (seg.options.mini && biome.options.typeA) passedCH += 30;
        bool mini = seg.options.mini;

        if (!corridorBlocksSize.blocks.empty()) {
            for (const auto& corridorBlock : corridorBlocksSize.blocks) {
                if (corridorBlock == "") continue;
                int numCB = (passedCH / 30) - (mini ? 2 : 1);
                for (int j = 1; j <= numCB; j++) {
                    themeGen += parseAddBlock(
                        corridorBlock, seg.coords.first, seg.coords.second + j*30,
                        biome.options.maxHeight, biome.options.minHeight, passedCH);
                }
            }
        }
        if (!corridorBlocksFillSize.blocks.empty()) {
            for (const auto& corridorBlock : corridorBlocksFillSize.blocks) {
                if (corridorBlock == "") continue;
                int numCB = (passedCH / 30) - (mini ? 2 : 1);
                for (int j = 1; j <= numCB; j++) {
                    themeGen += parseAddBlock(
                        corridorBlock, seg.coords.first, seg.coords.second + j*30,
                        biome.options.maxHeight, biome.options.minHeight, passedCH);
                }
                if (passedCH % 30 != 0) themeGen += parseAddBlock(
                    corridorBlock, seg.coords.first, seg.coords.second + passedCH - (mini ? 60 : 30),
                    biome.options.maxHeight, biome.options.minHeight, passedCH);
            }
        }

        // no such thing as a "transition" segment on typeB
        if (seg.options.isTransition) {
            if (seg.y_swing == 1 && !mini) passedCH += 30;
            else if (seg.y_swing == 1 && mini) passedCH -= 30;
        }

        // The section below is for theming the special overrides (speed changes, portals, spikes etc.)
        // Much of the code is ripped directly from StringGen, as the components here are positioned relative
        // to their target object. This seems excessive and might be trimmed down/compartmentalized later. -M
        int xP, yP;
        float rPdeg, scaleP;
        if (seg.options.isPortal == JFPGen::Portals::Gravity || seg.options.isPortal == JFPGen::Portals::Fake) {
            double portalFactor = (static_cast<double>(passedCH) / 60.0) * 1.414;
            int portalNormal = passedCH / 10;
            int portalPos = passedCH / 4;
            bool portalYellow;
            if (seg.options.isPortal == JFPGen::Portals::Fake)
                portalYellow = seg.options.gravity ? false : true;
            else portalYellow = seg.options.gravity ? true : false;

            xP = seg.coords.first - 15 - portalNormal + portalPos;
            yP = seg.coords.second + (seg.y_swing == 1
                ? portalNormal + portalPos - 15
                : passedCH + 15 - portalNormal - portalPos);
            if (mini && seg.y_swing == 1) yP -= 30;
            rPdeg = (mini ? 26.565 : 45) * seg.y_swing;
            scaleP = portalFactor / 2.5;
            if (passedCH > 60) scaleP *= 0.85;

            auto portalBank = portalYellow ? overrideBankS["yellow-portal"] : overrideBankS["blue-portal"];
            auto portalBlocks = mini ? portalBank.mini.blocks : portalBank.standard.blocks;
            for (const auto& block : portalBlocks) {
                std::string parsed = parseAddBlock(
                    block, xP, yP,
                    biome.options.maxHeight, biome.options.minHeight, passedCH, false, rPdeg, scaleP);
                if (!parsed.empty()) {
                    themeGen += parsed;
                }
            }
        }

        // Corridor-Spikes
        if (seg.options.isSpikeM) {
            bool spikeSide = seg.options.spikeSide;
            int xS = seg.coords.first - 6;
            if (!spikeSide) {
                xS = seg.coords.first + 6;
            }
            int yS = seg.coords.second + 6;
            if (mini) yS -= 4;
            if ((seg.y_swing == 1 && !spikeSide) || (seg.y_swing == -1 && spikeSide)) {
                yS = seg.coords.second + passedCH - 6;
                if (mini) yS += 4;
            }

            // this has to use rotations, spikes don't support being flipped
            float scaleS = 1.f;
            float rOffset = 45.f;
            if (mini) {
                yS -= 15;
                rOffset = 63.435f;
                if (!spikeSide) xS -= 1;
                else xS += 1;
            }
            int rS = rOffset;
            if (seg.y_swing == 1 && !spikeSide) {
                rS = 180 - rOffset;
            } else if (seg.y_swing == 1 && spikeSide) {
                rS = -rOffset;
            } else if (seg.y_swing == -1 && spikeSide) {
                rS = -180 + rOffset;
            }

            auto spikeBlocks = mini ? overrideBankS["spike"].mini.blocks :
                overrideBankS["spike"].standard.blocks;
            for (const auto& block : spikeBlocks) {
                std::string parsed = parseAddBlock(
                    block, xS, yS,
                    biome.options.maxHeight, biome.options.minHeight, passedCH, false, rS, scaleS);
                if (!parsed.empty()) {
                    themeGen += parsed;
                }
            }
        }

        // Speed-Changes
        if (seg.options.speedChange != JFPGen::SpeedChange::None) {
            int speedID = JFPGen::convertSpeed(seg.options.speedChange);

            int spY = seg.coords.second + passedCH/2 + 14 * (seg.y_swing == 1 ? -1 : 1);
            if (mini && seg.y_swing == 1) spY -= 30;
            int spR = (mini ? 63.435 : 45) * -seg.y_swing;
            float speedFactor = (mini ? 0.3 : 0.5) * (passedCH / 60.0);

            auto speedBank = overrideBankS["speed-05x"];
            if (seg.options.speedChange == JFPGen::SpeedChange::Speed1x) {
                speedBank = overrideBankS["speed-1x"];
            } else if (seg.options.speedChange == JFPGen::SpeedChange::Speed2x) {
                speedBank = overrideBankS["speed-2x"];
            } else if (seg.options.speedChange == JFPGen::SpeedChange::Speed3x) {
                speedBank = overrideBankS["speed-3x"];
            } else if (seg.options.speedChange == JFPGen::SpeedChange::Speed4x) {
                speedBank = overrideBankS["speed-4x"];
            }
            auto speedBlocks = mini ? speedBank.mini.blocks :
                speedBank.standard.blocks;
            for (const auto& block : speedBlocks) {
                std::string parsed = parseAddBlock(
                    block, seg.coords.first - 14, spY,
                    biome.options.maxHeight, biome.options.minHeight, passedCH, false, spR, speedFactor);
                if (!parsed.empty()) {
                    themeGen += parsed;
                }
            }
        }

        if (i > 0 && mini != biome.segments[i - 1].options.mini) {
            if (biome.options.typeA) {
                float ySP, xSP, rSP, scaleSP;
                if (seg.options.isPortal == JFPGen::Portals::None) {
                    xSP = seg.coords.first - 26;
                    ySP = biome.segments[i - 1].coords.second + 15 +
                        (biome.options.corridorHeight - 30) / 2;
                    ySP += biome.segments[i - 1].y_swing * 4;
                    rSP = (std::atan((biome.options.corridorHeight - 30.f) / 30.f) * 180/M_PI - 90) *
                        biome.segments[i - 1].y_swing;
                    scaleSP = std::sqrt(std::pow((biome.options.corridorHeight - 30), 2) + 900) / 110.f;
                } else {
                    xSP = xP;
                    ySP = yP;
                    rSP = rPdeg;
                    scaleSP = scaleP;
                }

                auto spikeBlocks = mini ? overrideBankS["mini-portal"].mini.blocks :
                    overrideBankS["big-portal"].standard.blocks;
                for (const auto& block : spikeBlocks) {
                    std::string parsed = parseAddBlock(
                        block, xSP, ySP,
                        biome.options.maxHeight, biome.options.minHeight, passedCH, false, rSP, scaleSP);
                    if (!parsed.empty()) {
                        themeGen += parsed;
                    }
                }
            } else {
                float ySP, xSP, rSP = 0, scaleSP;
                if (seg.options.isPortal == JFPGen::Portals::None) {
                    ySP = seg.coords.second + passedCH / 2.0f;
                    if (seg.y_swing == 1) {
                        if (mini) {
                            ySP -= 45;
                        } else {
                            ySP -= 15;
                        }
                    } else {
                        ySP += 15;
                    }
                    scaleSP = (passedCH / 90.f) * 0.8;
                    xSP = seg.coords.first - 14 + 13 * scaleSP;
                } else {
                    xSP = xP;
                    ySP = yP;
                    rSP = rPdeg;
                    scaleSP = scaleP;
                }

                auto spikeBlocks = mini ? overrideBankS["mini-portal"].mini.blocks :
                    overrideBankS["big-portal"].standard.blocks;
                for (const auto& block : spikeBlocks) {
                    std::string parsed = parseAddBlock(
                        block, xSP, ySP,
                        biome.options.maxHeight, biome.options.minHeight, passedCH, false, rSP, scaleSP);
                    if (!parsed.empty()) {
                        themeGen += parsed;
                    }
                }
            }
        }
    }

    return themeGen;
}

ThemeMetadata parseThemeMeta(const std::string& name) {
    bool inMetadata = false;
    ThemeMetadata metadata;

    std::string themeName = name;
    if (themeName.size() < 5 || themeName.substr(themeName.size() - 5) != ".jfpt") {
        themeName += ".jfpt";
    }
    std::string themeRaw = file::readString(Mod::get()->getSaveDir() / "themes" / themeName).unwrapOrDefault();
    std::istringstream iss(themeRaw);
    
    std::vector<std::string> lines;
    std::string line;
    uint32_t ctr = 0;
    while (std::getline(iss, line)) {
        line.erase(0, line.find_first_not_of("\t\r\n"));
        line.erase(line.find_last_not_of("\t\r\n") + 1);
        lines.push_back(line);
        ctr++;
        if (ctr > 20000) break;
    }

    for (const auto& l : lines) {
        // TODO(M): rework into a switch case or a neater conditional
        if (l == "# metadata #") {
            inMetadata = true;
            continue;
        } else if (l == "# end metadata #") {
            inMetadata = false;
            continue;
        }
        if (inMetadata) {
            auto pos = l.find(':');
            if (pos != std::string::npos) {
                std::string key = l.substr(0, pos);
                std::string value = l.substr(pos + 1);

                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                if (key == "Name") metadata.name = value;
                else if (key == "Author") metadata.author = value;
                else if (key == "Version") metadata.version = value;
                else if (key == "Biome") metadata.biome = value;
                else if (key == "Tags") {
                    metadata.tags.clear();
                    size_t start = 0, end = 0;
                    while ((end = value.find(", ", start)) != std::string::npos) {
                        metadata.tags.push_back(value.substr(start, end - start));
                        start = end + 2;
                    }
                    metadata.tags.push_back(value.substr(start));
                }
                else if (key == "Date") metadata.date = value;
                else if (key == "Pack") metadata.pack = value;
                else if (key == "Type") metadata.type = value;
                else if (key == "Description") metadata.description = value;
            }
            continue;
        }
    }

    return metadata;
}

std::vector<std::string> tagConflicts(ThemeMetadata tmd) {
    auto* mod = Mod::get();
    std::vector<std::string> conflicts;

    const bool isBigPresent =
            mod->getSavedValue<uint8_t>("opt-0-starting-size") != 1 ||
        mod->getSavedValue<bool>("opt-0-using-size-changes");
    const bool isMiniPresent =
            mod->getSavedValue<uint8_t>("opt-0-starting-size") != 0 ||
        mod->getSavedValue<bool>("opt-0-using-size-changes");
    const bool isTypeA = mod->getSavedValue<bool>("opt-0-using-size-changes") &&
        !(mod->getSavedValue<uint8_t>("opt-0-size-transition-type"));

    if (isMiniPresent && std::find(tmd.tags.begin(), tmd.tags.end(), "bigwave-only") !=
            tmd.tags.end()) {
        conflicts.push_back("bigwave-only");
    }
    if (isBigPresent && std::find(tmd.tags.begin(), tmd.tags.end(), "miniwave-only") !=
            tmd.tags.end()) {
        conflicts.push_back("miniwave-only");
    }
    if (isTypeA && std::find(tmd.tags.begin(), tmd.tags.end(), "type-b-only") !=
            tmd.tags.end()) {
        conflicts.push_back("type-b-only");
    }
    if (!isTypeA && std::find(tmd.tags.begin(), tmd.tags.end(), "type-a-only") !=
            tmd.tags.end()) {
        conflicts.push_back("type-a-only");
    }
    if (std::find(tmd.tags.begin(), tmd.tags.end(), "domu_game") != tmd.tags.end()) {
        conflicts.push_back("domu_game");
    }

    return conflicts;
}

}  // namespace ThemeGen
