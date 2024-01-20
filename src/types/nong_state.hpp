#pragma once

#include <cstdint>
#include <map>

#include "song_info.hpp"

struct NongState {
    uint32_t m_manifestVersion;
    std::map<uint32_t, NongData> m_nongs;
};