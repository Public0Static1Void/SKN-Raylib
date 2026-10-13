#include "Grid.h"
#include <fstream>
#include <string>

using namespace std;

bool Grid::ScanLevel(const char* fileName)
{
	string path = string("Levels/") + fileName;
	ifstream file(path);
	if (!file.is_open()) {
		cerr << "Could not open level: " << fileName << " at path: " << path << endl;
		return false;
	}

	tiles.clear();
	string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		if (line[0] == '#') continue;
		tiles.push_back(line);
		cout << line;
	}

	return true;
}
