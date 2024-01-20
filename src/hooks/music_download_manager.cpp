// #include <Geode/binding/MusicDownloadManager.hpp>
// #include <Geode/modify/MusicDownloadManager.hpp>

// #include "../managers/nong_manager.hpp"
// #include "../types/song_info.hpp"

// class $modify(MusicDownloadManager) {
// 	gd::string pathForSong(int id) {
// 		if (!NongManager::get()->checkIfNongsExist(id)) {
// 			return MusicDownloadManager::pathForSong(id);
// 		}
// 		auto currentData = NongManager::get()->getNongs(id);
// 		if (ghc::filesystem::exists(currentData.active)) {
// 			return currentData.active.string();
// 		}
// 		return MusicDownloadManager::pathForSong(id);
// 	}
// };