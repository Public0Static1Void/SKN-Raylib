#pragma once
#include <iterator>

enum class LevelGoal { HIT_TARGET, KILL_ENEMIES };
struct LevelInfo { const char* fileName; LevelGoal goal; };

inline constexpr LevelInfo LEVELS_INFO[] = {
	{ "Level1.txt", LevelGoal::HIT_TARGET },
	{ "Level2.txt", LevelGoal::KILL_ENEMIES },
};
inline constexpr int LEVEL_COUNT = (int)std::size(LEVELS_INFO);