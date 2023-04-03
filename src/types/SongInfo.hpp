#pragma once

#include <string>

struct SongInfo {
    ghc::filesystem::path path;
    std::string songName;
    std::string authorName;
    std::string songUrl;
};

struct NongData {
    ghc::filesystem::path active;
    ghc::filesystem::path defaultPath;
    std::vector<SongInfo> songs;
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
            };
            songs.push_back(song);
        }

        return NongData {
            .active = ghc::filesystem::path(value["active"].as_string()),
            .defaultPath = ghc::filesystem::path(value["defaultPath"].as_string()),
            .songs = songs
        };
    }

    static json::Value to_json(NongData const& value) {
        auto ret = json::Object();
        auto array = json::Array();
        ret["active"] = value.active.string();
        ret["defaultPath"] = value.defaultPath.string();
        for (auto song : value.songs) {
            auto obj = json::Object();
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