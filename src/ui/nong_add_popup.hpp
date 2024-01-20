// #pragma once

// #include <Geode/Geode.hpp>

// #include "../types/song_info.hpp"
// #include "../random_string.hpp"
// #include "nong_dropdown_layer.hpp"

// using namespace geode::prelude;

// class NongDropdownLayer;

// class NongAddPopup : public Popup<NongDropdownLayer*> {
// protected:
//     NongDropdownLayer* m_parentPopup;
//     CCMenuItemSpriteExtra* m_selectSongButton;
//     CCMenuItemSpriteExtra* m_addSongButton;
//     CCMenu* m_selectSongMenu;
//     CCMenu* m_addSongMenu;
//     CCLabelBMFont* m_selectedSongPath;
//     CCLayer* m_containerLayer;

//     CCTextInputNode* m_songNameInput;
//     CCTextInputNode* m_artistNameInput;

//     ghc::filesystem::path m_songPath;

//     bool setup(NongDropdownLayer* parent) override;
//     void createSelectedSongLabel(const std::string&);
//     void createInputs();

//     CCSize getPopupSize();
//     void openFile(CCObject*);
//     void addSong(CCObject*);
// public:
//     static NongAddPopup* create(NongDropdownLayer* parent);
// };
