// Copyright 2025 GDSRWave
#include <Geode/Geode.hpp>
#include "utils/shared.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    setupJFPDirectories();
    setupJFPMusic();
}
