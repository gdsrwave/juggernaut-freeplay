#include <string>
#include <vector>
#include <Geode/Geode.hpp>
#include "Ninja.hpp"
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
        for (int b = 0; b < entry.size; ++b) {
            if (value & (1 << (entry.size - b - 1))) {
                bytes[bitPos / 8] |= (1 << (7 - (bitPos % 8)));
            }
            ++bitPos;
        }
    }
    std::string bitStr;
    for (int i = 0; i < totalBits; ++i) {
        bitStr += ((bytes[i / 8] & (1 << (7 - (i % 8)))) ? '1' : '0');
    }
    log::info("Bit string: {}", bitStr);

    static const char b64_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string res;
    int val = 0, valb = -6;
    for (int i = 0; i < totalBytes; ++i) {
        val = (val << 8) | bytes[i];
        valb += 8;
        while (valb >= 0) {
            res.push_back(b64_table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) res.push_back(b64_table[((val << 8) >> (valb + 8)) & 0x3F]);
    
    std::string lenSuffix = fmt::format("+{}", Mod::get()->getSettingValue<uint32_t>("length"));

    return res + lenSuffix;
}

void importSettings(std::string packed) {
    auto* mod = Mod::get();

    size_t ls = packed.find('+');
    log::info("{}", packed);
    if (ls == std::string::npos) {
        throw std::runtime_error("Length suffix not found");
    }
    int length = std::stoi(packed.substr(ls + 1, packed.size() - 1));
    mod->setSettingValue("length", length);

    // int totalBits = 0;
    // for (const auto& entry : entries) totalBits += entry.size;
    // int totalBytes = (totalBits + 7) / 8;
    // std::vector<uint8_t> bytes(totalBytes, 0);


}

// the way the Geode settings system works, and the specificity of what JFP
// actually saves in option strings, makes it rather extra to do this in a
// more formulaic/elegant way, at least for right now. I do plan to revisit
// at a later date. -M
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
        std::string corridorRulesStr = mod->getSettingValue<std::string>("corridor-rules");
        int optCR = 3;
        if (corridorRulesStr == "NS") optCR = 1;
        else if (corridorRulesStr == "NSNZ") optCR = 0;
        else if (corridorRulesStr == "Experimental") optCR = 2;
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
            static_cast<int>(mod->getSettingValue<bool>("corner-pieces"))));
        // low-vis
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("low-vis"))));
            
        // color-mode (fakeenum)
        std::string colorModeStr = mod->getSettingValue<std::string>("color-mode");
        int optColorMode = 0;
        if (colorModeStr == "All Colors") optColorMode = 1;
        else if (colorModeStr == "Classic Mode") optColorMode = 2;
        else if (colorModeStr == "Night Mode") optColorMode = 3;
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
        resSettings.push_back(PackedEntry(2, optChangingSpeed));

        // starting-size
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("starting-size"))));
        // changing-size
        resSettings.push_back(PackedEntry(2,
            static_cast<int>(mod->getSettingValue<bool>("changing-size"))));
        // transition-type (fakeenum)
        std::string scTypeStr = mod->getSettingValue<std::string>("transition-type");
        int typeA = 1;
        if (scTypeStr == "Type B") typeA = 0;
        resSettings.push_back(PackedEntry(2, typeA));
    }

    return resSettings;
}