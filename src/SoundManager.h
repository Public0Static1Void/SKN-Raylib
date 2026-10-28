#pragma once
#include "SoundDisplayer.h"

#include <array>
#include <iterator>
#include <vector>

class SoundManager {
public:

	enum SoundType { BG_MUSIC, MOVE, ROTATE, SHOOT, HIT, CRATE_HIT, UI_CLICK, UI_ERROR, START_LEVEL, REWIND, LAST_NO_USE };

	SoundManager() = default;
	~SoundManager() {
		for (auto& sd : sounds) {
			sd.Unload();
		}
	}
	bool InitSounds() {
		bool allLoaded = false;
		for (size_t i = 0; i < SoundType::LAST_NO_USE; i++) {
			allLoaded = sounds[i].LoadFile(FILES[i].fileName, FILES[i].isMusic);
		}
		return allLoaded;
	}
	

	void PlayStoredSound(SoundType type) {
		int i = static_cast<size_t>(type);
		if (i < sounds.size())
			sounds[i].Play();
	}

	void UpdateSounds() {
		for (auto& sd : sounds) {
			sd.Update();
		}
	}

private:

	std::array<SoundDisplayer, static_cast<size_t>(SoundType::LAST_NO_USE)> sounds;

	struct FileType {
		const char* fileName = "";
		bool isMusic = false;
	};
	static constexpr FileType FILES[] = {
		{ "assets/sounds/s_music_gameloop.wav", true},
		{ "assets/sounds/s_move_box.wav", false },
		{ "assets/sounds/s_rotate.wav", false },
		{ "assets/sounds/s_small_explosion.mp3", false },
		{ "assets/sounds/s_body_hit.wav", false },
		{ "assets/sounds/s_wood_hit.wav", false },
		{ "assets/sounds/s_slider.wav", false },
		{ "assets/sounds/s_select.wav", false },
		{ "assets/sounds/s_start_level.wav", false },
		{ "assets/sounds/s_rewind.wav", false }
	};
	static_assert(std::size(FILES) == static_cast<size_t>(SoundType::LAST_NO_USE),
		"FILES needs one FileType per SoundType");
};