#pragma once
#include "Assets.h"
#include "Object.h"
#include <string>
#include <vector>


struct MenuOption {
	std::string name = "";
	Action action;
	Sprite icon = Sprite::UI_BASE;
	bool render_moves = false;
};

class InteractiveOverlay {
public:
	void DrawOverlay(const Assets& assets, const Grid& grid, const Object* ob) const;
	int GetClickedOption(const Vector2& mouse_world, const Grid& grid) const;
	const Action GetIndexAction(int index) const;

	const bool IsVisible() const { return visible; }
	void SetVisible(bool v) { visible = v; }

	void SetAnchor(const Vector2& pos) { anchor_pos = pos; }
private:
	std::vector<MenuOption> options = {
	{ "Moves left",	{Action::Type::NONE, {0, 0}, 0 },		Sprite::UI_BASE,	true },
	{ "Move",		{Action::Type::MOVE, {0, 0}, 0 },		Sprite::UI_MOVE,	false },
	{ "Rotate",		{Action::Type::ROTATE, {1, 0}, 90 },	Sprite::UI_ROTATE,	false }
	};

	Rectangle OptionRec(int i, const Grid& grid) const;

	bool visible = false;

	Vector2 anchor_pos{0, 0};
};