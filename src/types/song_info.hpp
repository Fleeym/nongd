#pragma once

#include <Geode/Geode.hpp>
#include <string>

#include "../manifest.hpp"

using namespace geode::prelude;

struct SongInfo {
    ghc::filesystem::path path;
    std::string songName;
    std::string authorName;
    std::string songUrl;
    std::string levelName;
};

struct NongData {
    ghc::filesystem::path active;
    ghc::filesystem::path defaultPath;
    std::vector<SongInfo> songs;
    int version;
};

template<>
struct json::Serialize<NongData> {
    static NongData from_json(json::Value const& value) {
        std::vector<SongInfo> songs;
        auto jsonSongs = value["songs"].as_array();

        for (auto jsonSong : jsonSongs) {
            auto path = ghc::filesystem::path(jsonSong["path"].as_string());

            SongInfo song = {
                .path = path,
                .songName = jsonSong["songName"].as_string(),
                .authorName = jsonSong["authorName"].as_string(),
                .songUrl = jsonSong["songUrl"].as_string(),
                .levelName = ""
            };
            if (jsonSong.contains("levelName")) {
                song.levelName = jsonSong["levelName"].as_string();
            }
            songs.push_back(song);
        }

        int version;

        if (value.contains("version")) {
            version = value["version"].as_int();
        } else {
            version = nongd::get_manifest_version();
        }

        return NongData {
            .active = ghc::filesystem::path(value["active"].as_string()),
            .defaultPath = ghc::filesystem::path(value["defaultPath"].as_string()),
            .songs = songs,
            .version = version
        };
    }

    static json::Value to_json(NongData const& value) {
        auto ret = json::Object();
        auto array = json::Array();
        ret["version"] = value.version;
        ret["active"] = value.active.string();
        ret["defaultPath"] = value.defaultPath.string();
        for (auto song : value.songs) {
            auto obj = json::Object();
            obj["path"] = song.path.string();
            obj["songName"] = song.songName;
            obj["authorName"] = song.authorName;
            obj["songUrl"] = song.songUrl;
            if (song.levelName != "") {
                obj["levelName"] = song.levelName;
            }

            array.push_back(obj);
        }

        ret["songs"] = array;
        return ret;
    }
};