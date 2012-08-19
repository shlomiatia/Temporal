#ifndef GRID_H
#define GRID_H

#include "Shapes.h"

#include <vector>

namespace Temporal
{
	class Vector;
	class Vector;
	class Fixture;
	class DirectedSegment;

	typedef std::vector<const Fixture*> FixtureCollection;
	typedef FixtureCollection::const_iterator FixtureIterator;

	class RayCastResult
	{
	public:
		RayCastResult() : _fixture(NULL), _point(Vector::Zero) {}

		const Fixture& getFixture() const { return *_fixture; }
		void setFixture(const Fixture* fixture) { _fixture = fixture; }
		const Vector& getPoint() const { return _point; }
		void SetPoint(const Vector& point) { _point = point; }

	private:
		const Fixture* _fixture;
		Vector _point;
	};

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

		void add(const Fixture* body);
		void update(const Shape& previous, const Fixture* body);

		bool cast(const Vector& rayOrigin, const Vector& rayDirection, RayCastResult& result, int mask = -1, int group = -1) const;
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

		void add(const Fixture* body, int i, int j);
		FixtureCollection* getTile(int i, int j) const;

		Grid() {};
		Grid(const Grid&);
		Grid& operator=(const Grid&);
	};
}

#endif