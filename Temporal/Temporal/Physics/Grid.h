#pragma once

#include <math.h>
#include <vector>

#include "StaticBody.h"

namespace Temporal
{
	// TODO: Make this a component as well
	class Grid
	{
	public:
		static Grid& get(void)
		{
			static Grid instance;
			return instance;
		}

		void init(const Vector& worldSize, float tileSize);
		void dispose(void);

		int getAxisIndex(float value) const { return (int)(value / _tileSize); }
		float getTileSize(void) const { return _tileSize; }
		Vector getTileCenter(int i, int j) const { return  Vector(getTileAxisCenter(i), getTileAxisCenter(j)); }
		std::vector<const StaticBody* const>* getTile(int x, int y) const;

		void add(const StaticBody* const staticBody);
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int i, int j)) const;
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const;

		void draw(void) const;

	private:
		std::vector<const StaticBody* const>** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		int getIndex(int i, int j) const { return i * _gridWidth + j; }
		int getSize(void) const { return _gridWidth * _gridHeight; }

		static bool iterateStaticBodies(void* caller, void* data, int i, int j);
		static bool add(void* caller, void* data, int i, int j);

		Grid(void) {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}

