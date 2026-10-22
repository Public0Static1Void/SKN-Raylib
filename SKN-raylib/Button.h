#pragma once
#include <raylib.h>
#include <functional>

class Button {
public:
	enum class Anchor { TOP_LEFT, TOP_RIGHT };

	Button() = default;
	Button(Anchor anchor, Vector2 size, float margin)
		: anchor(anchor), size(size), margin(margin) {}

	bool IsHovered() const {
		return CheckCollisionPointRec(GetMousePosition(), GetButtonRect());
	}
	void Update() {
		if (on_click && IsHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			on_click();
	}

	Rectangle GetButtonRect() const {
		return AnchoredRect(anchor, size, margin);
	}

	inline Rectangle AnchoredRect(Anchor a, Vector2 size, float margin) const {
		const float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
		switch (a) {
		case Anchor::TOP_LEFT:     return { margin, margin, size.x, size.y };
		case Anchor::TOP_RIGHT:    return { sw - size.x - margin, margin, size.x, size.y };
		}
		return {};
	}

	std::function<void()> on_click;

private:
	Anchor anchor = Anchor::TOP_RIGHT;

	Vector2 size{ 0, 0 };
	float margin = 0;
};