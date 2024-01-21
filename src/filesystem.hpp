#pragma once

#include <Geode/Geode.hpp>
#include <filesystem>

#ifdef GEODE_IS_WINDOWS
namespace fs = std::filesystem;
#else
namespace fs = ghc::filesystem;
#endif