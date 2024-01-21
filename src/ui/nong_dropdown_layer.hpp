#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>

#include "../types/song_info.hpp"
#include "../types/fetch_status.hpp"
#include "../managers/nong_manager.hpp"
#include "nong_cell.hpp"

using namespace geode::prelude;

class NongDropdownLayer : public Popup<int, CustomSongWidget*> {
protected:
    NongData m_songs;
    int m_songID;
    Ref<CustomSongWidget> m_parentWidget;
    ListView* m_listView = nullptr;

    bool m_fetching = false;

    bool setup(int songID, CustomSongWidget* parent) override;
    void createList();
    SongInfo getActiveSong();
    CCSize getCellSize() const;
    void deleteAllNongs(CCObject*);
    void fetchSongFileHub(CCObject*);
    void onSFHFetched(nongd::FetchStatus result);
    void onSettings(CCObject*);
    void openAddPopup(CCObject*);
public:
    int getSongID();
    void setActiveSong(SongInfo const& song);
    void deleteSong(SongInfo const& song);
    void addSong(SongInfo const& song);
    void updateParentWidget(SongInfo const& song);
    void updateParentSizeAndIDLabel(SongInfo const& song, int songID = 0);
    void saveSongsToJson();

    static NongDropdownLayer* create(int songID, CustomSongWidget* parent) {
        auto ret = new NongDropdownLayer;
        if (ret && ret->init(420.f, 280.f, songID, parent, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};