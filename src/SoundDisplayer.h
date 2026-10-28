#pragma once
#include <raylib.h>
#include <utility>
class SoundDisplayer {
public:
	enum FileType { MUSIC, SOUND, NONE };

	SoundDisplayer() = default;
	~SoundDisplayer() { Unload(); }

	SoundDisplayer(const SoundDisplayer&) = delete;
	SoundDisplayer& operator=(const SoundDisplayer&) = delete;

	bool LoadFile(const char* fileName, bool isMusic) {
		if (isMusic)
			return LoadMusicFile(fileName);
		else
			return LoadSoundFile(fileName);
	}
	bool LoadMusicFile(const char* fileName) {
		if (type != FileType::NONE) return true;

		music = LoadMusicStream(fileName);
		if (!IsMusicValid(music)) {
			TraceLog(LOG_ERROR, "[SOUND DISPLAYER] Couldn't load '%s'", fileName);
			return false;
		}
		type = FileType::MUSIC;

		return true;
	}
	bool LoadSoundFile(const char* fileName) {
		if (type != FileType::NONE) return true;

		sound = LoadSound(fileName);
		if (!IsSoundValid(sound)) {
			TraceLog(LOG_ERROR, "[SOUND DISPLAYER] Couldn't load '%s'", fileName);
			return false;
		}
		type = FileType::SOUND;

		return true;
	}

	void Unload() {
		if (type != FileType::NONE) {
			if (IsMusicValid(music)) UnloadMusicStream(music);
			if (IsSoundValid(sound)) UnloadSound(sound);
			type = FileType::NONE;
		}
	}

	void Play() {
		if (type == FileType::NONE) return;

		switch (type) {
		case FileType::MUSIC:
			PlayMusicStream(music);
			break;
		case FileType::SOUND:
			PlaySound(sound);
			break;
		}
	}

	void Update() {
		if (type == FileType::MUSIC) {
			UpdateMusicStream(music);
		}
	}

private:
	Music music{};
	Sound sound{};

	FileType type = FileType::NONE;
};