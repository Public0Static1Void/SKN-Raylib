#include "InteractiveOverlay.h"
#include "Object.h"
#include <string>

void InteractiveOverlay::DrawOverlay(const Assets& assets, const Grid& grid, const Object* ob) const {
	if (!visible) return;

	int moves = ob != nullptr ? ob->GetMoves() : 0;

	for (int i = 0; i < (int)options.size(); i++) {
		// Draw the sprite
		Rectangle rect = OptionRec(i, grid);
		DrawSprite(assets.tilemap, options[i].icon, rect);
		if (ob != nullptr && options[i].render_moves > 0) {
			const int fontSize = (int)(rect.height / 2);
			const char* text = TextFormat("%d", moves);
			int textWidth = MeasureText(text, fontSize);
			DrawText(
				text,
				rect.x + rect.width / 2 - textWidth / 2,
				rect.y + rect.height / 2 - fontSize / 2,
				fontSize, GRAY
			);
		}
	}
}

int InteractiveOverlay::GetClickedOption(const Vector2& mouse_world, const Grid& grid) const
{
	if (!visible) return -1;
	for (int i = 0; i < (int)options.size(); i++) {
		if (options[i].action.type != Action::Type::NONE && CheckCollisionPointRec(mouse_world, OptionRec(i, grid)))
			return i;
	}
	std::cout << "No option clicked at: " << std::to_string(mouse_world.x) << ", " << std::to_string(mouse_world.y) << std::endl;
	return -1;
}

const Action InteractiveOverlay::GetIndexAction(int index) const
{
	if (index == -1) return { Action::Type::NONE, {0, 0}, 0 };
	return options[index].action;
}

Rectangle InteractiveOverlay::OptionRec(int i, const Grid& grid) const {
	const float ts    = (float)grid.tile_size;
	const float size  = ts / 1;
	const float total = options.size() * size;
	return { anchor_pos.x + ts / 2.0f - total / 2.0f + i * size, anchor_pos.y, size, size };
}
