#pragma once
#include <raylib.h>

enum class Sprite { PLAYER, ENEMY, CANNON, BULLET, WALL, FLOOR, LAST_NO_USE };
constexpr int SPRITE_SIZE = 32;

inline void DrawSprite(const Texture2D& tex, Sprite s, Rectangle dest, float rotation = 0, Color tint = WHITE) {
	Rectangle src = { (float)((int)s * SPRITE_SIZE), 0.0f, (float)SPRITE_SIZE, (float)SPRITE_SIZE };
	Rectangle d = { dest.x + dest.width / 2, dest.y + dest.height / 2, dest.width, dest.height };
	DrawTexturePro(tex, src, d, { dest.width / 2, dest.height / 2 }, rotation, tint);
}

class Assets {
public:
	Assets() = default;
	~Assets() { Unload(); };

	Assets(const Assets&) = delete;
	Assets& operator=(const Assets&) = delete;

	void Load(const char* fileName) {
		tilemap = LoadTexture(fileName);
		SetTextureFilter(tilemap, TEXTURE_FILTER_POINT);
		loaded = true;
	}
	void Unload() {
		if (loaded) {
			UnloadTexture(tilemap);
			loaded = false;
		}
	}

	Texture2D tilemap{};

private:
	bool loaded = false;
};