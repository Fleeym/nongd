// #pragma once

// #include <Geode/Geode.hpp>
// #include <Geode/ui/GeodeUI.hpp>

// #include "../types/song_info.hpp"
// #include "../types/fetch_status.hpp"
// #include "../managers/nong_manager.hpp"
// #include "nong_cell.hpp"
// #include "nong_add_popup.hpp"

// using namespace geode::prelude;

// class NongDropdownLayer : public GJDropDownLayer {
// protected:
//     NongData m_songs;
//     int m_songID;
//     Ref<CustomSongWidget> m_parentWidget;

//     bool m_fetching = false;

//     void setup();
//     void createList();
//     SongInfo getActiveSong();
//     CCSize getCellSize() const;
//     void deleteAllNongs(CCObject*);
//     void fetchSongFileHub(CCObject*);
//     void onSFHFetched(nongd::FetchStatus result);
//     void onSettings(CCObject*);
//     void openAddPopup(CCObject*);
//     void exitLayer(CCObject* target) override;
// public:
//     int getSongID();
//     void setActiveSong(SongInfo const& song);
//     void deleteSong(SongInfo const& song);
//     void addSong(SongInfo const& song);
//     void updateParentWidget(SongInfo const& song);
//     void updateParentSizeAndIDLabel(SongInfo const& song, int songID = 0);
//     void saveSongsToJson();

//     static NongDropdownLayer* create(int songID, CustomSongWidget* parent) {
//         log::info("proceeds to call create 51000 times");
//         auto ret = new NongDropdownLayer;
//         std::stringstream ss;
//         ss << "nongs for ";
//         ss << songID;
//         if (ret && ret->init(ss.str().c_str(), 220.f)) {
//             ret->m_parentWidget = parent;
//             ret->m_songID = songID;
//             ret->setup();
//             ret->autorelease();
//             return ret;
//         }

//         CC_SAFE_DELETE(ret);
//         return nullptr;
//     }
// };