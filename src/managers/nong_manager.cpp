#include "nong_manager.hpp"


NongData NongManager::getNongs(int songID) {
    auto path = this->getJsonPath(songID);

    std::ifstream input(path.string());
    std::stringstream buffer;
    buffer << input.rdbuf();
    input.close();

    auto json = matjson::parse(std::string_view(buffer.str()));
    return matjson::Serialize<NongData>::from_json(json);
}

SongInfo NongManager::getActiveNong(int songID) {
    auto nongs = this->getNongs(songID);

    for (auto &song : nongs.songs) {
        if (song.path == nongs.active) {
            return song;
        }
    }
    
    nongs.active = nongs.defaultPath;

    for (auto &song : nongs.songs) {
        if (song.path == nongs.active) {
            return song;
        }
    }

    throw std::runtime_error("If you somehow reached this, good job.");
}

std::vector<SongInfo> NongManager::validateNongs(int songID) {
    auto currentData = this->getNongs(songID);
    // Validate nong paths and delete those that don't exist anymore
    std::vector<SongInfo> invalidSongs;
    std::vector<SongInfo> validSongs;

    for (auto &song : currentData.songs) {
        if (!ghc::filesystem::exists(song.path) && currentData.defaultPath != song.path && song.songUrl == "local") {
            invalidSongs.push_back(song);
            if (song.path == currentData.active) {
                currentData.active = currentData.defaultPath;
            }
        } else {
            validSongs.push_back(song);
        }
    }

    if (invalidSongs.size() > 0) {
        NongData newData = {
            .active = currentData.active,
            .defaultPath = currentData.defaultPath,
            .songs = validSongs,
        };

        this->saveNongs(newData, songID);
    }

    return invalidSongs;
}

void NongManager::saveNongs(NongData const& data, int songID) {
    auto path = this->getJsonPath(songID);
    auto json = matjson::Serialize<NongData>::to_json(data);
    std::ofstream output(path.string());
    output << json.dump();
    output.close();
}

void NongManager::addNong(SongInfo const& song, int songID) {
    auto existingData = this->getNongs(songID);
    for (auto savedSong : existingData.songs) {
        if (song.path.string() == savedSong.path.string()) {
            return;
        }
    }
    existingData.songs.push_back(song);
    saveNongs(existingData, songID);
}

NongData NongManager::deleteAll(int songID) {
    std::vector<SongInfo> newSongs;
    auto existingData = this->getNongs(songID);

    for (auto savedSong : existingData.songs) {
        if (savedSong.path != existingData.defaultPath) {
            if (ghc::filesystem::exists(savedSong.path)) {
                ghc::filesystem::remove(savedSong.path);
            }
            continue;
        }
        newSongs.push_back(savedSong);
    }

    NongData newData = {
        .active = existingData.defaultPath,
        .defaultPath = existingData.defaultPath,
        .songs = newSongs,
    };
    this->saveNongs(newData, songID);
    return newData;
}

void NongManager::deleteNong(SongInfo const& song, int songID) {
    std::vector<SongInfo> newSongs;
    auto existingData = this->getNongs(songID);
    for (auto savedSong : existingData.songs) {
        if (savedSong.path == song.path) {
            if (song.path == existingData.active) {
                existingData.active = existingData.defaultPath;
            }
            if (song.songUrl != "local" && existingData.defaultPath != song.path && ghc::filesystem::exists(song.path)) {
                ghc::filesystem::remove(song.path);
            }
            continue;
        }
        newSongs.push_back(savedSong);
    }
    NongData newData = {
        .active = existingData.active,
        .defaultPath = existingData.defaultPath,
        .songs = newSongs,
    };
    this->saveNongs(newData, songID);
}

void NongManager::createDefaultSongIfNull(SongInfo const& song, int songID) {
    if (!this->checkIfNongsExist(songID)) {
        std::vector<SongInfo> songs = { song };
        NongData nongData = {
            .active = song.path,
            .defaultPath = song.path,
            .songs = songs,
        };
        this->saveNongs(nongData, songID);
    }
}

std::string NongManager::getFormattedSize(SongInfo const& song) {
    try {
        auto size = ghc::filesystem::file_size(song.path);
        double toMegabytes = size / 1024.f / 1024.f;
        std::stringstream ss;
        ss << std::setprecision(3) << toMegabytes << "MB";
        return ss.str();
    } catch (ghc::filesystem::filesystem_error) {
        return "N/A";
    }
}

ghc::filesystem::path NongManager::getJsonPath(int songID) {
    auto filename = std::to_string(songID) + ".json";
    auto nong_data_path = Mod::get()->getSaveDir() / "nong_data";
    if (!ghc::filesystem::exists(nong_data_path)) {
        ghc::filesystem::create_directory(nong_data_path);
    }
    nong_data_path.append(filename);
    return nong_data_path;
}

bool NongManager::checkIfNongsExist(int songID) {
    auto nongData = Mod::get()->getSaveDir().append("nong_data");
    if (!ghc::filesystem::exists(nongData)) {
        ghc::filesystem::create_directory(nongData);
    }
    auto path = this->getJsonPath(songID);
    if (!ghc::filesystem::exists(path)) return false;

    auto data = this->getNongs(songID);

    if (data.songs.size() == 1) return false;
    return true;
}

void NongManager::fetchSFH(int songID, std::function<void(nongd::FetchStatus)> callback) {
    std::string url = "https://api.songfilehub.com/songs?songID=" + std::to_string(songID);
    web::AsyncWebRequest()
        .fetch(url)
        .text()
        .then([this, callback, songID](std::string const& text) {
            auto copy = text;
            copy.erase(std::remove_if(copy.begin(), copy.end(), [](char x) {
                return static_cast<int>(x) < 0;
            }), copy.end());
            matjson::Value data;
            data = matjson::parse(copy);
            std::vector<SFHItem> ret;
            if (!data.is_array()) {
                callback(nongd::FetchStatus::FAILED);
                return;
            }
            auto songs = data.as_array();
            bool getMashups = Mod::get()->getSettingValue<bool>("store-mashups");
            for (auto const& song : songs) {
                bool isMashup = song["state"].as_string() == "mashup";
                if (isMashup && !getMashups) {
                    continue;
                }
                SFHItem item = {
                    .songName = song["songName"].as_string(),
                    .downloadUrl = song["downloadUrl"].as_string(),
                    .levelName = song.contains("name") ? song["name"].as_string() : "",
                    .songURL = song.contains("songURL") ? song["songURL"].as_string() : ""
                };
                ret.push_back(item);
            }
            if (ret.size() == 0) {
                callback(nongd::FetchStatus::NOTHING_FOUND);
                return;
            }
            this->addNongsFromSFH(ret, songID);
            callback(nongd::FetchStatus::SUCCESS);
        })
        .expect([callback](std::string const& error) {
            log::error("{}", error);
            callback(nongd::FetchStatus::FAILED);
        })
        .cancelled([callback](auto r) {
            callback(nongd::FetchStatus::FAILED);
        });
}

void NongManager::downloadSong(SongInfo const& song, std::function<void(double percentage)> progress, std::function<void(SongInfo const& song, std::string const& error)> failed) {
    if (ghc::filesystem::exists(song.path)) {
        return;
    }
    web::AsyncWebRequest()
        .fetch(song.songUrl)
        .into(song.path)
        .then([progress](std::monostate state){
            progress(100.f);
        })
        .expect([song, failed](std::string const& error) {
            failed(song, error);
        })
        .cancelled([song, failed](auto request) {
            failed(song, "The download was cancelled");
        });
}

void NongManager::addNongsFromSFH(std::vector<SFHItem> const& songs, int songID) {
    auto nongsPath = Mod::get()->getSaveDir().append("nongs");
    if (!ghc::filesystem::exists(nongsPath)) {
        ghc::filesystem::create_directory(nongsPath);
    }
    auto nongs = this->getNongs(songID);
    int index = 1;
    for (auto const& sfhSong : songs) {
        bool shouldSkip = false;
        auto path = nongsPath;
        auto unique = nongd::random_string(16);
        path.append(std::to_string(songID) + "_" + sfhSong.levelName + "_" + unique + ".mp3");
        for (auto& localSong : nongs.songs) {
            if (localSong.songUrl == sfhSong.downloadUrl) {
                shouldSkip = true;
                break;
            }
        }
        if (shouldSkip) {
            continue;
        }

        auto sfhSongName = sfhSong.songName;
        nongd::trim(sfhSongName);

        if (sfhSongName.find_first_of("-") == std::string::npos) {
            // probably dash doesn't exist because of unicode filtering, try to insert it by hand
            for (size_t i = 0; i < sfhSongName.size() - 1; i++) {
                if (sfhSongName.at(i) == ' ' && sfhSongName.at(i + 1) == ' ') {
                    sfhSongName.insert(i + 1, "-");
                }
            }
        }

        std::string songName = "-";
        std::string artistName = "-";
        std::stringstream ss;
        ss << sfhSongName;
        std::string part;
        size_t i = 0;
        while (std::getline(ss, part, '-')) {
            if (i == 0) {
                artistName = part;
                nongd::right_trim(artistName);
            } else {
                songName = part;
                nongd::left_trim(songName);
            }
            i++;
        }

        if (songName == "-") {
            auto temp = songName;
            songName = artistName;
            artistName = temp;
        }

        SongInfo song = {
            .path = path,
            .songName = songName,
            .authorName = artistName,
            .songUrl = sfhSong.downloadUrl
        };

        if (sfhSong.levelName != "") {
            song.songName += " (" + sfhSong.levelName + ")";
        }
        nongs.songs.push_back(song);
    }

    this->saveNongs(nongs, songID);
}

bool NongManager::loadSongs() {
    return true;
}