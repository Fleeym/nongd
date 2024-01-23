#pragma once

#include <Geode/Geode.hpp>
#include <filesystem>

#ifndef GEODE_IS_MACOS
namespace fs = std::filesystem;
#else
namespace fs = ghc::filesystem;
#endif