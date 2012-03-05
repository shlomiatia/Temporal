#pragma once

#include <math.h>
#include <vector>
#include <Temporal\Base\Base.h>
#include "StaticBody.h"

namespace Temporal
{
	class StaticBodiesIndex
	{
	public:
		static StaticBodiesIndex& get(void)
		{
			static StaticBodiesIndex instance;
			return instance;
		}

		void init(const Vector& worldSize, float tileSize);

		float getTileSize(void) const { return _tileSize; }
		int getAxisIndex(float value) const { return (int)(value / _tileSize); }
		Vector getTileCenter(int i, int j) const { return  Vector(getTileAxisCenter(i), getTileAxisCenter(j)); }
		
		void add(StaticBody* staticBody);
		std::vector<StaticBody*>* get(int x, int y) const;
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int i, int j)) const;
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const;

		void draw(void) const;

	private:
		std::vector<StaticBody*>** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		int getIndex(int i, int j) const { return i * _gridWidth + j; }
		int getGridSize(void) const { return _gridWidth * _gridHeight; }

		static bool iterateStaticBodies(void* caller, void* data, int i, int j);
		static bool add(void* caller, void* data, int i, int j);

		StaticBodiesIndex(void) {};
		StaticBodiesIndex(const StaticBodiesIndex&);
		StaticBodiesIndex& operator=(const StaticBodiesIndex&);
	};
}

