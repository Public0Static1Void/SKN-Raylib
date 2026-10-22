#pragma once
#include "Tween.h"
#include <raylib.h>
#include <cmath>

class Animator {
public:
	void MoveTo(Vector2 from, Vector2 to, float duration, EaseFn fn, bool returns = false, std::function<void()> end_event = nullptr) {
		x.Begin(from.x, to.x, duration, fn);
		y.Begin(from.y, to.y, duration, fn);
		x.on_end_animation = std::move(end_event);
		x.returns_to_start = returns;
		y.returns_to_start = returns;
	}
	void ScaleTo(Vector2 from, Vector2 to, float duration, EaseFn fn, bool returns = false, std::function<void()> end_event = nullptr) {
		w.Begin(from.x, to.x, duration, fn);
		h.Begin(from.y, to.y, duration, fn);
		w.on_end_animation = std::move(end_event);
		w.returns_to_start = returns;
		h.returns_to_start = returns;
	}
	void RotateTo(float from, float to, float duration, EaseFn fn, bool returns = false, std::function<void()> end_event = nullptr) {
		rot.Begin(from, to, duration, fn);
		rot.on_end_animation = std::move(end_event);
		rot.returns_to_start = returns;
	}

	bool IsMoving()		const { return x.active || y.active; }
	bool IsScaling()	const { return w.active || h.active; }
	bool IsRotating()	const { return rot.active; }
	bool IsAnimating()	const { return IsMoving() || IsScaling() || IsRotating(); }

	template <class TransformT>
	bool Update(float deltaTime, TransformT& tr) {
		bool wasAnimating = IsAnimating();
		if (x.active) tr.position.x = x.Update(deltaTime);
		if (y.active) tr.position.y = y.Update(deltaTime);
		if (w.active) tr.size.x = w.Update(deltaTime);
		if (h.active) tr.size.y = h.Update(deltaTime);
		if (rot.active) {
			float rt = rot.Update(deltaTime);
			if (!rot.active) rt = fmodf(fmodf(rt, 360.0f) + 360.0f, 360.0f);
			tr.rotation = rt;
		}
		return wasAnimating && !IsAnimating();
	}

private:
	Tween x, y, w, h, rot;
};