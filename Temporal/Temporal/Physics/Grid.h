#ifndef GRID_H
#define GRID_H

#include <Temporal\Base\Vector.h>
#include <vector>

namespace Temporal
{
	class Rect;
	class StaticBody;

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
		std::vector<const StaticBody*>* getTile(int i, int j) const;

		void add(const StaticBody* staticBody);
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int index)) const;
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const;

		void draw(void) const;

	private:
		std::vector<const StaticBody*>** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		int getIndex(int i, int j) const { return i + j * _gridWidth; }
		int getSize(void) const { return _gridWidth * _gridHeight; }
		std::vector<const StaticBody*>* getTile(int index) const;

		static bool iterateStaticBodies(void* caller, void* data, int index);
		static bool add(void* caller, void* data, int index);

		Grid(void) {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}


#endif