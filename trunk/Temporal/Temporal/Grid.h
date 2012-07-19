#ifndef GRID_H
#define GRID_H

#include "NumericPair.h"
#include <vector>

namespace Temporal
{
	class Shape;
	class StaticBody;
	class DirectedSegment;

	typedef std::vector<const StaticBody*> StaticBodyCollection;
	typedef StaticBodyCollection::const_iterator StaticBodyIterator;

	class Grid
	{
	public:
		static Grid& get()
		{
			static Grid instance;
			return instance;
		}

		void init(const Size& worldSize, float tileSize);
		void dispose();

		bool cast(const Point& rayOrigin, const Vector& rayDirection, int collisionFilter, Point& pointOfIntersection);
		bool cast(const DirectedSegment& dirSeg, int collisionFilter, Point& pointOfIntersection);

		void add(const StaticBody* staticBody);
		void iterateTiles(const Shape& shape, int collisionFilter, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const;

		void draw() const;

	private:
		StaticBodyCollection** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		Point getTileCenter(int i, int j) const { return  Point(getTileAxisCenter(i), getTileAxisCenter(j)); }
		float getTileSize() const { return _tileSize; }
		int getAxisIndex(float value) const { return static_cast<int>(value / _tileSize); }
		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		int getIndex(int i, int j) const { return i + j * _gridWidth; }
		int getSize() const { return _gridWidth * _gridHeight; }

		StaticBodyCollection* getTile(int i, int j) const;
		StaticBodyCollection* getTile(int index) const;

		Grid() {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}

#endif