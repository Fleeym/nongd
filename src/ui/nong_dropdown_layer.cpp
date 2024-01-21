#include "nong_dropdown_layer.hpp"

bool NongDropdownLayer::setup(int songID, CustomSongWidget* parent) {
    m_songID = songID;
    m_parentWidget = parent;
    auto result = NongManager::get()->getNongs(m_songID);
    if (!result.has_value()) {
        NongManager::get()->createDefault(m_songID);
        NongManager::get()->writeJson();
        result = NongManager::get()->getNongs(m_songID);
    }
    m_songs = result.value();
    auto winsize = CCDirector::sharedDirector()->getWinSize();

    auto spr = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
    spr->setScale(0.7f);
    auto menu = CCMenu::create();
    menu->setID("bottom-right-menu");
    auto downloadBtn = CCMenuItemSpriteExtra::create(
        spr,
        this,
        // menu_selector(NongDropdownLayer::fetchSongFileHub)
        nullptr
    );
    downloadBtn->setPositionY(35.f);
    spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    spr->setScale(0.7f);
    auto addBtn = CCMenuItemSpriteExtra::create(
        spr,
        this,
        // menu_selector(NongDropdownLayer::openAddPopup)
        nullptr
    );
    spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    spr->setScale(0.7f);
    auto removeBtn = CCMenuItemSpriteExtra::create(
        spr,
        this,
        // menu_selector(NongDropdownLayer::deleteAllNongs)
        nullptr
    );
    removeBtn->setPositionY(67.f);

    menu->addChild(addBtn);
    menu->addChild(downloadBtn);
    menu->addChild(removeBtn);
    menu->setPosition(winsize.width / 2 + 185.f, winsize.height / 2 - 105.f);
    m_mainLayer->addChild(menu);

    menu = CCMenu::create();
    menu->setID("settings-menu");
    auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    sprite->setScale(0.8f);
    auto settingsButton = CCMenuItemSpriteExtra::create(
        sprite,
        this,
        menu_selector(NongDropdownLayer::onSettings)
    );
    settingsButton->setID("settings-button");
    menu->addChild(settingsButton);
    menu->setPosition(winsize.width - 30.f, winsize.height - 31.f);
    m_mainLayer->addChild(menu);

    this->createList();
    auto listpos = m_listView->getPosition();
    auto leftspr = CCSprite::createWithSpriteFrameName("GJ_commentSide2_001.png");
    leftspr->setPosition(ccp(listpos.x - 162.f, listpos.y));
    leftspr->setScaleY(6.8f);
    m_mainLayer->addChild(leftspr);
    auto rightspr = CCSprite::createWithSpriteFrameName("GJ_commentSide2_001.png");
    rightspr->setPosition(ccp(listpos.x + 162.f, listpos.y));
    rightspr->setScaleY(6.8f);
    rightspr->setFlipX(true);
    m_mainLayer->addChild(rightspr);
    auto bottomspr = CCSprite::createWithSpriteFrameName("GJ_commentTop2_001.png");
    bottomspr->setPosition(ccp(listpos.x, listpos.y - 95.f));
    bottomspr->setFlipY(true);
    bottomspr->setScaleX(0.934f);
    m_mainLayer->addChild(bottomspr);
    auto topspr = CCSprite::createWithSpriteFrameName("GJ_commentTop2_001.png");
    topspr->setPosition(ccp(listpos.x, listpos.y + 95.f));
    topspr->setScaleX(0.934f);
    m_mainLayer->addChild(topspr);
    auto title = CCSprite::create("JB_ListLogo.png"_spr);
    title->setPosition(ccp(winsize.width / 2, winsize.height / 2 + 125.f));
    title->setScale(2.15f);
    m_mainLayer->addChild(title);
    return true;
}

void NongDropdownLayer::onSettings(CCObject* sender) {
    geode::openSettingsPopup(Mod::get());
}

// void NongDropdownLayer::openAddPopup(CCObject* target) {
//     NongAddPopup::create(this)->show();
// }

void NongDropdownLayer::createList() {
    auto songs = CCArray::create();
    auto activeSong = this->getActiveSong();

    songs->addObject(NongCell::create(activeSong, this, this->getCellSize(), true, activeSong.path == m_songs.defaultPath));

    for (auto song : m_songs.songs) {
        // if (m_songs.active == song.path) {
        //     continue;
        // }
        songs->addObject(NongCell::create(song, this, this->getCellSize(), false, song.path == m_songs.defaultPath));
    }
    if (m_listView) {
        m_listView->removeFromParent();
    }

    auto list = ListView::create(songs, this->getCellSize().height, this->getCellSize().width, 200.f);
    m_mainLayer->addChild(list);
    auto winsize = CCDirector::sharedDirector()->getWinSize();
    list->setPosition(winsize.width / 2, winsize.height / 2 - 15.f);
    list->ignoreAnchorPointForPosition(false);
    m_listView = list;
}

SongInfo NongDropdownLayer::getActiveSong() {
    auto active = NongManager::get()->getActiveNong(m_songID);
    if (!active.has_value()) {
        m_songs.active = m_songs.defaultPath;
        NongManager::get()->saveNongs(m_songs, m_songID);
        return NongManager::get()->getActiveNong(m_songID).value();
    }
    return active.value();
}

CCSize NongDropdownLayer::getCellSize() const {
    return {
        320.f,
        60.f
    };
}

void NongDropdownLayer::setActiveSong(SongInfo const& song) {
    if (
        !fs::exists(song.path) && 
        song.path != m_songs.defaultPath &&
        song.songUrl != "local"
    ) {
        auto loading = LoadingCircle::create();
        loading->setParentLayer(this);
        loading->setFade(true);
        loading->show();
        m_fetching = true;
        NongManager::get()->downloadSong(song, [this, song, loading](double progress) {
            if (progress == 100.f) {
                m_fetching = false;
                this->updateParentWidget(song);
                loading->fadeAndRemove();
            }
        },
        [this, loading](SongInfo const& song, std::string const& error) {
            loading->fadeAndRemove();
            m_fetching = false;
            FLAlertLayer::create("Failed", "Failed to download song", "Ok")->show();

            for (auto song : m_songs.songs) {
                if (song.path == m_songs.defaultPath) {
                    this->setActiveSong(song);
                }
            }
        });
    }

    m_songs.active = song.path;

    this->saveSongsToJson();
    
    this->updateParentWidget(song);

    this->createList();
}

void NongDropdownLayer::updateParentWidget(SongInfo const& song) {
    m_parentWidget->m_songInfoObject->m_artistName = song.authorName;
    m_parentWidget->m_songInfoObject->m_songName = song.songName;
    if (song.songUrl != "local") {
        m_parentWidget->m_songInfoObject->m_songUrl = song.songUrl;
    }
    m_parentWidget->updateSongObject(this->m_parentWidget->m_songInfoObject);
    // if (m_songs.defaultPath == song.path) {
    //     this->updateParentSizeAndIDLabel(song, m_songID);
    // } else {
    //     this->updateParentSizeAndIDLabel(song);
    // }
}

// void NongDropdownLayer::updateParentSizeAndIDLabel(SongInfo const& song, int songID) {
//     auto label = typeinfo_cast<CCLabelBMFont*>(m_parentWidget->getChildByID("nongd-id-and-size-label"));
//     if (!label) {
//         return;
//     }
//     auto sizeText = NongManager::get()->getFormattedSize(song);
//     std::string labelText;
//     if (songID != 0) {
//         labelText = "SongID: " + std::to_string(songID) + "  Size: " + sizeText;
//     } else {
//         labelText = "SongID: NONG  Size: " + sizeText;
//     }
//     if (label) {
//         label->setString(labelText.c_str());
//     }
// }

// void NongDropdownLayer::deleteSong(SongInfo const& song) {
//     NongManager::get()->deleteNong(song, m_songID);
//     this->updateParentWidget(NongManager::get()->getActiveNong(m_songID));
//     FLAlertLayer::create("Success", "The song was deleted!", "Ok")->show();
//     m_songs = NongManager::get()->getNongs(m_songID);
//     this->createList();
// }

// void NongDropdownLayer::addSong(SongInfo const& song) {
//     for (auto savedSong : m_songs.songs) {
//         if (song.path.string() == savedSong.path.string()) {
//             FLAlertLayer::create("Error", "This NONG already exists! (<cy>" + savedSong.songName + "</c>)", "Ok")->show();
//             return;
//         }
//     }
//     NongManager::get()->addNong(song, m_songID);
//     this->updateParentWidget(song);
//     FLAlertLayer::create("Success", "The song was added!", "Ok")->show();
//     m_songs = NongManager::get()->getNongs(m_songID);
//     this->createList();
// }

void NongDropdownLayer::saveSongsToJson() {
    NongManager::get()->saveNongs(m_songs, m_songID);
}

// void NongDropdownLayer::onSFHFetched(nongd::FetchStatus result) {
//     switch (result) {
//         case nongd::FetchStatus::SUCCESS:
//             FLAlertLayer::create("Success", "The Song File Hub data was fetched successfully!", "Ok")->show();
//             m_songs = NongManager::get()->getNongs(m_songID);
//             this->createList();
//             break;
//         case nongd::FetchStatus::NOTHING_FOUND:
//             FLAlertLayer::create("Failed", "Found no data for this song!", "Ok")->show();
//             break;
//         case nongd::FetchStatus::FAILED:
//             FLAlertLayer::create("Failed", "Failed to fetch data from Song File Hub!", "Ok")->show();
//             break;
//     }
// }

// void NongDropdownLayer::fetchSongFileHub(CCObject*) {
//     createQuickPopup(
//         "Fetch SFH", 
//         "Do you want to fetch <cl>Song File Hub</c> content for <cy>" + std::to_string(m_songID) + "</c>?", 
//         "No", "Yes",
//         [this](auto, bool btn2) {
//             if (btn2) {
//                 auto loading = LoadingCircle::create();
//                 loading->setParentLayer(this);
//                 loading->setFade(true);
//                 loading->show();
//                 m_fetching = true;
//                 NongManager::get()->fetchSFH(m_songID, [this, loading](nongd::FetchStatus result) {
//                     this->onSFHFetched(result);
//                     m_fetching = false;
//                     loading->fadeAndRemove();
//                 });
//             }
//         }
//     );
// }

// void NongDropdownLayer::deleteAllNongs(CCObject*) {
//     createQuickPopup("Delete all nongs", 
//         "Are you sure you want to <cr>delete all nongs</c> for this song?", 
//         "No", 
//         "Yes",
//         [this](auto, bool btn2) {
//             if (!btn2) {
//                 return;
//             }

//             m_songs = NongManager::get()->deleteAll(m_songID);
//             this->updateParentWidget(this->getActiveSong());
//             std::vector<SongInfo> newSongs;
//             for (auto song : m_songs.songs) {
//                 if (song.path == m_songs.defaultPath) {
//                     newSongs.push_back(song);
//                     break;
//                 }
//             }
//             m_songs.active = m_songs.defaultPath;
//             this->createList();
//             FLAlertLayer::create("Success", "All nongs were deleted successfully!", "Ok")->show();
//         }
//     );
// }