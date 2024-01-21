#pragma once

#include <Geode/Geode.hpp>
#include <string>

#include "../filesystem.hpp"
#include "../manifest.hpp"

using namespace geode::prelude;

struct SongInfo {
    fs::path path;
    std::string songName;
    std::string authorName;
    std::string songUrl;
};

struct NongData {
    fs::path active;
    fs::path defaultPath;
    std::vector<SongInfo> songs;
};

template<>
struct matjson::Serialize<NongData> {
    static NongData from_json(matjson::Value const& value) {
        std::vector<SongInfo> songs;
        auto jsonSongs = value["songs"].as_array();

        for (auto jsonSong : jsonSongs) {
            auto path = fs::path(jsonSong["path"].as_string());

            SongInfo song = {
                .path = path,
                .songName = jsonSong["songName"].as_string(),
                .authorName = jsonSong["authorName"].as_string(),
                .songUrl = jsonSong["songUrl"].as_string(),
            };
            songs.push_back(song);
        }

        return NongData {
            .active = fs::path(value["active"].as_string()),
            .defaultPath = fs::path(value["defaultPath"].as_string()),
            .songs = songs,
        };
    }

    static matjson::Value to_json(NongData const& value) {
        auto ret = matjson::Object();
        auto array = matjson::Array();
        ret["active"] = value.active.string();
        ret["defaultPath"] = value.defaultPath.string();
        for (auto song : value.songs) {
            auto obj = matjson::Object();
            obj["path"] = song.path.string();
            obj["songName"] = song.songName;
            obj["authorName"] = song.authorName;
            obj["songUrl"] = song.songUrl;

            array.push_back(obj);
        }

        ret["songs"] = array;
        return ret;
    }
};