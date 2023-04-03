#include "NongPopup.hpp"

bool NongPopup::setup(int songID, CustomSongWidget* parent) {
    this->m_songID = songID;
    this->m_parentWidget = parent;
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    // convenience function provided by Popup 
    // for adding/setting a title to the popup
    auto title = "NONGs for " + std::to_string(songID);
    this->setTitle(title);

    this->setSongs();
    this->createList();
    this->createAddButton();
    return true;
}

void NongPopup::createAddButton() {
    this->m_addButtonMenu = CCMenu::create();
    this->m_addButtonMenu->setID("add-button-menu");
    auto addButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"),
        this,
        menu_selector(NongPopup::openAddPopup)
    );
    addButton->setID("add-button");
    
    this->m_addButtonMenu->addChild(addButton);
    this->m_addButtonMenu->setPosition(ccp(524.5f, 29.f));
    this->addChild(m_addButtonMenu);
}

NongPopup* NongPopup::create(int songID, CustomSongWidget* parent) {
    auto ret = new NongPopup();
    auto size = ret->getPopupSize();
    if (ret && ret->init(size.width, size.height, songID, parent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCSize NongPopup::getPopupSize() const {
    return { 500.f, 280.f };
}

void NongPopup::setSongs() {
    m_songs = NongManager::getNongs(this->m_songID);
}

CCArray* NongPopup::createNongCells() {
    auto songs = CCArray::create();
    auto activeSong = this->getActiveSong();

    songs->addObject(NongCell::create(activeSong, this, this->getCellSize(), true));

    for (auto song : m_songs.songs) {
        if (m_songs.active == song.path) {
            continue;
        }
        songs->addObject(NongCell::create(song, this, this->getCellSize(), false));
    }

    return songs;
}

SongInfo NongPopup::getActiveSong() {
    for (auto song : m_songs.songs) {
        if (song.path == m_songs.active) {
            return song;
        }
    }
    throw std::exception("Yoy did bad");
}

void NongPopup::saveSongsToJson() {
    NongManager::saveNongs(this->m_songs, this->m_songID);
}

CCSize NongPopup::getCellSize() const {
    return {
        this->getListSize().width,
        60.f
    };
}

CCSize NongPopup::getListSize() const {
    return { 400.f, 190.f };
}

void NongPopup::createList() {
    auto cells = this->createNongCells();
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_listLayer = CCLayer::create();
    this->m_listLayer->setID("nong-list-layer");
    this->m_list = ListView::create(
        cells,
        this->getCellSize().height,
        this->getListSize().width,
        this->getListSize().height
    );
    this->m_list->setID("nong-list");

    this->m_list->setPositionY(-10.f);

    auto sideLeft = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
    sideLeft->setAnchorPoint(ccp(0, 0));
    sideLeft->setScaleY(5.45f);
    sideLeft->setScaleX(1.2f);
    sideLeft->setPositionX(-3.f);
    sideLeft->setZOrder(9);

    auto sideTop = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
    sideTop->setAnchorPoint(ccp(0, 0));
    sideTop->setScaleX(1.15f);
    sideTop->setPosition(ccp(-3.f, 163.f));
    sideTop->setZOrder(9);

    auto sideBottom = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
    sideBottom->setFlipY(true);
    sideBottom->setAnchorPoint(ccp(0, 0));
    sideBottom->setPosition(ccp(-3.f, -15.f));
    sideBottom->setScaleX(1.15f);
    sideBottom->setZOrder(9);

    auto sideRight = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
    sideRight->setFlipX(true);
    sideRight->setScaleY(5.45f);
    sideRight->setScaleX(1.2f);
    sideRight->setAnchorPoint(ccp(0, 0));
    sideRight->setPositionX(396.f);
    sideRight->setZOrder(9);

    this->m_listLayer->addChild(sideLeft);
    this->m_listLayer->addChild(sideTop);
    this->m_listLayer->addChild(sideBottom);
    this->m_listLayer->addChild(sideRight);
    this->m_listLayer->addChild(this->m_list);
    this->m_listLayer->setPosition(winSize / 2 - m_list->getScaledContentSize() / 2);
    this->addChild(m_listLayer);
}

void NongPopup::setActiveSong(SongInfo const& song) {
    this->m_songs.active = song.path;

    this->saveSongsToJson();
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

    this->m_listLayer->removeAllChildrenWithCleanup(true);
    this->removeChild(m_listLayer);
    CC_SAFE_DELETE(m_listLayer);
    this->createList();
}

void NongPopup::openAddPopup(CCObject* target) {
    auto popup = NongAddPopup::create(this);
    popup->m_noElasticity = true;
    popup->show();
}

void NongPopup::addSong(SongInfo const& song) {
    for (auto savedSong : this->m_songs.songs) {
        if (song.path.string() == savedSong.path.string()) {
            FLAlertLayer::create("Error", "This NONG already exists! (<cy>" + savedSong.songName + "</c>)", "Ok")->show();
            return;
        }
    }
    NongManager::addNong(song, this->m_songID);
    FLAlertLayer::create("Success", "The song was added!", "Ok")->show();
    this->m_listLayer->removeAllChildrenWithCleanup(true);
    this->removeChild(m_listLayer);
    CC_SAFE_DELETE(m_listLayer);
    this->setSongs();
    this->createList();
}

void NongPopup::deleteSong(SongInfo const& song) {
    NongManager::deleteNong(song, this->m_songID);
    FLAlertLayer::create("Success", "The song was deleted!", "Ok")->show();
    this->m_listLayer->removeAllChildrenWithCleanup(true);
    this->removeChild(m_listLayer);
    CC_SAFE_DELETE(m_listLayer);
    this->setSongs();
    this->createList();
}

void NongPopup::updateParentSizeAndIDLabel(SongInfo const& song, int songID) {
		auto label = typeinfo_cast<CCLabelBMFont*>(this->m_parentWidget->getChildByID("id-and-size-label"));
		auto sizeText = NongManager::getFormattedSize(song);
		std::string labelText;
		if (songID != 0) {
			labelText = "SongID: " + std::to_string(songID) + "  Size: " + sizeText;
		} else {
			labelText = "SongID: NONG  Size: " + sizeText;
		}

		label->setString(labelText.c_str());
	}