#pragma once
#include <algorithm>
#include <functional>

using EaseFn = float(*)(float);

namespace Easing {
	inline float Linear(float t) { return t; }
	inline float OutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
	inline float OutBack(float t) {
		const float c1 = 1.70158f, c3 = c1 + 1.0f;
		float u = t - 1.0f;
		return 1.0f + c3 * u * u * u + c1 * u * u;
	}
}

struct Tween {
	float start = 0, end = 0;
	float duration = 1, timer = 0;
	EaseFn ease = Easing::OutQuad;
	bool active = false;
	bool returns_to_start = false;
	std::function<void()> on_end_animation;

	void Begin(float from, float to, float dur, EaseFn fn, bool returns = false) {
		start = from;
		end = to;
		duration = dur;
		timer = 0;
		ease = fn;
		returns_to_start = returns;
		on_end_animation = nullptr;

		active = true;
	}

	float Update(float deltaTime) {
		timer += deltaTime;
		float t = timer / duration;
		if (t >= 1.0f) {
			active = false;
			float final_value = returns_to_start ? start : end;
			if (on_end_animation) {
				auto stored = std::move(on_end_animation);
				on_end_animation = nullptr;
				stored();
			}
		}
		t = ease(t);
		if (returns_to_start) t = 4.0f * t * (1.0f - t);
		return start + (end - start) * t;
	}
};