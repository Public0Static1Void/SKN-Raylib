#pragma once
#include <vector>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "Object.h"
#include "Grid.h"
#include "Assets.h"
namespace ObjectRenderer {
	inline void DrawObject(const Object& ob, const Assets& assets, const Grid& grid) {
		if (!ob.GetLifeSystem()->IsAlive() || !ob.GetCanRender()) return;

		const ObjectTransform& transform = ob.objectTransform;
		Rectangle dest = {
			transform.position.x + (grid.tile_size - transform.size.x) / 2.0f,
			transform.position.y + (grid.tile_size - transform.size.y) / 2.0f,
			transform.size.x, transform.size.y
		};

		DrawSprite(assets.tilemap, ob.GetSprite(), dest, (float)transform.rotation);
	}

	inline void DrawAllObjects(std::vector<std::unique_ptr<Object>>& objects, const Assets& assets, const Grid& grid) {
		for (auto& ob : objects)
			DrawObject(*ob, assets, grid);
	}
}