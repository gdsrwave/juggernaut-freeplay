// Copyright 2025 GDSRWave
#pragma once

#include <string>
#include <vector>

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

    PackedEntry(uint32_t sizeSize, uint32_t valueVal);
};

std::string exportSettings(const std::vector<PackedEntry>& entries);
void importSettings(std::string packed);
int readStoredNum(const std::vector<uint8_t>& bytes, int offset, int size);
std::vector<PackedEntry> getSettings(JFPGen::JFPBiome biome);
