#pragma once
#include <iostream>
#include <vector>
class Grid
{
public:
	bool ScanLevel(const char* fileName);

	std::vector<std::string> tiles;
};