#include "nong_manager.hpp"

std::optional<NongData> NongManager::getNongs(int songID) {
    if (!m_state.m_nongs.contains(songID)) {
        return std::nullopt;
    }

    return m_state.m_nongs[songID];
}

std::optional<SongInfo> NongManager::getActiveNong(int songID) {
    auto nongs_res = this->getNongs(songID);
    if (!nongs_res.has_value()) {
        return std::nullopt;
    }
    auto nongs = nongs_res.value();

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

    return std::nullopt;
}

std::optional<SongInfo> NongManager::getDefaultNong(int songID) {
    auto nongs_res = this->getNongs(songID);
    if (!nongs_res.has_value()) {
        return std::nullopt;
    }
    auto nongs = nongs_res.value();

    for (auto &song : nongs.songs) {
        if (song.path == nongs.defaultPath) {
            return song;
        }
    }
    
    return std::nullopt;
}

void NongManager::resolveSongInfoCallback(int id) {
    if (m_getSongInfoCallbacks.contains(id)) {
        m_getSongInfoCallbacks[id](id);
        m_getSongInfoCallbacks.erase(id);
    }
}

std::vector<SongInfo> NongManager::validateNongs(int songID) {
    auto result = this->getNongs(songID);
    // Validate nong paths and delete those that don't exist anymore
    std::vector<SongInfo> invalidSongs;
    std::vector<SongInfo> validSongs;
    if (!result.has_value()) {
        return invalidSongs;
    }
    auto currentData = result.value();

    for (auto &song : currentData.songs) {
        if (!fs::exists(song.path) && currentData.defaultPath != song.path && song.songUrl == "local") {
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
    m_state.m_nongs[songID] = data;
    this->writeJson();
}

void NongManager::writeJson() {
    auto json = matjson::Serialize<NongState>::to_json(m_state);
    auto path = this->getJsonPath();
    std::ofstream output(path.string());
    output << json.dump(matjson::NO_INDENTATION);
    output.close();
}

void NongManager::addNong(SongInfo const& song, int songID) {
    auto result = this->getNongs(songID);
    if (!result.has_value()) {
        return;
    }
    auto existingData = result.value();

    for (auto const& savedSong : existingData.songs) {
        if (song.path.string() == savedSong.path.string()) {
            return;
        }
    }
    existingData.songs.push_back(song);
    this->saveNongs(existingData, songID);
}

void NongManager::deleteAll(int songID) {
    std::vector<SongInfo> newSongs;
    auto result = this->getNongs(songID);
    if (!result.has_value()) {
        return;
    }
    auto existingData = result.value();

    for (auto savedSong : existingData.songs) {
        if (savedSong.path != existingData.defaultPath) {
            if (fs::exists(savedSong.path)) {
                fs::remove(savedSong.path);
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
}

void NongManager::deleteNong(SongInfo const& song, int songID) {
    std::vector<SongInfo> newSongs;
    auto result = this->getNongs(songID);
    if(!result.has_value()) {
        return;
    }
    auto existingData = result.value();
    for (auto savedSong : existingData.songs) {
        if (savedSong.path == song.path) {
            if (song.path == existingData.active) {
                existingData.active = existingData.defaultPath;
            }
            if (song.songUrl != "local" && existingData.defaultPath != song.path && fs::exists(song.path)) {
                fs::remove(song.path);
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

void NongManager::createDefault(int songID, bool fromCallback) {
    if (m_state.m_nongs.contains(songID)) {
        return;
    }
    SongInfoObject* songInfo = MusicDownloadManager::sharedState()->getSongInfoObject(songID);
    if (songInfo == nullptr && !m_getSongInfoCallbacks.contains(songID) && !fromCallback) {
        MusicDownloadManager::sharedState()->getSongInfo(songID, true);
        m_getSongInfoCallbacks[songID] = [this](int songID) {
            this->createDefault(songID, true);
        };
        return;
    }
    if (songInfo == nullptr) {
        return;
    }
    fs::path songPath = fs::path(std::string(MusicDownloadManager::sharedState()->pathForSong(songID)));
    NongData data;
    SongInfo defaultSong;
    defaultSong.authorName = songInfo->m_artistName;
    defaultSong.songName = songInfo->m_songName;
    defaultSong.path = songPath;
    defaultSong.songUrl = songInfo->m_songUrl;
    data.active = songPath;
    data.defaultPath = songPath;
    data.songs.push_back(defaultSong);
    m_state.m_nongs[songID] = data;
}

void NongManager::createUnknownDefault(int songID) {
    if (this->getNongs(songID).has_value()) {
        return;
    }
    fs::path songPath = fs::path(std::string(MusicDownloadManager::sharedState()->pathForSong(songID)));
    NongData data;
    SongInfo defaultSong;
    defaultSong.authorName = "Unknown";
    defaultSong.songName = "Unknown";
    defaultSong.path = songPath;
    defaultSong.songUrl = "";
    data.active = songPath;
    data.defaultPath = songPath;
    data.songs.push_back(defaultSong);
    data.defaultValid = false;
    m_state.m_nongs[songID] = data;
    this->writeJson();
}

std::string NongManager::getFormattedSize(SongInfo const& song) {
    try {
        auto size = fs::file_size(song.path);
        double toMegabytes = size / 1024.f / 1024.f;
        std::stringstream ss;
        ss << std::setprecision(3) << toMegabytes << "MB";
        return ss.str();
    } catch (fs::filesystem_error) {
        return "N/A";
    }
}

void NongManager::getMultiAssetSizes(std::string songs, std::string sfx, std::function<void(std::string)> callback) {
    std::thread([this, songs, sfx, callback]() {
        float sum = 0.f;
        std::istringstream stream(songs);
        std::string s;
        while (std::getline(stream, s, ',')) {
            int id = std::stoi(s);
            auto path = fs::path(std::string(MusicDownloadManager::sharedState()->pathForSong(id)));
            if (path.string().starts_with("songs/")) {
                auto gdFolder = fs::path(std::string(CCFileUtils::get()->getWritablePath2())) / "Resources";
                gdFolder = gdFolder / path.string();
                path = gdFolder;
            }
            if (fs::exists(path)) {
                sum += fs::file_size(path);
            }
        }
        stream = std::istringstream(sfx);
        while (std::getline(stream, s, ',')) {
            int id = std::stoi(s);
            auto path = fs::path(std::string(MusicDownloadManager::sharedState()->pathForSFX(id)));
            if (path.string().starts_with("sfx/")) {
                auto gdFolder = fs::path(std::string(CCFileUtils::get()->getWritablePath2())) / "Resources";
                gdFolder = gdFolder / path.string();
                path = gdFolder;
            }
            if (fs::exists(path)) {
                sum += fs::file_size(path);
            }
        }

        double toMegabytes = sum / 1024.f / 1024.f;
        std::stringstream ss;
        ss << std::setprecision(3) << toMegabytes << "MB";
        callback(ss.str());
    }).detach();
}

fs::path NongManager::getJsonPath() {
    auto savedir = fs::path(Mod::get()->getSaveDir().string());
    return savedir / "nong_data.json";
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
            if (!this->addNongsFromSFH(ret, songID)) {
                callback(nongd::FetchStatus::FAILED);
            } else {
                callback(nongd::FetchStatus::SUCCESS);
            }
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
    if (fs::exists(song.path)) {
        return;
    }

    web::AsyncWebRequest()
        .fetch(song.songUrl)
        .into(ghc::filesystem::path(song.path.string()))
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

bool NongManager::addNongsFromSFH(std::vector<SFHItem> const& songs, int songID) {
    auto savedir = fs::path(Mod::get()->getSaveDir().string());
    auto nongsPath = savedir / "nongs";
    if (!fs::exists(nongsPath)) {
        fs::create_directory(nongsPath);
    }
    auto result = this->getNongs(songID);
    if (!result.has_value()) {
        return false;
    }
    auto nongs = result.value();
    int index = 1;
    for (auto const& sfhSong : songs) {
        bool update = false;
        auto path = nongsPath;
        auto unique = nongd::random_string(16);
        path.append(std::to_string(songID) + "_" + sfhSong.levelName + "_" + unique + ".mp3");
        for (auto& localSong : nongs.songs) {
            if (localSong.songUrl == sfhSong.downloadUrl) {
                update = true;
                break;
            }
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
            .songUrl = sfhSong.downloadUrl,
            .levelName = sfhSong.levelName
        };

        if (update) {
            for(auto& localSong : nongs.songs) {
                if (localSong.songUrl == song.songUrl) {
                    localSong.songName = song.songName;
                    localSong.authorName = song.authorName;
                    localSong.levelName = song.levelName;
                }
            }
        } else {
            nongs.songs.push_back(song);
        }
    }

    this->saveNongs(nongs, songID);
    return true;
}

void NongManager::loadSongs() {
    auto path = this->getJsonPath();
    if (!fs::exists(path)) {
        return;
    }
    std::ifstream input(path.string());
    std::stringstream buffer;
    buffer << input.rdbuf();
    input.close();

    auto json = matjson::parse(std::string_view(buffer.str()));
    m_state = matjson::Serialize<NongState>::from_json(json);
}