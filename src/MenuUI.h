#pragma once
#include <raylib.h>
#include "Assets.h"
#include "Grid.h"
#include "Button.h"

class MenuUI {
public:
	MenuUI() : button(Button::Anchor::TOP_RIGHT, { SIZE, SIZE }, MARGIN) {}

	void Render(Assets& assets) {
		Rectangle btn_rect = button.GetButtonRect();

		DrawSprite(assets.tilemap, Sprite::UI_ROTATE, btn_rect);
	}

	Button button;

private:
	static constexpr float MARGIN = 10;
	static constexpr float SIZE = 128;
};