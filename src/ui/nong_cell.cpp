// #include "nong_cell.hpp"

// bool NongCell::init(SongInfo info, NongDropdownLayer* parentPopup, CCSize const& size, bool selected, bool isDefault) {
//     if (!JBListCell::init(parentPopup, size)) return false;

//     m_songInfo = info;
//     m_parentPopup = parentPopup;

//     CCMenuItemSpriteExtra* button;

//     if (selected) {
//         auto sprite = ButtonSprite::create("Set", "goldFont.fnt", "GJ_button_02.png");
//         button = CCMenuItemSpriteExtra::create(
//             sprite,
//             this,
//             menu_selector(NongCell::onSet)
//         );
//     } else {
//         button = CCMenuItemSpriteExtra::create(
//             ButtonSprite::create("Set"),
//             this,
//             menu_selector(NongCell::onSet)
//         );
//     }
//     button->setAnchorPoint(ccp(0.5f, 0.5f));
//     button->setID("set-button");

//     auto menu = CCMenu::create();
//     menu->addChild(button);

//     if (!isDefault) {
//         auto deleteButton = CCMenuItemSpriteExtra::create(
//             CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"),
//             this,
//             menu_selector(NongCell::deleteSong)
//         );
//         deleteButton->setID("delete-button");
//         menu->addChild(deleteButton);
//         deleteButton->setPositionX(53.f);
//     }

//     menu->setAnchorPoint(ccp(0, 0));
//     menu->setPosition(ccp(280.f, 30.f));
//     menu->setID("button-menu");
//     this->addChild(menu);

//     m_songInfoLayer = CCLayer::create();

//     m_songNameLabel = CCLabelBMFont::create(m_songInfo.songName.c_str(), "bigFont.fnt");
//     m_songNameLabel->limitLabelWidth(220.f, 0.8f, 0.1f);

//     if (selected) {
//         m_songNameLabel->setColor(ccc3(188, 254, 206));
//     }

//     m_authorNameLabel = CCLabelBMFont::create(m_songInfo.authorName.c_str(), "goldFont.fnt");
//     m_authorNameLabel->limitLabelWidth(240.f, 0.7f, 0.1f);
//     m_authorNameLabel->setID("author-name");
//     m_songNameLabel->setID("song-name");

//     m_songInfoLayer->addChild(m_authorNameLabel);
//     m_songInfoLayer->addChild(m_songNameLabel);
//     m_songInfoLayer->setID("song-info");
//     auto layout = ColumnLayout::create();
//     layout->setAutoScale(false);
//     layout->setAxisAlignment(AxisAlignment::Even);
//     layout->setCrossAxisLineAlignment(AxisAlignment::Start);
//     m_songInfoLayer->setContentSize(ccp(240.f, this->getContentSize().height - 6.f));
//     m_songInfoLayer->setAnchorPoint(ccp(0.f, 0.f));
//     m_songInfoLayer->setPosition(ccp(12.f, 1.5f));
//     m_songInfoLayer->setLayout(layout);

//     this->addChild(m_songInfoLayer);
//     return true;
// }

// void NongCell::onSet(CCObject* target) {
//     m_parentPopup->setActiveSong(this->m_songInfo);
// }

// void NongCell::deleteSong(CCObject* target) {
//     FLAlertLayer::create(this, "Are you sure?", "Are you sure you want to delete <cy>" + this->m_songInfo.songName + "</c> from your NONGs?", "No", "Yes")->show();
// }

// NongCell* NongCell::create(SongInfo info, NongDropdownLayer* parentPopup, CCSize const& size, bool selected, bool isDefault) {
//     auto ret = new NongCell();
//     if (ret && ret->init(info, parentPopup, size, selected, isDefault)) {
//         return ret;
//     }
//     CC_SAFE_DELETE(ret);
//     return nullptr;
// }

// void NongCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
//     if (btn2) {
//         m_parentPopup->deleteSong(m_songInfo);
//     }
// }
