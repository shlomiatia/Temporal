#ifndef GRID_H
#define GRID_H

#include "NumericPair.h"
#include <vector>

namespace Temporal
{
	class Shape;
	class CollisionInfo;
	class DirectedSegment;

	typedef std::vector<const CollisionInfo*> CollisionInfoCollection;
	typedef CollisionInfoCollection::const_iterator CollisionInfoIterator;

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

		void add(const CollisionInfo* body);
		CollisionInfoCollection iterateTiles(const Shape& shape, int collisionFilter) const;

		void draw() const;

	private:
		CollisionInfoCollection** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		Point getTileCenter(int i, int j) const { return  Point(getTileAxisCenter(i), getTileAxisCenter(j)); }
		float getTileSize() const { return _tileSize; }
		int getAxisIndex(float value) const { return static_cast<int>(value / _tileSize); }
		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		int getIndex(int i, int j) const { return i + j * _gridWidth; }
		int getSize() const { return _gridWidth * _gridHeight; }

		CollisionInfoCollection* getTile(int i, int j) const;
		CollisionInfoCollection* getTile(int index) const;

		Grid() {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}

#endif