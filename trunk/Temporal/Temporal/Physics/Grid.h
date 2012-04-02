#ifndef GRID_H
#define GRID_H

#include <Temporal\Base\NumericPair.h>
#include <vector>

namespace Temporal
{
	class Rect;
	class StaticBody;

	typedef std::vector<const StaticBody*> StaticBodyCollection;
	typedef StaticBodyCollection::const_iterator StaticBodyIterator;

	class Grid
	{
	public:
		static Grid& get(void)
		{
			static Grid instance;
			return instance;
		}

		void init(const Size& worldSize, float tileSize);
		void dispose(void);

		int getAxisIndex(float value) const { return (int)(value / _tileSize); }
		float getTileSize(void) const { return _tileSize; }
		Point getTileCenter(int i, int j) const { return  Point(getTileAxisCenter(i), getTileAxisCenter(j)); }
		StaticBodyCollection* getTile(int i, int j) const;

		void add(const StaticBody* staticBody);
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int index)) const;
		void iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const;

		void draw(void) const;

	private:
		StaticBodyCollection** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		int getIndex(int i, int j) const { return i + j * _gridWidth; }
		int getSize(void) const { return _gridWidth * _gridHeight; }
		StaticBodyCollection* getTile(int index) const;

		static bool iterateStaticBodies(void* caller, void* data, int index);
		static bool add(void* caller, void* data, int index);

		Grid(void) {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}

#endif