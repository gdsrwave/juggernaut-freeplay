// Copyright 2025 GDSRWave
#include <string>
#include <vector>
#include <Geode/Geode.hpp>
#include "./Ninja.hpp"
#include "OptionStr.hpp"

using namespace geode::prelude;

std::string exportSettings(const std::vector<PackedEntry>& entries) {
    int totalBits = 0;
    for (const auto& entry : entries) totalBits += entry.sizeSize;
    int totalBytes = (totalBits + 7) / 8;
    std::vector<uint8_t> bytes(totalBytes, 0);

    int bitPos = 0;
    for (const auto& entry : entries) {
        uint32_t value = entry.valueVal;
        for (int b = 0; b < entry.sizeSize; b++) {
            if (value & (1 << (entry.sizeSize - b - 1))) {
                bytes[bitPos / 8] |= (1 << (7 - (bitPos % 8)));
            }
            bitPos++;
        }
    }

    uint8_t ver = readStoredNum(bytes, 0, 6);
    if (ver == 37) v37tov38(bytes);

    static const char b64_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string res;
    int val = 0, valb = -6;
    for (int i = 0; i < totalBytes; i++) {
        val = (val << 8) | bytes[i];
        valb += 8;
        while (valb >= 0) {
            res.push_back(b64_table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) res.push_back(b64_table[((val << 8) >> (valb + 8)) & 0x3F]);

    std::string lenSuffix =
        fmt::format("+{}", Mod::get()->getSavedValue<uint32_t>("opt-0-length"));

    return res + lenSuffix;
}

void importSettingsOld(std::string packed) {
    auto* mod = Mod::get();

    size_t ls = packed.find('+');
    if (ls == std::string::npos) {
        return FLAlertLayer::create("Error", "Length suffix not found", "OK")->show();
    }
    int length = geode::utils::numFromString<int>(packed.substr(ls + 1, std::string::npos)).unwrapOr(-1);
    mod->setSettingValue("length", length);

    std::string dataPack = packed.substr(0, ls);

    static const char b64_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::vector<uint8_t> bytes;
    
    int val = 0, valb = -8;
    for (char c : dataPack) {
        const char* p = std::strchr(b64_table, c);
        if (!p) continue;
        int idx = static_cast<int>(p - b64_table);
        val = (val << 6) | idx;
        valb += 6;
        if (valb >= 0) {
            bytes.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    uint8_t ver = readStoredNum(bytes, 0, 6);
    if (ver == 37) v37tov38(bytes);

    // std::string bitStr;
    // for (int i = 0; i < (bytes.size() * 8); i++) {
    //     bitStr += ((bytes[i / 8] & (1 << (7 - (i % 8)))) ? '1' : '0');
    // }
    // log::info("Bit string: {}", bitStr);
    uint8_t biome = readStoredNum(bytes, 6, 4);
    if (biome == static_cast<int>(JFPGen::JFPBiome::Juggernaut)) {
        auto getSpeedChange = [](int8_t val) -> std::string {
            switch (val) {
            case 5: return "0.5x";
            case 1: return "1x";
            case 2: return "2x";
            case 3: return "3x";
            case 4: return "4x";
            default: return "1x";
            }
        };

        std::string sspeedStr = getSpeedChange(readStoredNum(bytes, 10, 3));
        std::string maxSpeedStr = getSpeedChange(readStoredNum(bytes, 13, 3));
        std::string minSpeedStr = getSpeedChange(readStoredNum(bytes, 16, 3));

        mod->setSettingValue<std::string>("starting-speed", sspeedStr);
        mod->setSettingValue<std::string>("max-speed", maxSpeedStr);
        mod->setSettingValue<std::string>("min-speed", minSpeedStr);

        mod->setSettingValue<bool>("corridor-widening",
            static_cast<bool>(readStoredNum(bytes, 43, 1)));

        mod->setSettingValue<float>("corridor-height", readStoredNum(bytes, 44, 8));

        std::array<std::string, 7> corridorRules = {
            "NSNZ",
            "NS",
            "Juggernaut",
            "Unrestricted",
            "LRD",
            "Random",
            "Limp"
        };
        size_t cri = readStoredNum(bytes, 64, 4);
        std::string crstr = (cri >= 0 && cri < corridorRules.size())
            ? corridorRules[cri]
            : "Unrestricted";
        mod->setSettingValue<std::string>("corridor-rules", crstr);

        std::array<std::string, 4> diffs = {
            "None",
            "Light",
            "Balanced",
            "Aggressive"
        };
        size_t p_i = readStoredNum(bytes, 68, 3);
        std::string pstr = (p_i >= 0 && p_i < diffs.size()) ? diffs[p_i] : "None";
        mod->setSettingValue<std::string>("portals", pstr);

        mod->setSettingValue<bool>("fake-gravity-portals",
            static_cast<bool>(readStoredNum(bytes, 71, 1)));
        mod->setSettingValue<bool>("upside-start",
            static_cast<bool>(readStoredNum(bytes, 72, 2)));


        std::array<std::string, 3> inputTypes = {
            "Both",
            "Releases",
            "Holds"
        };
        size_t piRaw = readStoredNum(bytes, 74, 3);
        std::string piStr = (piRaw >= 0 && piRaw < inputTypes.size()) ? inputTypes[piRaw] : "Both";
        mod->setSettingValue<std::string>("portal-inputs", piStr);

        mod->setSettingValue<bool>("corners",
            static_cast<bool>(readStoredNum(bytes, 77, 1)));
        mod->setSettingValue<bool>("low-vis",
            static_cast<bool>(readStoredNum(bytes, 78, 2)));

        std::array<std::string, 5> colorMode = {
            "Washed",
            "All Colors",
            "Classic Mode",
            "Night Mode",
            "Random"
        };
        size_t cmi = readStoredNum(bytes, 80, 3);
        std::string cmstr = (cmi >= 0 && cmi < colorMode.size())
            ? colorMode[cmi]
            : "Washed";
        mod->setSettingValue<std::string>("color-mode", cmstr);

        std::array<std::string, 3> csizes = {
            "Both",
            "Big Wave",
            "Mini Wave"
        };
        size_t csp = readStoredNum(bytes, 83, 2);
        if (csp > 0) csp--;
        std::string splstr = (csp >= 0 && csp < csizes.size()) ? csizes[csp] : "Both";
        bool corridorSpikes = csp > 0;
        mod->setSettingValue<bool>("corridor-spikes",
            static_cast<bool>(corridorSpikes));
        mod->setSettingValue<std::string>("spike-placement", splstr);
        mod->setSettingValue<bool>("fuzzy-spikes",
            static_cast<bool>(readStoredNum(bytes, 85, 2)));

        size_t sc_i = readStoredNum(bytes, 87, 3);
        std::string scstr = (sc_i >= 0 && sc_i < diffs.size()) ? diffs[sc_i] : "None";
        mod->setSettingValue<std::string>("changing-speed", scstr);

        std::string sizeStr = readStoredNum(bytes, 90, 2) ? "Mini" : "Big";
        mod->setSettingValue<std::string>("starting-size", sizeStr);

        mod->setSettingValue<bool>("changing-size",
            static_cast<bool>(readStoredNum(bytes, 92, 3)));

        std::string ttStr = readStoredNum(bytes, 95, 2) ? "Type A" : "Type B";
        mod->setSettingValue<std::string>("transition-type", ttStr);

        mod->setSettingValue<bool>("portals-in-spams",
            !static_cast<bool>(readStoredNum(bytes, 97, 2)));
    }
}

void v37tov38(std::vector<uint8_t>& bytes) {
    uint8_t startingSpeed = readStoredNum(bytes, 10, 3);
    if (startingSpeed == 5) startingSpeed = 0;
    else if (startingSpeed == 0) startingSpeed = 5;
    writeStoredNum(bytes, 10, 3, startingSpeed);

    uint8_t corridorRules = readStoredNum(bytes, 64, 4);
    if (corridorRules == 5) corridorRules = 0;
    else if (corridorRules < 5) corridorRules++;
    writeStoredNum(bytes, 64, 4, corridorRules);

    uint8_t colorMode = readStoredNum(bytes, 80, 3);
    if (colorMode == 4) colorMode = 0;
    else if (colorMode < 4) colorMode++;
    writeStoredNum(bytes, 80, 3, colorMode);

    uint8_t tType = readStoredNum(bytes, 95, 2);
    if (tType == 1) tType = 0;
    else if (tType == 0) tType = 1;
    writeStoredNum(bytes, 95, 2, tType);

    uint8_t maxSpeed = readStoredNum(bytes, 13, 3);
    uint8_t minSpeed = readStoredNum(bytes, 16, 3);
    if (maxSpeed == 5) maxSpeed = 0;
    else if (maxSpeed == 0) maxSpeed = 5;
    if (minSpeed == 5) minSpeed = 0;
    else if (minSpeed == 0) minSpeed = 5;
    uint8_t selectedSpeeds = 0;
    if (maxSpeed >= minSpeed && maxSpeed <= 4) {
        for (uint8_t i = minSpeed; i <= maxSpeed; ++i) {
            selectedSpeeds |= (1 << i);
        }
    }
    writeStoredNum(bytes, 13, 6, selectedSpeeds);

    FLAlertLayer::create("Alert", "This code was copied from an older version of JFP.\n"
        "Please import, refresh, and copy the latest version of this code.", "OK")->show();
}

void importSettings(std::string packed) {
    auto* mod = Mod::get();

    size_t ls = packed.find('+');
    if (ls == std::string::npos) {
        return FLAlertLayer::create("Error", "Length suffix not found", "OK")->show();
    }
    int length = geode::utils::numFromString<int>(packed.substr(ls + 1, std::string::npos)).unwrapOr(-1);
    mod->setSettingValue("length", length);

    std::string dataPack = packed.substr(0, ls);

    static const char b64_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::vector<uint8_t> bytes;
    
    int val = 0, valb = -8;
    for (char c : dataPack) {
        const char* p = std::strchr(b64_table, c);
        if (!p) continue;
        int idx = static_cast<int>(p - b64_table);
        val = (val << 6) | idx;
        valb += 6;
        if (valb >= 0) {
            bytes.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    uint8_t ver = readStoredNum(bytes, 0, 6);
    // this was added in JFP 1.7.0; option codes have their own versioning, which changes
    // when code-breaking changes are made to the options system. intended to not be often.
    if (ver < 38) v37tov38(bytes);

    uint8_t biome = readStoredNum(bytes, 6, 4);
    if (biome != static_cast<uint8_t>(JFPGen::JFPBiome::Juggernaut)) return;

    mod->setSavedValue<uint8_t>("opt-0-starting-speed", readStoredNum(bytes, 10, 3));
    mod->setSavedValue<uint8_t>("opt-0-speed-changes", readStoredNum(bytes, 13, 6));
    mod->setSavedValue<bool>(
        "opt-0-widen-playfield-bounds",
        static_cast<bool>(readStoredNum(bytes, 43, 1))
    );
    mod->setSavedValue<uint32_t>("opt-0-corridor-height", readStoredNum(bytes, 44, 8));
    mod->setSavedValue<uint8_t>("opt-0-corridor-rules", readStoredNum(bytes, 64, 4));
    bool usingPortals = static_cast<bool>(readStoredNum(bytes, 68, 3));
    mod->setSavedValue<bool>("opt-0-using-grav-portals", usingPortals);
    if (usingPortals)
        mod->setSavedValue<uint8_t>("opt-0-grav-portals-diff", readStoredNum(bytes, 68, 3) - 1);
    mod->setSavedValue<bool>(
        "opt-0-fake-grav-portals",
        static_cast<bool>(readStoredNum(bytes, 71, 1))
    );
    mod->setSavedValue<bool>(
        "opt-0-grav-portal-start",
        static_cast<bool>(readStoredNum(bytes, 72, 2))
    );
    mod->setSavedValue<uint8_t>("opt-0-portal-input-types", readStoredNum(bytes, 74, 3));
    mod->setSavedValue<bool>(
        "opt-0-add-corner-pieces",
        static_cast<bool>(readStoredNum(bytes, 77, 1))
    );
    mod->setSavedValue<bool>(
        "opt-0-low-visibility",
        static_cast<bool>(readStoredNum(bytes, 78, 2))
    );
    mod->setSavedValue<uint8_t>("opt-0-color-mode", readStoredNum(bytes, 80, 3));
    bool usingSpikes = static_cast<bool>(readStoredNum(bytes, 83, 2));
    mod->setSavedValue<bool>("opt-0-using-corridor-spikes", usingSpikes);
    if (usingSpikes)
        mod->setSavedValue<uint8_t>("opt-0-spike-placement-types", readStoredNum(bytes, 83, 2) - 1);
    mod->setSavedValue<bool>(
        "opt-0-corridor-fuzz",
        static_cast<bool>(readStoredNum(bytes, 85, 2))
    );
    bool usingSpeed = static_cast<bool>(readStoredNum(bytes, 87, 3));
    mod->setSavedValue<bool>("opt-0-using-speed-changes", usingSpeed);
    if (usingSpeed)
        mod->setSavedValue<uint8_t>("opt-0-speed-changes-diff", readStoredNum(bytes, 87, 3) - 1);
    mod->setSavedValue<uint8_t>("opt-0-starting-size", readStoredNum(bytes, 90, 2));
    mod->setSavedValue<bool>(
        "opt-0-using-size-changes",
        static_cast<bool>(readStoredNum(bytes, 92, 3))
    );
    mod->setSavedValue<uint8_t>("opt-0-size-transition-type", readStoredNum(bytes, 95, 2));
    mod->setSavedValue<bool>(
        "opt-0-remove-portals-in-spams",
        static_cast<bool>(readStoredNum(bytes, 97, 2))
    );
}

int readStoredNum(std::vector<uint8_t>& bytes, int offset, int size) {
    int res = 0;
    for (int i = 0; i < size; i++) {
        int bitIndex = offset + i;
        int byteIndex = bitIndex / 8;
        int bitInByte = 7 - (bitIndex % 8);
        if (byteIndex < bytes.size()) {
            if (bytes[byteIndex] & (1 << bitInByte)) {
                res |= (1 << (size - i - 1));
            }
        }
    }
    return res;
}

void writeStoredNum(std::vector<uint8_t>& bytes, int offset, int size, int value) {
    for (int i = 0; i < size; i++) {
        int bitIndex = offset + i;
        int byteIndex = bitIndex / 8;
        int bitInByte = 7 - (bitIndex % 8);
        if (byteIndex < bytes.size()) {
            if (value & (1 << (size - i - 1))) {
                bytes[byteIndex] |= (1 << bitInByte);
            } else {
                bytes[byteIndex] &= ~(1 << bitInByte);
            }
        }
    }
}

std::vector<PackedEntry> getSettings(JFPGen::JFPBiome biome) {
    std::vector<PackedEntry> resSettings;
    auto* mod = Mod::get();

    if (biome != JFPGen::JFPBiome::Juggernaut) return resSettings;

    resSettings.push_back(PackedEntry{
        6, 38
    });
    resSettings.push_back(PackedEntry{
        4, static_cast<uint32_t>(biome)
    });
    resSettings.push_back(PackedEntry{
        3, mod->getSavedValue<uint32_t>("opt-0-starting-speed")
    });
    resSettings.push_back(PackedEntry{
        6, mod->getSavedValue<uint32_t>("opt-0-speed-changes")
    });
    resSettings.push_back(PackedEntry{
        12, 255
    });
    resSettings.push_back(PackedEntry{
        12, 45
    });
    resSettings.push_back(PackedEntry{
        1, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-widen-playfield-bounds"))
    });
    resSettings.push_back(PackedEntry{
        8, mod->getSavedValue<uint32_t>("opt-0-corridor-height")
    });
    resSettings.push_back(PackedEntry{
        12, 135
    });
    resSettings.push_back(PackedEntry{
        4, mod->getSavedValue<uint32_t>("opt-0-corridor-rules")
    });
    bool usingGP = mod->getSavedValue<bool>("opt-0-using-grav-portals");
    resSettings.push_back(PackedEntry{
        3, usingGP ? mod->getSavedValue<uint32_t>("opt-0-grav-portals-diff") + 1 : 0
    });
    resSettings.push_back(PackedEntry{
        1, mod->getSavedValue<uint32_t>("opt-0-fake-grav-portals")
    });
    resSettings.push_back(PackedEntry{
        2, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-grav-portal-start"))
    });
    resSettings.push_back(PackedEntry{
        3, mod->getSavedValue<uint32_t>("opt-0-portal-input-types")
    });
    resSettings.push_back(PackedEntry{
        1, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-add-corner-pieces"))
    });
    resSettings.push_back(PackedEntry{
        2, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-low-visibility"))
    });
    resSettings.push_back(PackedEntry{
        3, mod->getSavedValue<uint32_t>("opt-0-color-mode")
    });
    bool usingSpikes = mod->getSavedValue<bool>("opt-0-using-corridor-spikes");
    resSettings.push_back(PackedEntry{
        2, usingSpikes ? mod->getSavedValue<uint32_t>("opt-0-spike-placement-types") + 1 : 0
    });
    resSettings.push_back(PackedEntry{
        2, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-corridor-fuzz"))
    });
    bool usingSpeed = mod->getSavedValue<bool>("opt-0-using-speed-changes");
    resSettings.push_back(PackedEntry{
        3, usingSpeed ? mod->getSavedValue<uint32_t>("opt-0-speed-changes-diff") + 1 : 0
    });
    resSettings.push_back(PackedEntry{
        2, mod->getSavedValue<uint32_t>("opt-0-starting-size")
    });
    resSettings.push_back(PackedEntry{
        3, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-using-size-changes"))
    });
    resSettings.push_back(PackedEntry{
        2, mod->getSavedValue<uint32_t>("opt-0-size-transition-type")
    });
    resSettings.push_back(PackedEntry{
        2, static_cast<uint32_t>(mod->getSavedValue<bool>("opt-0-remove-portals-in-spams"))
    });

    return resSettings;
}

// the way the Geode settings system works, and the specificity of what JFP
// actually saves in option strings, makes it rather extra to do this in a
// more formulaic/elegant way, at least for right now. I might revisit at
// a later date. -M
std::vector<PackedEntry> getSettingsOld(JFPGen::JFPBiome biome) {
    std::vector<PackedEntry> resSettings;
    auto* mod = Mod::get();

    if (biome == JFPGen::JFPBiome::Juggernaut) {
        uint8_t ver = 37;
        resSettings.push_back(PackedEntry{6, ver});

        uint8_t biomeType = static_cast<uint32_t>(biome);
        resSettings.push_back(PackedEntry{4, biomeType});

        auto getSpeedChange = [](const std::string& speedStr) -> int8_t {
            if (speedStr == "0.5x") return 5;
            else if (speedStr == "1x") return 1;
            else if (speedStr == "2x") return 2;
            else if (speedStr == "3x") return 3;
            else if (speedStr == "4x") return 4;
            return 0;
        };

        std::string sspeedStr = mod->getSettingValue<std::string>("starting-speed");
        std::string maxSpeedStr = mod->getSettingValue<std::string>("max-speed");
        std::string minSpeedStr = mod->getSettingValue<std::string>("min-speed");

        // starting-speed (fakeenum)
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(getSpeedChange(sspeedStr))});
        // max-speed (fakeenum)
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(getSpeedChange(maxSpeedStr))});
        // min-speed (fakeenum)
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(getSpeedChange(minSpeedStr))});

        // max-height
        resSettings.push_back(PackedEntry{12, 255});
        // min-height
        resSettings.push_back(PackedEntry{12, 45});
        // corridor-widening
        resSettings.push_back(PackedEntry{1,
            static_cast<uint32_t>(mod->getSettingValue<bool>("corridor-widening"))});
        // corridor height
        resSettings.push_back(PackedEntry{8,
            static_cast<uint32_t>(mod->getSettingValue<float>("corridor-height"))});

        // starting-height
        resSettings.push_back(PackedEntry{12, 135});
        // starting-dist

        // corridor-rules (fakeenum)
        std::string corridorRulesStr =
            mod->getSettingValue<std::string>("corridor-rules");
        int optCR = 3;
        if (corridorRulesStr == "NS") optCR = 1;
        else if (corridorRulesStr == "NSNZ") optCR = 0;
        else if (corridorRulesStr == "Juggernaut") optCR = 2;
        else if (corridorRulesStr == "LRD") optCR = 4;
        else if (corridorRulesStr == "Random") optCR = 5;
        else if (corridorRulesStr == "Limp") optCR = 6;
        resSettings.push_back(PackedEntry{4, static_cast<uint32_t>(optCR)});

        // portals (fakeenum)
        std::string portalsStr = mod->getSettingValue<std::string>("portals");
        int optPortals = 0;
        if (portalsStr == "Light") optPortals = 1;
        else if (portalsStr == "Balanced") optPortals = 2;
        else if (portalsStr == "Aggressive") optPortals = 3;
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(optPortals)});

        // fake-gravity-portals
        resSettings.push_back(PackedEntry{1,
            static_cast<uint32_t>(mod->getSettingValue<bool>("fake-gravity-portals"))});
        resSettings.push_back(PackedEntry{2,
            static_cast<uint32_t>(mod->getSettingValue<bool>("upside-start"))});
        
        // portal inputs (fakeenum)
        std::string piStr = mod->getSettingValue<std::string>("portal-inputs");
        int optPortalInputs = 0;
        if (piStr == "Releases") optPortalInputs = 1;
        else if (piStr == "Holds") optPortalInputs = 2;
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(optPortalInputs) });

        // corner-pieces
        resSettings.push_back(PackedEntry{1,
            static_cast<uint32_t>(mod->getSettingValue<bool>("corners"))});
        // low-vis
        resSettings.push_back(PackedEntry{2,
            static_cast<uint32_t>(mod->getSettingValue<bool>("low-vis"))});

        // color-mode (fakeenum)
        std::string colorModeStr = mod->getSettingValue<std::string>("color-mode");
        int optColorMode = 0;
        if (colorModeStr == "All Colors") optColorMode = 1;
        else if (colorModeStr == "Classic Mode") optColorMode = 2;
        else if (colorModeStr == "Night Mode") optColorMode = 3;
        else if (colorModeStr == "Random") optColorMode = 4;
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(optColorMode)});

        // corridor-spikes
        uint32_t optCorridorSpikes = mod->getSettingValue<uint32_t>("corridor-spikes");
        if (optCorridorSpikes) {
            std::string spikeStr = mod->getSettingValue<std::string>("spike-placement");
            if (spikeStr == "Both") optCorridorSpikes = 1;
            else if (spikeStr == "Big Wave") optCorridorSpikes = 2;
            else if (spikeStr == "Mini Wave") optCorridorSpikes = 3;
        }
        resSettings.push_back(PackedEntry{2, optCorridorSpikes});

        // fuzzy-spike

        resSettings.push_back(PackedEntry{2,
            static_cast<uint32_t>(mod->getSettingValue<bool>("fuzzy-spikes"))});

        // changing-speed (fakeenum)
        std::string cspeedStr = mod->getSettingValue<std::string>("changing-speed");
        int optChangingSpeed = 0;
        if (cspeedStr == "Light") optChangingSpeed = 1;
        else if (cspeedStr == "Balanced") optChangingSpeed = 2;
        else if (cspeedStr == "Aggressive") optChangingSpeed = 3;
        resSettings.push_back(PackedEntry{3, static_cast<uint32_t>(optChangingSpeed)});

        // starting-size
        std::string sizeStr = mod->getSettingValue<std::string>("starting-size");
        int mini = 1;
        if (sizeStr == "Big") mini = 0;
        resSettings.push_back(PackedEntry{2, static_cast<uint32_t>(mini)});
        // changing-size
        resSettings.push_back(PackedEntry{3,
            static_cast<uint8_t>(mod->getSettingValue<bool>("changing-size"))});
        // transition-type (fakeenum)
        std::string scTypeStr = mod->getSettingValue<std::string>("transition-type");
        int typeA = 1;
        if (scTypeStr == "Type B") typeA = 0;
        resSettings.push_back(PackedEntry{2, static_cast<uint32_t>(typeA)});

        // portals-in-spams
        resSettings.push_back(PackedEntry{2,
            static_cast<uint8_t>(!(mod->getSettingValue<bool>("portals-in-spams")))});
    }

    return resSettings;
}

void loadDefaults(bool fullReset) {
    auto* mod = Mod::get();

    // biome 0 (Juggernaut) default options
    mod->setSavedValue<uint8_t>("opt-0-corridor-rules", 1);
    mod->setSavedValue<uint16_t>("opt-0-corridor-height", 60);
    mod->setSavedValue<uint32_t>("opt-0-length", 400);
    mod->setSavedValue<uint8_t>("opt-0-grav-portals-diff", 1);
    mod->setSavedValue<uint8_t>("opt-0-speed-changes-diff", 1);
    mod->setSavedValue<uint8_t>("opt-0-speed-changes", 9);
    mod->setSavedValue<bool>("opt-0-show-meter-marks", true);
    mod->setSavedValue<uint32_t>("opt-0-mark-interval", 100);
    mod->setSavedValue<std::string>("opt-u-filename", "waveman");
    mod->setSavedValue<uint8_t>("opt-0-music-source", 1);
    mod->setSavedValue<uint8_t>("opt-0-color-mode", 1);
    mod->setSavedValue<uint8_t>("opt-0-starting-speed", 3);
    mod->setSavedValue<bool>("opt-0-background-texture", 1);

    // biome 0 full reload
    if (fullReset) {
        mod->setSavedValue<bool>("opt-u-waveman-button-shown", false);
        mod->setSavedValue<bool>("opt-u-theme-creator-tools", false);
        mod->setSavedValue<bool>("opt-u-debug", false);
        mod->setSavedValue<bool>("opt-0-using-set-seed", false);
        mod->setSavedValue<uint8_t>("opt-0-seed", 0);
        mod->setSavedValue<bool>("opt-0-using-speed-changes", false);
        mod->setSavedValue<bool>("opt-0-using-size-changes", false);
        mod->setSavedValue<uint8_t>("opt-0-starting-size", 0);
        mod->setSavedValue<uint8_t>("opt-0-size-transition-type", 0);
        mod->setSavedValue<bool>("opt-u-save-on-close", false);
        mod->setSavedValue<bool>("opt-0-widen-playfield-bounds", false);
        mod->setSavedValue<bool>("opt-0-low-visibility", false);
        mod->setSavedValue<bool>("opt-0-add-corner-pieces", false);
        mod->setSavedValue<bool>("opt-0-show-finish-line", false);
        mod->setSavedValue<bool>("opt-0-hide-icon", false);
        mod->setSavedValue<bool>("opt-0-using-grav-portals", false);
        mod->setSavedValue<bool>("opt-0-grav-portal-start", false);
        mod->setSavedValue<bool>("opt-0-fake-grav-portals", false);
        mod->setSavedValue<bool>("opt-0-remove-portals-in-spams", false);
        mod->setSavedValue<uint8_t>("opt-0-portal-input-types", 0);
        mod->setSavedValue<bool>("opt-0-using-corridor-spikes", false);
        mod->setSavedValue<bool>("opt-0-corridor-fuzz", false);
        mod->setSavedValue<uint8_t>("opt-0-spike-placement-types", 0);
    }

    mod->setSavedValue<bool>("ack-disclaimer", "true");
}