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

struct SavedOptionStrs {
    std::vector<OptionString> juggernaut;
};

template<>
struct matjson::Serialize<OptionString> {
    static geode::Result<OptionString> fromJson(matjson::Value const& value) {
        return geode::Ok(OptionString{
            .name = value["name"].asString().unwrapOr("Invalid Preset"),
            .optExported = value["optExported"].asString().unwrapOr(""),
        });
    }

    static matjson::Value toJson(OptionString const& value) {
        return matjson::makeObject({
            {"name", value.name},
            {"optExported", value.optExported}
        });
    }
};

template<>
struct matjson::Serialize<SavedOptionStrs> {
    static geode::Result<SavedOptionStrs> fromJson(matjson::Value const& value) {
        auto arrayb0 = value["juggernaut"].asArray().unwrapOr(std::vector<matjson::Value>{});
        std::vector<OptionString> vectorb0;
        for (auto const& item : arrayb0) {
            vectorb0.push_back(item.as<OptionString>().unwrapOr(OptionString{
                .name = "Invalid Preset",
                .optExported = "",
            }));
        }
        return geode::Ok(SavedOptionStrs{
            .juggernaut = vectorb0,
        });
    }

    static matjson::Value toJson(SavedOptionStrs const& value) {
        auto arrayb0 = matjson::Value::array();
        for (auto const& item : value.juggernaut) {
            arrayb0.push(matjson::Serialize<OptionString>::toJson(item));
        }
        return matjson::makeObject({
            {"juggernaut", arrayb0}
        });
    }
};

extern const std::vector<OptionString> defaultOptBank;

std::string exportSettings(const std::vector<PackedEntry>& entries);
void importSettingsOld(std::string packed);
void importSettings(std::string packed, bool toast = false);
void v37tov38(std::vector<uint8_t>& bytes);
int readStoredNum(std::vector<uint8_t>& bytes, int offset, int size);
void writeStoredNum(std::vector<uint8_t>& bytes, int offset, int size, int value);
std::vector<PackedEntry> getSettings(JFPGen::JFPBiome biome);
std::vector<PackedEntry> getSettingsOld(JFPGen::JFPBiome biome);
void loadDefaults(bool fullReset = false);