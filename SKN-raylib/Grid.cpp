#include "Grid.h"
#include <fstream>
#include <string>
#include <raylib.h>
#include "Assets.h"

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
	}

	// Get the elements start positions
	for (int i = 0; i < tiles.size(); i++) {
		string line = tiles[i];
		for (int j = 0; j < line.size(); j++) {
			switch (line[j]) {
			case 'P':
				objects_start_positions.push_back(
					ObjectPosition{ Vector2{(float)j, (float)i}, EntityType::PLAYER }
				);
				break;
			case 'E':
				objects_start_positions.push_back(
					ObjectPosition{ Vector2{(float)j, (float)i}, EntityType::ENEMY }
				);
				break;
			case 'C':
				objects_start_positions.push_back(
					ObjectPosition{ Vector2{(float)j, (float)i}, EntityType::CANNON }
				);
				break;
			}
		}
	}

	int line_size = tiles[0].size();
	if (start_draw_position.x > 0)
		start_draw_position.x = start_draw_position.x - (line_size) * tile_size;
	if (start_draw_position.y > 0)
		start_draw_position.y = start_draw_position.y - (line_size) * tile_size;

	return true;
}

void Grid::DrawGrid(const Assets& assets)
{
	int size = tiles.size();
	if (size <= 0) return;

	int line_size = tiles[0].size();
	int start_x = start_draw_position.x;
	int start_y = start_draw_position.y;

	for (int i = 0; i < size; i++) {
		std::string line = tiles[i];
		for (int j = 0; j < line.size(); j++) {
			Sprite spr = Sprite::LAST_NO_USE;
			switch (line[j]) {
			case '1': 
				spr = Sprite::WALL; break;
			default:
				spr = Sprite::FLOOR; break;
			}
			Rectangle dest = {
				(float)(start_x + j * tile_size), (float)(start_y + i * tile_size),
				(float)tile_size, (float)tile_size
			};
			DrawSprite(assets.tilemap, spr, dest);
		}
		
	}

	for (auto& p : path) {
		DrawCircle(p.x + start_draw_position.x, p.y + start_draw_position.y, 1, YELLOW);
	}
}