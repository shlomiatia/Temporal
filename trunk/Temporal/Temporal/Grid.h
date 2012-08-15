#ifndef GRID_H
#define GRID_H

#include "Shapes.h"

#include <vector>

namespace Temporal
{
	class Fixture;
	class DirectedSegment;

	typedef std::vector<Fixture*> FixtureCollection;
	typedef FixtureCollection::const_iterator FixtureIterator;

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

		bool cast(const Point& rayOrigin, const Vector& rayDirection, Point& pointOfIntersection, int mask = -1, int group = -1);
		bool cast(const DirectedSegment& dirSeg, Point& pointOfIntersection, int mask = -1, int group = -1);

		void add(Fixture* body);
		void update(const Shape& previous, Fixture* body);
		FixtureCollection iterateTiles(const Shape& shape, int mask = -1, int group = -1) const;

		void draw() const;

	private:
		FixtureCollection** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		AABB getTileAABB(int i, int j) const;
		int getAxisIndex(float value) const { return static_cast<int>(value / _tileSize); }
		int getIndex(int i, int j) const { return i + j * _gridWidth; }
		int getSize() const { return _gridWidth * _gridHeight; }

		void add(Fixture* body, int i, int j);
		FixtureCollection* getTile(int i, int j) const;

		Grid() {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}

#endif