#include "Theming.hpp"
#include <string>
#include <fstream>
#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include "Ninja.hpp"

using namespace geode::prelude;

namespace ThemeGen {

std::array<int, 3> hexToColor(const std::string& hex) {
    std::array<int, 3> color = {255, 255, 255};
    if (hex.size() == 7 && hex[0] == '#') {
        try {
            color[0] = std::stoi(hex.substr(1, 2), nullptr, 16);
            color[1] = std::stoi(hex.substr(3, 2), nullptr, 16);
            color[2] = std::stoi(hex.substr(5, 2), nullptr, 16);
        } catch (const std::invalid_argument&) {}
    }
    return color;
}

std::string parseAddBlock(std::string addBlockLine, float X, float Y, int maxHeight, int minHeight, int corridorHeight) {

    // remove legacy prefix
    if (addBlockLine.empty()) return "";
    const std::string prefix = "Add Block";
    auto pos = addBlockLine.find(prefix);
    if (pos != std::string::npos) {
        addBlockLine = addBlockLine.substr(pos + prefix.length());
    }

    addBlockLine.erase(0, addBlockLine.find_first_not_of(" \t\r\n"));
    addBlockLine.erase(addBlockLine.find_last_not_of(" \t\r\n") + 1);

    // eval bracketed arithmetic
    std::string result;
    size_t i = 0;
    while (i < addBlockLine.size()) {
        if (addBlockLine[i] == '[') {
            size_t end = addBlockLine.find(']', i);
            if (end != std::string::npos) {
                std::string expr = addBlockLine.substr(i + 1, end - i - 1);

                expr.erase(remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
                float value = 0.f;

                // throw in xpos, ypos, corridorheight values
                std::string parsedExpr;
                for (size_t j = 0; j < expr.size(); ++j) {
                    if (expr[j] == 'X') {
                        parsedExpr += std::to_string(X);
                    } else if (expr[j] == 'Y') {
                        parsedExpr += std::to_string(Y);
                    } else if (expr[j] == 'C') {
                        parsedExpr += std::to_string(corridorHeight);
                    } else {
                        parsedExpr += expr[j];
                    }
                }

                // evaluate the parsedExpr as a left-to-right sum/sub/mul/div (no operator precedence)
                float acc = 0.f;
                char lastOp = 0;
                size_t idx = 0;
                while (idx < parsedExpr.size()) {
                    size_t nextOp = parsedExpr.find_first_of("+-*/", idx);
                    std::string numStr = parsedExpr.substr(idx, nextOp - idx);
                    float num = 0.f;
                    try {
                        num = std::stof(numStr);
                    } catch (...) {
                        num = 0.f;
                    }
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
                    }
                    if (nextOp == std::string::npos) break;
                    lastOp = parsedExpr[nextOp];
                    idx = nextOp + 1;
                }
                value = acc;

                // TODO: rework this section - it's intended to cut around the field of view to prevent obj hell
                if (expr.find("Y") != std::string::npos) {
                    if (value > 375 || value < -15) {
                        return "";
                    }
                }

                result += std::to_string(static_cast<int>(value));
                i = end + 1;
            } else {
                result += addBlockLine[i++];
            }
        } else {
            result += addBlockLine[i++];
        }
    }

    if (!result.empty() && result.back() != ';') {
        result += ';';
    }
    return result;
}

std::string parseTheme(const std::string& name, const JFPGen::LevelData& ldata) {
    bool inMetadata = false;
    bool inAddcolor = false;
    bool inKValues = false;
    bool inMatches = false;
    bool inPattern = false;
    InOverride inOverride = InOverride::None;

    RepeatingPattern patternGen = RepeatingPattern();
    ThemeMatch cMP = ThemeMatch();
    ThemeMetadata metadata;
    std::vector<std::vector<ThemeMatch>> matchConditions;
    std::vector<ThemeMatch> cMPList;
    std::vector<RepeatingPattern> repeatingPatterns;
    std::string themeGen = "";

    overrideBank["override-base"] = false;
    overrideBank["override-enddown"] = false;
    overrideBank["override-endup"] = false;

    JFPGen::Color sColor = JFPGen::Color();
    
    auto biome = ldata.biomes[0];

    auto localPath = CCFileUtils::sharedFileUtils();
    std::string themeName = name;
    if (themeName.size() < 5 || themeName.substr(themeName.size() - 5) != ".jfpt") {
        themeName += ".jfpt";
    }
    std::string fp = std::string(localPath->getWritablePath()) + "/jfp/themes/" + themeName;
    std::ifstream file(fp);
    if (!file.is_open()) {
        return "";
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    for (const auto& l : lines) {
        // TODO: rework into a switch case or a neater conditional
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
                else if (key == "Date") metadata.date = value;
                else if (key == "Pack") metadata.pack = value;
                else if (key == "Type") metadata.type = value;
                else if (key == "Description") metadata.description = value;
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
                        try {
                            sColor.copyColor = std::stoi(value.substr(5));
                        } catch (...) {
                            sColor.copyColor = -1;
                        }
                    } else {
                        sColor.rgb = hexToColor(value);
                    }
                } else if (key == "Slot") {
                    try {
                        sColor.slot = std::stoi(value);
                    } catch (const std::invalid_argument&) {
                        sColor.slot = 13;
                    }
                } else if (key == "Blending") {
                    sColor.blending = (value == "true" || value == "1");
                } else if (key == "Opacity") {
                    try {
                        sColor.opacity = std::stof(value);
                    } catch (const std::invalid_argument&) {
                        sColor.opacity = 1.0f;
                    }
                } else if (key == "Special") {
                    sColor.special = value;
                }
            }
            continue;
        }

        if (l == "# override base #" || l == "# override #") {
            inOverride = InOverride::Base;
            overrideBank["override-base"] = true;
            continue;
        } else if (l == "# override enddown #") {
            inOverride = InOverride::EndDown;
            overrideBank["override-enddown"] = true;
            continue;
        } else if (l == "# override endup #") {
            inOverride = InOverride::EndUp;
            overrideBank["override-endup"] = true;
            continue;
        } else if (l == "# end override #") {
            inOverride = InOverride::None;
            continue;
        }
        if (inOverride == InOverride::Base) {
            themeGen += parseAddBlock(
                l,
                0,
                0,
                biome.options.maxHeight,
                biome.options.minHeight,
                biome.options.corridorHeight
            );
            continue;
        } else if (inOverride == InOverride::EndUp && biome.segments.back().y_swing == 1) {
            themeGen += parseAddBlock(
                l,
                biome.segments.back().coords.first,
                biome.segments.back().coords.second,
                biome.options.maxHeight,
                biome.options.minHeight,
                biome.options.corridorHeight
            );
            continue;
        } else if (inOverride == InOverride::EndDown && biome.segments.back().y_swing == -1) {
            themeGen += parseAddBlock(
                l,
                biome.segments.back().coords.first,
                biome.segments.back().coords.second,
                biome.options.maxHeight,
                biome.options.minHeight,
                biome.options.corridorHeight
            );
            continue;
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

        if (!l.empty() && 
                ((l.find("# if corridor ") == 0 && l.back() == '#') ||
                (l.find("# else if corridor ") == 0 && l.back() == '#'))) {
            inMatches = true;
            if (l.find("# if corridor ") == 0) {
                cMP = ThemeMatch();
            } else if (l.find("# else if corridor ") == 0) {
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
                if (bracketStart != std::string::npos && bracketEnd != std::string::npos && bracketEnd > bracketStart) {
                    symbols = t.substr(0, bracketStart);
                    std::string offsetStr = t.substr(bracketStart + 1, bracketEnd - bracketStart - 1);

                    int displacement = 0;
                    for (size_t idx = bracketEnd + 1; idx < t.size(); ++idx) {
                        if (t[idx] == '+' || t[idx] == '-' || t[idx] == '/' || t[idx] == '\\') {
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
                    patternStr += std::to_string(pattern[pi]);
                    if (pi + 1 < pattern.size()) patternStr += ",";
                }

                if (!isNot) {
                    cMP.pattern = pattern;
                    cMP.offset = offset;
                } else {
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

        if (l == "# pattern #") {
            inPattern = true;
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
            auto pos = l.find(':');
            if (pos != std::string::npos) {
                std::string key = l.substr(0, pos);
                std::string value = l.substr(pos + 1);

                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                if (key == "Data") {
                    patternGen.data = value;
                } else if (key == "Start") {
                    try {
                        patternGen.start = std::stoi(value);
                    } catch (...) {
                        patternGen.start = 195;
                    }
                } else if (key == "Repeat") {
                    try {
                        patternGen.repeat = std::stoi(value);
                        if (patternGen.repeat < 1) {
                            patternGen.repeat = 1;
                        }
                    } catch (...) {
                        patternGen.repeat = 30;
                    }
                }
            }
            continue;
        }
    }


    const int trueLength = (biome.options.length * 30);
    for (const auto& pattern : repeatingPatterns) {
        int loopCount = std::min((trueLength / pattern.repeat) + 1, 1000);
        int n = pattern.start;
        for (int i = 0; i < loopCount; ++i) {
            themeGen += parseAddBlock(
                pattern.data, 
                n, 0, 
                biome.options.maxHeight, 
                biome.options.minHeight, 
                biome.options.corridorHeight
            );
            n += pattern.repeat;
        }
    }

    for (int i = 0; i < biome.segments.size(); i++) {

        for (const auto& condition : matchConditions) {
            for (const auto& match : condition) {
                bool successfulMatch = false;

                bool notPatternsOk = true;
                for (int np = 0; np < match.notPatterns.size(); ++np) {
                    if (JFPGen::orientationMatch(
                            biome.segments, 
                            i + match.notOffsets[np] + 1, 
                            match.notPatterns[np])) {
                        notPatternsOk = false;
                        break;
                    }
                }

                if (notPatternsOk &&
                    (JFPGen::orientationMatch(biome.segments, i + match.offset + 1, match.pattern, true) ||
                     (match._else && match.pattern.empty()))) {
                    
                    for (const auto& cmd : match.commands) {
                        std::string parsed = parseAddBlock(
                            cmd, biome.segments[i].coords.first, biome.segments[i].coords.second,
                            biome.options.maxHeight, biome.options.minHeight, biome.options.corridorHeight);
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
    
    return themeGen;
}

}