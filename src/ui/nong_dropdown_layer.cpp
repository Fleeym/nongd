#include "nong_dropdown_layer.hpp"

void NongDropdownLayer::setup() {
    log::info("song id: {}", m_songID);
    m_songs = NongManager::get()->getNongs(m_songID);
    this->createList();
}

void NongDropdownLayer::createList() {
    auto songs = CCArray::create();
    auto activeSong = this->getActiveSong();

    songs->addObject(NongCell::create(activeSong, this, this->getCellSize(), true, activeSong.path == m_songs.defaultPath));

    for (auto song : m_songs.songs) {
        if (m_songs.active == song.path) {
            continue;
        }
        songs->addObject(NongCell::create(song, this, this->getCellSize(), false, song.path == m_songs.defaultPath));
    }
    if (m_listLayer->m_listView) {
        m_listLayer->m_listView->removeFromParent();
    }

    auto list = ListView::create(songs, this->getCellSize().height, this->getCellSize().width);
    m_listLayer->addChild(list);
    m_listLayer->m_listView = list;
}

SongInfo NongDropdownLayer::getActiveSong() {
    for (auto song : m_songs.songs) {
        if (song.path == m_songs.active) {
            return song;
        }
    }
    
    m_songs.active = m_songs.defaultPath;
    NongManager::get()->saveNongs(m_songs, this->m_songID);
    for (auto song : m_songs.songs) {
        if (song.path == m_songs.active) {
            return song;
        }
    }

    throw std::runtime_error("If you somehow reached this, good job.");
}

CCSize NongDropdownLayer::getCellSize() const {
    return {
        m_listLayer->getContentSize().width,
        60.f
    };
}

void NongDropdownLayer::setActiveSong(SongInfo const& song) {
    if (
        !ghc::filesystem::exists(song.path) && 
        song.path != m_songs.defaultPath &&
        song.songUrl != "local"
    ) {
        this->setKeyboardEnabled(false);
        this->setKeypadEnabled(false);
        this->setMouseEnabled(false);
        auto loading = LoadingCircle::create();
        loading->setParentLayer(this);
        loading->setFade(true);
        loading->show();
        NongManager::get()->downloadSong(song, [this, song, loading](double progress) {
            if (progress == 100.f) {
                loading->fadeAndRemove();
                this->updateParentSizeAndIDLabel(song);
                this->setKeyboardEnabled(true);
                this->setKeypadEnabled(true);
                this->setMouseEnabled(true);
            }
        },
        [this, loading](SongInfo const& song, std::string const& error) {
            this->setKeyboardEnabled(true);
            this->setKeypadEnabled(true);
            this->setMouseEnabled(true);
            loading->fadeAndRemove();
            FLAlertLayer::create("Failed", "Failed to download song", "Ok")->show();

            for (auto song : m_songs.songs) {
                if (song.path == m_songs.defaultPath) {
                    this->setActiveSong(song);
                }
            }
        });
    }

    this->m_songs.active = song.path;

    this->saveSongsToJson();
    
    this->updateParentWidget(song);

    this->createList();
}

void NongDropdownLayer::updateParentWidget(SongInfo const& song) {
    this->m_parentWidget->m_songInfo->m_artistName = song.authorName;
    this->m_parentWidget->m_songInfo->m_songName = song.songName;
    if (song.songUrl != "local") {
        this->m_parentWidget->m_songInfo->m_songURL = song.songUrl;
    }
    this->m_parentWidget->updateSongObject(this->m_parentWidget->m_songInfo);
    if (this->m_songs.defaultPath == song.path) {
        this->updateParentSizeAndIDLabel(song, this->m_songID);
    } else {
        this->updateParentSizeAndIDLabel(song);
    }
}

void NongDropdownLayer::updateParentSizeAndIDLabel(SongInfo const& song, int songID) {
    auto label = typeinfo_cast<CCLabelBMFont*>(this->m_parentWidget->getChildByID("nongd-id-and-size-label"));
    if (!label) {
        return;
    }
    auto sizeText = NongManager::get()->getFormattedSize(song);
    std::string labelText;
    if (songID != 0) {
        labelText = "SongID: " + std::to_string(songID) + "  Size: " + sizeText;
    } else {
        labelText = "SongID: NONG  Size: " + sizeText;
    }
    if (label) {
        label->setString(labelText.c_str());
    }
}

void NongDropdownLayer::deleteSong(SongInfo const& song) {
    NongManager::get()->deleteNong(song, m_songID);
    this->updateParentWidget(NongManager::get()->getActiveNong(m_songID));
    FLAlertLayer::create("Success", "The song was deleted!", "Ok")->show();
    m_songs = NongManager::get()->getNongs(m_songID);
    this->createList();
}

void NongDropdownLayer::addSong(SongInfo const& song) {
    for (auto savedSong : m_songs.songs) {
        if (song.path.string() == savedSong.path.string()) {
            FLAlertLayer::create("Error", "This NONG already exists! (<cy>" + savedSong.songName + "</c>)", "Ok")->show();
            return;
        }
    }
    NongManager::get()->addNong(song, m_songID);
    this->updateParentWidget(song);
    FLAlertLayer::create("Success", "The song was added!", "Ok")->show();
    m_songs = NongManager::get()->getNongs(m_songID);
    this->createList();
}

void NongDropdownLayer::saveSongsToJson() {
    NongManager::get()->saveNongs(m_songs, m_songID);
}

void NongDropdownLayer::onSFHFetched(nongd::FetchStatus result) {
    switch (result) {
        case nongd::FetchStatus::SUCCESS:
            FLAlertLayer::create("Success", "The Song File Hub data was fetched successfully!", "Ok")->show();
            m_songs = NongManager::get()->getNongs(m_songID);
            this->createList();
            break;
        case nongd::FetchStatus::NOTHING_FOUND:
            FLAlertLayer::create("Failed", "Found no data for this song!", "Ok")->show();
            break;
        case nongd::FetchStatus::FAILED:
            FLAlertLayer::create("Failed", "Failed to fetch data from Song File Hub!", "Ok")->show();
            break;
    }
}

void NongDropdownLayer::fetchSongFileHub(CCObject*) {
    createQuickPopup(
        "Fetch SFH", 
        "Do you want to fetch <cl>Song File Hub</c> content for <cy>" + std::to_string(this->m_songID) + "</c>?", 
        "No", "Yes",
        [this](auto, bool btn2) {
            if (btn2) {
                NongManager::get()->fetchSFH(m_songID, [this](nongd::FetchStatus result) {
                    this->onSFHFetched(result);
                });
            }
        }
    );
}

void NongDropdownLayer::deleteAllNongs(CCObject*) {
    createQuickPopup("Delete all nongs", 
        "Are you sure you want to <cr>delete all nongs</c> for this song?", 
        "No", 
        "Yes",
        [this](auto, bool btn2) {
            if (!btn2) {
                return;
            }

            m_songs = NongManager::get()->deleteAll(this->m_songID);
            this->updateParentWidget(this->getActiveSong());
            std::vector<SongInfo> newSongs;
            for (auto song : m_songs.songs) {
                if (song.path == m_songs.defaultPath) {
                    newSongs.push_back(song);
                    break;
                }
            }
            m_songs.active = m_songs.defaultPath;
            this->createList();
            FLAlertLayer::create("Success", "All nongs were deleted successfully!", "Ok")->show();
        }
    );
}