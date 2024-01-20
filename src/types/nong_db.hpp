#pragma once
#include <cstdint>
#include <map>

#include "song_info.hpp"

struct NongDB {
    uint32_t m_manifestVersion;
    std::map<uint32_t, SongInfo> m_nongs;
};