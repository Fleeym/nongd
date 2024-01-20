#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/CustomSongWidget.hpp>

// #include "../types/song_info.hpp"
// #include "../managers/nong_manager.hpp"
// #include "../ui/nong_dropdown_layer.hpp"

using namespace geode::prelude;

class $modify(JBSongWidget, CustomSongWidget) {
    bool init(
        SongInfoObject* songInfo,
        CustomSongDelegate* songDelegate,
        bool showSongSelect,
        bool showPlayMusic,
        bool showDownload,
        bool isRobtopSong,
        bool unk,
        bool hasMultipleAssets
    ) {
        if (!CustomSongWidget::init(songInfo, songDelegate, showSongSelect, showPlayMusic, showDownload, isRobtopSong, unk, hasMultipleAssets)) {
            return false;
        }

        if (isRobtopSong) {
            return true;
        }
        

        log::info("This works!");

        return true;
    }
};

// class $modify(NongSongWidget, CustomSongWidget) {
// 	NongData nongData;
// 	int nongdSong;

// 	bool hasDefaultSong = false;
// 	bool firstRun = true;

// 	bool init(SongInfoObject* songInfo, LevelSettingsObject* levelSettings, bool p2, bool p3, bool p4, bool hasDefaultSong, bool hideBackground) {
// 		if (!CustomSongWidget::init(songInfo, levelSettings, p2, p3, p4, hasDefaultSong, hideBackground)) return false;

// 		if (!songInfo) {
// 			return true;
// 		}

// 		m_fields->firstRun = false;

// 		if (hasDefaultSong) {
// 			m_fields->hasDefaultSong = true;
// 			this->updateSongObject(m_songInfo);
// 			return true;
// 		}

// 		auto songNameLabel = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("song-name-label"));
// 		songNameLabel->setVisible(false);

// 		auto idAndSizeLabel = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("id-and-size-label"));
// 		idAndSizeLabel->setVisible(false);
// 		auto newLabel = CCLabelBMFont::create("new", "bigFont.fnt");
// 		newLabel->setID("nongd-id-and-size-label");
// 		newLabel->setPosition(ccp(0.f, -32.f));
// 		newLabel->setScale(0.4f);
// 		this->addChild(newLabel);

// 		m_fields->nongdSong = songInfo->m_songID;

// 		if (!NongManager::get()->checkIfNongsExist(songInfo->m_songID)) {
// 			auto strPath = std::string(MusicDownloadManager::sharedState()->pathForSong(songInfo->m_songID));

// 			SongInfo defaultSong = {
// 				.path = ghc::filesystem::path(strPath),
// 				.songName = songInfo->m_songName,
// 				.authorName = songInfo->m_artistName,
// 				.songUrl = songInfo->m_songURL,
// 			};

// 			NongManager::get()->createDefaultSongIfNull(defaultSong, songInfo->m_songID);
// 		}

// 		auto invalidSongs = NongManager::get()->validateNongs(songInfo->m_songID);

// 		if (invalidSongs.size() > 0) {
// 			std::string invalidSongList = "";
// 			for (auto &song : invalidSongs) {
// 				invalidSongList += song.songName + ", ";
// 			}

// 			invalidSongList = invalidSongList.substr(0, invalidSongList.size() - 2);
// 			// If anyone asks this was mat's idea
// 			Loader::get()->queueInMainThread([this, invalidSongList]() {
// 				auto alert = FLAlertLayer::create("Invalid NONGs", "The NONGs [<cr>" + invalidSongList + "</c>] have been deleted, because their paths were invalid.", "Ok");
// 				alert->m_scene = this->getParent();
// 				alert->show();
// 			});
// 		}

// 		m_fields->nongData = NongManager::get()->getNongs(m_songInfo->m_songID);
// 		SongInfo nong;
// 		for (auto song : m_fields->nongData.songs) {
// 			if (song.path == m_fields->nongData.active) {
// 				nong = song;
// 			}
// 		}

// 		m_songInfo->m_artistName = nong.authorName;
// 		m_songInfo->m_songName = nong.songName;
// 		this->updateSongObject(m_songInfo);
// 		if (auto found = this->getChildByID("song-name-menu")) {
// 			this->updateSongNameLabel(m_songInfo->m_songName, m_songInfo->m_songID);
// 		} else {
// 			this->addMenuItemLabel(m_songInfo->m_songName, m_songInfo->m_songID);
// 		}
// 		if (nong.path == m_fields->nongData.defaultPath) {
// 			this->updateIDAndSizeLabel(nong, m_songInfo->m_songID);
// 		} else {
// 			this->updateIDAndSizeLabel(nong);
// 		}

// 		return true;
// 	}
		
// 	void addMenuItemLabel(std::string const& text, int songID) {
// 		auto menu = CCMenu::create();
// 		menu->setID("song-name-menu");

// 		auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
// 		label->limitLabelWidth(220.f, 0.8f, 0.1f);
// 		auto info = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
// 		info->setScale(0.5f);
// 		auto songNameMenuLabel = CCMenuItemSpriteExtra::create(
// 			label,
// 			this,
// 			menu_selector(NongSongWidget::addNongLayer)
// 		);
// 		songNameMenuLabel->addChild(info);
// 		songNameMenuLabel->setTag(songID);
// 		// I am not even gonna try and understand why this works, but this places the label perfectly in the menu
// 		auto labelScale = label->getScale();
// 		songNameMenuLabel->setID("song-name-label");
// 		songNameMenuLabel->setPosition(ccp(0.f, 0.f));
// 		songNameMenuLabel->setAnchorPoint(ccp(0.f, 0.5f));
// 		menu->addChild(songNameMenuLabel);
// 		menu->setContentSize(ccp(220.f, 25.f));
// 		menu->setPosition(ccp(-140.f, 27.5f));
// 		auto layout = RowLayout::create();
// 		layout->setAxisAlignment(AxisAlignment::Start);
// 		layout->setAutoScale(false);
// 		songNameMenuLabel->setLayout(layout);
// 		songNameMenuLabel->setContentSize({ 220.f, labelScale * 30 });

// 		this->addChild(menu);
// 	}

// 	void updateSongNameLabel(std::string const& text, int songID) {
// 		auto menu = this->getChildByID("song-name-menu");
// 		auto labelMenuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("song-name-label"));
// 		labelMenuItem->setTag(songID);
// 		auto child = typeinfo_cast<CCLabelBMFont*>(labelMenuItem->getChildren()->objectAtIndex(0));
// 		child->setString(text.c_str());
// 		child->limitLabelWidth(220.f, 0.8f, 0.1f);
// 		auto labelScale = child->getScale();
// 		labelMenuItem->setContentSize({ 220.f, labelScale * 30 });
// 		labelMenuItem->updateLayout();
// 	}

// 	void updateIDAndSizeLabel(SongInfo const& song, int songID = 0) {
// 		auto label = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("nongd-id-and-size-label"));
// 		auto normalLabel = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("id-and-size-label"));
// 		auto defaultPath = m_fields->nongData.defaultPath;

// 		if (!ghc::filesystem::exists(song.path) && song.path == defaultPath) {
// 			label->setVisible(false);
// 			this->getChildByID("id-and-size-label")->setVisible(true);
// 			return;
// 		} else if (normalLabel && normalLabel->isVisible()) {
// 			normalLabel->setVisible(false);
// 			label->setVisible(true);
// 		}

// 		std::string sizeText;
// 		if (ghc::filesystem::exists(song.path)) {
// 			sizeText = NongManager::get()->getFormattedSize(song);
// 		} else {
// 			sizeText = "NA";
// 		}
// 		std::string labelText;
// 		if (songID != 0) {
// 			labelText = "SongID: " + std::to_string(songID) + "  Size: " + sizeText;
// 		} else {
// 			labelText = "SongID: NONG  Size: " + sizeText;
// 		}

// 		if (label) {
// 			label->setString(labelText.c_str());
// 		}
// 	}

// 	void updateSongObject(SongInfoObject* song) {
// 		if (m_fields->firstRun) {
// 			CustomSongWidget::updateSongObject(song);
// 			return;
// 		}

// 		if (m_fields->hasDefaultSong) {
// 			CustomSongWidget::updateSongObject(song);
// 			if (auto found = this->getChildByID("song-name-menu")) {
// 				found->setVisible(false);
// 				this->getChildByID("nongd-id-and-size-label")->setVisible(false);
// 			}
// 			this->getChildByID("id-and-size-label")->setVisible(true);
// 			return;
// 		}

// 		m_fields->nongdSong = song->m_songID;
// 		if (!NongManager::get()->checkIfNongsExist(song->m_songID)) {
// 			auto strPath = std::string(MusicDownloadManager::sharedState()->pathForSong(song->m_songID));

// 			SongInfo defaultSong = {
// 				.path = ghc::filesystem::path(strPath),
// 				.songName = song->m_songName,
// 				.authorName = song->m_artistName,
// 				.songUrl = song->m_songURL,
// 			};

// 			NongManager::get()->createDefaultSongIfNull(defaultSong, song->m_songID);
// 		}
// 		SongInfo active;
// 		auto nongData = NongManager::get()->getNongs(song->m_songID);
// 		for (auto nong : nongData.songs) {
// 			if (nong.path == nongData.active) {
// 				active = nong;
// 				song->m_songName = nong.songName;
// 				song->m_artistName = nong.authorName;
// 				if (nong.songUrl != "local") {
// 					song->m_songURL = nong.songUrl;
// 				}
// 			}
// 		}
// 		CustomSongWidget::updateSongObject(song);
// 		if (auto found = this->getChildByID("song-name-menu")) {
// 			this->updateSongNameLabel(song->m_songName, song->m_songID);
// 		} else {
// 			this->addMenuItemLabel(song->m_songName, song->m_songID);
// 		}
// 		if (active.path == nongData.defaultPath) {
// 			this->updateIDAndSizeLabel(active, song->m_songID);
// 		} else {
// 			this->updateIDAndSizeLabel(active);
// 		}
// 	}

// 	void addNongLayer(CCObject* target) {
// 		auto scene = CCDirector::sharedDirector()->getRunningScene();
// 		auto layer = NongDropdownLayer::create(m_fields->nongdSong, this);
// 		// based robtroll
// 		layer->setZOrder(106);
// 		scene->addChild(layer);
// 		layer->showLayer(false);
// 	}

// };