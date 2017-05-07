#ifndef GRID_H
#define GRID_H

#include "Shapes.h"
#include "GameState.h"

#include <vector>

namespace Temporal
{
	class Vector;
	class Fixture;
	class DirectedSegment;

	typedef std::vector<const Fixture*> FixtureList;
	typedef FixtureList::const_iterator FixtureIterator;

	class RayCastResult
	{
	public:
		RayCastResult() : _fixture(0), _directedSegment(DirectedSegment::Zero) {}

		const Fixture& getFixture() const { return *_fixture; }
		void setFixture(const Fixture* fixture) { _fixture = fixture; }
		const DirectedSegment& getDirectedSegment() const { return _directedSegment; }
		void setDirectedSegment(const DirectedSegment& directedSegment) { _directedSegment = directedSegment; }

	private:
		const Fixture* _fixture;
		DirectedSegment _directedSegment;
	};

	class Grid : public GameStateComponent
	{
	public:
		Grid() : _grid(0), _worldSize(Vector::Zero), _tileSize(0.0f), _gridWidth(0), _gridHeight(0) {};
		~Grid();

		virtual void init(GameState* gameState);

		const Vector& getWorldSize() const { return _worldSize; }
		float getTileSize() const { return _tileSize; }

		void add(const Fixture* body);
		void update(Fixture* body);
		void remove(Fixture* body);

		bool cast(const Vector& rayOrigin, const Vector& rayDirection, RayCastResult& result, int mask = -1, int group = -1) const;
		bool iterateTiles(const OBB& shape, int mask = -1, int group = -1, FixtureList* result = 0, bool checkIntersection = true, bool isIntersectsExclusive = false) const;

		void draw() const;

	private:
		FixtureList** _grid;
		Vector _worldSize;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		float getTileAxisCenter(int index) const { return index * _tileSize + _tileSize / 2.0f; }
		OBB getTileShape(int i, int j) const;
		int getAxisIndex(float value) const { return static_cast<int>(value / _tileSize); }
		int getIndex(int i, int j) const { return i + j * _gridWidth; }
		int getSize() const { return _gridWidth * _gridHeight; }

		void add(const Fixture* body, int i, int j);
		FixtureList* getTile(int i, int j) const;

		Grid(const Grid&);
		Grid& operator=(const Grid&);

		friend class SerializationAccess;
	};
}

#endif