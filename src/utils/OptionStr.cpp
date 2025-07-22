// Copyright 2025 GDSRWave
#include <string>
#include <vector>
#include <Geode/Geode.hpp>
#include "./Ninja.hpp"
#include "OptionStr.hpp"

using namespace geode::prelude;

std::string exportSettings(const std::vector<PackedEntry>& entries) {
    int totalBits = 0;
    for (const auto& entry : entries) totalBits += entry.size;
    int totalBytes = (totalBits + 7) / 8;
    std::vector<uint8_t> bytes(totalBytes, 0);

    int bitPos = 0;
    for (const auto& entry : entries) {
        uint32_t value = entry.value;
        for (int b = 0; b < entry.size; b++) {
            if (value & (1 << (entry.size - b - 1))) {
                bytes[bitPos / 8] |= (1 << (7 - (bitPos % 8)));
            }
            bitPos++;
        }
    }

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
        fmt::format("+{}", Mod::get()->getSettingValue<uint32_t>("length"));

    return res + lenSuffix;
}

void importSettings(std::string packed) {
    auto* mod = Mod::get();

    size_t ls = packed.find('+');
    if (ls == std::string::npos) {
        throw std::runtime_error("Length suffix not found");
    }
    int length = std::stoi(packed.substr(ls + 1, std::string::npos));
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

        std::array<std::string, 6> corridorRules = {
            "NSNZ",
            "NS",
            "Experimental",
            "Unrestricted",
            "LRD",
            "Random"
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
        mod->setSettingValue<bool>("portal-inputs",
            static_cast<bool>(readStoredNum(bytes, 74, 3)));
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

        mod->setSettingValue<bool>("corridor-spikes",
            static_cast<bool>(readStoredNum(bytes, 83, 2)));
        mod->setSettingValue<bool>("fuzzy-spikes",
            static_cast<bool>(readStoredNum(bytes, 85, 2)));

        size_t sc_i = readStoredNum(bytes, 87, 3);
        std::string scstr = (sc_i >= 0 && sc_i < diffs.size()) ? diffs[sc_i] : "None";
        mod->setSettingValue<std::string>("changing-speed", scstr);

        std::string sizeStr = readStoredNum(bytes, 90, 3) ? "Mini" : "Big";
        mod->setSettingValue<std::string>("starting-size", sizeStr);

        mod->setSettingValue<bool>("changing-size",
            static_cast<bool>(readStoredNum(bytes, 93, 3)));

        std::string ttStr = readStoredNum(bytes, 96, 2) ? "Type A" : "Type B";
        mod->setSettingValue<std::string>("transition-type", ttStr);
    }
}

int readStoredNum(const std::vector<uint8_t>& bytes, int offset, int size) {
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

// the way the Geode settings system works, and the specificity of what JFP
// actually saves in option strings, makes it rather extra to do this in a
// more formulaic/elegant way, at least for right now. I might revisit at
// a later date. -M
std::vector<PackedEntry> getSettings(JFPGen::JFPBiome biome) {
    std::vector<PackedEntry> resSettings;
    auto* mod = Mod::get();

    if (biome == JFPGen::JFPBiome::Juggernaut) {
        uint8_t ver = 37;
        resSettings.push_back(PackedEntry(6, ver));

        uint8_t biomeType = static_cast<int>(biome);
        resSettings.push_back(PackedEntry(4, biomeType));

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
        resSettings.push_back(PackedEntry(3, getSpeedChange(sspeedStr)));
        // max-speed (fakeenum)
        resSettings.push_back(PackedEntry(3, getSpeedChange(maxSpeedStr)));
        // min-speed (fakeenum)
        resSettings.push_back(PackedEntry(3, getSpeedChange(minSpeedStr)));

        // max-height
        resSettings.push_back(PackedEntry(12, 255));
        // min-height
        resSettings.push_back(PackedEntry(12, 45));
        // corridor-widening
        resSettings.push_back(PackedEntry(1,
            static_cast<int>(mod->getSettingValue<bool>("corridor-widening"))));
        // corridor height
        resSettings.push_back(PackedEntry(8,
            static_cast<int>(mod->getSettingValue<float>("corridor-height"))));

        // starting-height
        resSettings.push_back(PackedEntry(12, 135));
        // starting-dist

        // corridor-rules (fakeenum)
        std::string corridorRulesStr =
            mod->getSettingValue<std::string>("corridor-rules");
        int optCR = 3;
        if (corridorRulesStr == "NS") optCR = 1;
        else if (corridorRulesStr == "NSNZ") optCR = 0;
        else if (corridorRulesStr == "Experimental") optCR = 2;
        else if (corridorRulesStr == "LRD") optCR = 4;
        else if (corridorRulesStr == "Random") optCR = 5;
        resSettings.push_back(PackedEntry(4, optCR));

        // portals (fakeenum)
        std::string portalsStr = mod->getSettingValue<std::string>("portals");
        int optPortals = 0;
        if (portalsStr == "Light") optPortals = 1;
        else if (portalsStr == "Balanced") optPortals = 2;
        else if (portalsStr == "Aggressive") optPortals = 3;
        resSettings.push_back(PackedEntry(3, optPortals));

        // fake-gravity-portals
        resSettings.push_back(PackedEntry(1,
            static_cast<int>(mod->getSettingValue<bool>("fake-gravity-portals"))));
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("upside-start"))));
        resSettings.push_back(PackedEntry(3,
            static_cast<int>(mod->getSettingValue<bool>("portal-inputs"))));

        // corner-pieces
        resSettings.push_back(PackedEntry(1,
            static_cast<int>(mod->getSettingValue<bool>("corners"))));
        // low-vis
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("low-vis"))));

        // color-mode (fakeenum)
        std::string colorModeStr = mod->getSettingValue<std::string>("color-mode");
        int optColorMode = 0;
        if (colorModeStr == "All Colors") optColorMode = 1;
        else if (colorModeStr == "Classic Mode") optColorMode = 2;
        else if (colorModeStr == "Night Mode") optColorMode = 3;
        else if (colorModeStr == "Random") optColorMode = 4;
        resSettings.push_back(PackedEntry(3, optColorMode));

        // corridor-spikes
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("corridor-spikes"))));

        // fuzzy-spikes
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("fuzzy-spikes"))));

        // changing-speed (fakeenum)
        std::string cspeedStr = mod->getSettingValue<std::string>("changing-speed");
        int optChangingSpeed = 0;
        if (cspeedStr == "Light") optChangingSpeed = 1;
        else if (cspeedStr == "Balanced") optChangingSpeed = 2;
        else if (cspeedStr == "Aggressive") optChangingSpeed = 3;
        resSettings.push_back(PackedEntry(3, optChangingSpeed));

        // starting-size
        std::string sizeStr = mod->getSettingValue<std::string>("starting-size");
        int mini = 1;
        if (sizeStr == "Big") mini = 0;
        resSettings.push_back(PackedEntry(2, mini));
        // changing-size
        resSettings.push_back(PackedEntry(3,
            static_cast<int>(mod->getSettingValue<bool>("changing-size"))));
        // transition-type (fakeenum)
        std::string scTypeStr = mod->getSettingValue<std::string>("transition-type");
        int typeA = 1;
        if (scTypeStr == "Type B") typeA = 0;
        resSettings.push_back(PackedEntry(2, typeA));
    }

    return resSettings;
}
