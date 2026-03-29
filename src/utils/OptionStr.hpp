// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <vector>
#include "./Ninja.hpp"

struct BitField {
    uint64_t seg1;
    uint64_t seg2;

    BitField() : seg1(0), seg2(0) {}
};

struct ConfigEntry {
    uint32_t offset;
    uint32_t size;
    std::string optionname;
    std::string type;
};

struct PackedEntry {
    uint32_t sizeSize;
    uint32_t valueVal;
};

struct OptionString {
    std::string name;
    std::string optExported;
    bool isPreset = false;
};

template<>
struct matjson::Serialize<OptionString> {
    static OptionString from_json(matjson::Value const& value) {
        return OptionString{
            .name = value["name"].asString().unwrapOr("Invalid Preset"),
            .optExported = value["optExported"].asString().unwrapOr(""),
        };
    }

    static matjson::Value to_json(OptionString const& value) {
        return matjson::makeObject({
            {"name", value.name},
            {"optExported", value.optExported}
        });
    }
};

extern const std::vector<OptionString> defaultOptBank;

std::string exportSettings(const std::vector<PackedEntry>& entries);
void importSettingsOld(std::string packed);
void importSettings(std::string packed);
void v37tov38(std::vector<uint8_t>& bytes);
int readStoredNum(std::vector<uint8_t>& bytes, int offset, int size);
void writeStoredNum(std::vector<uint8_t>& bytes, int offset, int size, int value);
std::vector<PackedEntry> getSettings(JFPGen::JFPBiome biome);
std::vector<PackedEntry> getSettingsOld(JFPGen::JFPBiome biome);
void loadDefaults(bool fullReset = false);