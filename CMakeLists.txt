cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(jukebox VERSION 2.0.0)

file(GLOB SOURCES
    src/ui/*.cpp
    src/managers/*.cpp
    src/hooks/*.cpp
	src/*.cpp
)

if (${NONGD_BUILD_ANDROID})
    if (NOT DEFINED ENV{NDK})
        message(FATAL_ERROR "Unable to find NDK. Make sure it is defined in your path")
    else()
        message(STATUS "Found Android NDK: $ENV{NDK}")
    endif()
    set(CMAKE_TOOLCHAIN_FILE $ENV{NDK}/build/cmake/android.toolchain.cmake)
    set(ANDROID_ABI "arm64-v8a")
    set(ANDROID_PLATFORM "android-24")
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
    set(GEODE_DONT_INSTALL_MODS 1)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND WIN32 AND NOT DEFINED ${NONGD_BUILD_ANDROID})
    message(STATUS "Setting Clang to 32-bit")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

target_link_libraries(${PROJECT_NAME} geode-sdk)
create_geode_file(${PROJECT_NAME})
