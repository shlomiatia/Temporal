#include "Grid.h"
#include "Shapes.h"
#include "Graphics.h"
#include "ShapeOperations.h"
#include "Fixture.h"
#include "Vector.h"
#include "CollisionFilter.h"
#include "Math.h"
#include <algorithm>

namespace Temporal
{
	OBB Grid::getTileShape(int i, int j) const
	{
		return OBBAABB(Vector(getTileAxisCenter(i), getTileAxisCenter(j)), Vector(_tileSize, _tileSize) / 2.0f);
	}

	void Grid::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);
		_gridWidth = getAxisIndex(getWorldSize().getX()) + 1;
		_gridHeight = getAxisIndex(getWorldSize().getY()) + 1;
		
		int size = getSize();
		_grid = new FixtureCollection*[size];
		for(int i = 0; i < size; ++i)
			_grid[i] = 0;
	}

	Grid::~Grid()
	{
		int size = getSize();
		for(int i = 0; i < size; ++i)
			if(_grid[i])
				delete _grid[i];
		delete _grid;
	}

	void Grid::draw() const
	{
		for(int i = 0; i < _gridWidth; ++i)
		{
			for(int j = 0; j < _gridHeight; ++j)
			{
				FixtureCollection* fixtures = getTile(i, j);
				if(fixtures && fixtures->size() != 0)
				{
					Graphics::get().getSpriteBatch().add(getTileShape(i, j), Color(0.0f, 0.0f, 1.0f, 0.3f));
				}
			}
		}
	}

	void Grid::add(const Fixture* body, int i, int j)
	{
		int index = getIndex(i, j);
		if(index < 0 || index >= getSize())
			return;
		FixtureCollection* bodies = getTile(i, j);
		if(!bodies)
		{
			bodies = new FixtureCollection();
			_grid[index] = bodies;
		}
		bodies->push_back(body);
	}

	void Grid::add(const Fixture* body)
	{
		const OBB& shape = body->getGlobalShape();
		OBBAABBWrapper aabb = shape.getAABBWrapper();
		int leftIndex = getAxisIndex(aabb.getLeft());
		int rightIndex = getAxisIndex(aabb.getRight());
		int topIndex = getAxisIndex(aabb.getTop());
		int bottomIndex = getAxisIndex(aabb.getBottom());	

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				OBB tile = getTileShape(i, j);
				if(intersects(tile, body->getGlobalShape()))
					add(body, i, j);
			}
		}
	}

	void Grid::update(Fixture* body)
	{
		const OBB& previous = body->getGlobalShape();
		OBBAABBWrapper previousAABB = previous.getAABBWrapper();
		int leftRemoveIndex = getAxisIndex(previousAABB.getLeft());
		int rightRemoveIndex = getAxisIndex(previousAABB.getRight());
		int topRemoveIndex = getAxisIndex(previousAABB.getTop());
		int bottomRemoveIndex = getAxisIndex(previousAABB.getBottom());

		body->update();
		const OBB& current = body->getGlobalShape();
		OBBAABBWrapper currentAABB = current.getAABBWrapper();
		int leftIndex = getAxisIndex(currentAABB.getLeft());
		int rightIndex = getAxisIndex(currentAABB.getRight());
		int topIndex = getAxisIndex(currentAABB.getTop());
		int bottomIndex = getAxisIndex(currentAABB.getBottom());	

		for(int i = leftRemoveIndex; i <= rightRemoveIndex; ++i)
		{
			for(int j = bottomRemoveIndex; j <= topRemoveIndex; ++j)
			{
				if(i >= leftIndex && i <= rightIndex && j >= bottomIndex && j <= topIndex)
					continue;
				FixtureCollection* bodies = getTile(i, j);
				if(!bodies)
					continue;
				for(FixtureIterator iterator = bodies->begin(); iterator != bodies->end(); ++iterator)
				{
					if((**iterator).getEntityId() == body->getEntityId())
					{
						bodies->erase(iterator);
						break;
					}
				}
			}
		}
		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				if(i >= leftRemoveIndex && i <= rightRemoveIndex && j >= bottomRemoveIndex && j <= topRemoveIndex)
					continue;
				add(body, i, j);
			}
		}

	}

	FixtureCollection* Grid::getTile(int i, int j) const
	{
		int index = getIndex(i, j);
		if(index < 0 || index >= getSize())
			return 0;
		else
			return _grid[index];
	}

	bool Grid::cast(const Vector& rayOrigin, const Vector& rayDirection, RayCastResult& result, int mask, int group) const
	{
		if(rayDirection == Vector::Zero)
			return false;
		float maxSize = std::max(_gridWidth * _tileSize, _gridHeight * _tileSize);
		DirectedSegment ray = DirectedSegment(rayOrigin, rayDirection * maxSize);
		const Vector& origin = ray.getOrigin();
		const Vector& destination = ray.getOrigin() + ray.getDirection() * maxSize;
		float x1 = origin.getX();
		float y1 = origin.getY();
		float x2 = destination.getX();
		float y2 = destination.getY();
		
		int i = getAxisIndex(x1);
		int j = getAxisIndex(y1);

		// Determine end grid cell coordinates (iend, jend)
		int iend = getAxisIndex(x2);
		int jend = getAxisIndex(y2);

		// Determine in which primary direction to step
		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));

		// Determine tx and ty, the values of t at which the directed segment
		// (x1,y1)-(x2,y2) crosses the first horizontal and vertical cell
		// boundaries, respectively. Min(tx, ty) indicates how far one can
		// travel along the segment and still remain in the current cell
		float minx = _tileSize * floorf(x1/_tileSize), maxx = minx + _tileSize;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1)) / abs(x2 - x1);
		float miny = _tileSize * floorf(y1/_tileSize), maxy = miny + _tileSize;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1)) / abs(y2 - y1);

		// Determine deltax/deltay, how far (in units of t) one must step
		// along the directed line segment for the horizontal/vertical
		// movement (respectively) to equal the width/height of a cell
		float deltatx = _tileSize / abs(x2 - x1);
		float deltaty = _tileSize / abs(y2 - y1);

		float minDistance = maxSize;

		// Main loop. Visits cells until last cell reached
		while(true)
		{
			FixtureCollection* bodies = getTile(i, j);
			OBB tile = getTileShape(i, j);
			if(bodies)
			{
				float distance = 0;
				Vector pointOfIntersection = Vector::Zero;
				for(FixtureIterator iterator = bodies->begin(); iterator != bodies->end(); ++iterator)
				{
					const Fixture& body = **iterator;
					if(body.getFilter().canCollide(mask, group) &&
					   intersects(ray, body.getGlobalShape(), &pointOfIntersection, &distance) &&
					   distance < minDistance &&
					   intersects(tile, pointOfIntersection))
					{
						result.setFixture(&body);
						result.SetPoint(pointOfIntersection);
						minDistance = distance;
					}
				}
				if(result.getPoint() != Vector::Zero)
				{
					return true;
				}
			}
			if (tx <= ty) 
			{ // tx smallest, step in x
				if (i == iend) break;
				tx += deltatx;
				i += di;
			}
			else
			{ // ty smallest, step in y
				if (j == jend) break;
				ty += deltaty;
				j += dj;
			}
		}
		return false;
	}

	FixtureCollection Grid::iterateTiles(const OBB& shape, int mask, int group, bool checkIntersection) const
	{
		OBBAABBWrapper shapeAABB = shape.getAABBWrapper();
		int leftIndex = getAxisIndex(shapeAABB.getLeft());
		int rightIndex = getAxisIndex(shapeAABB.getRight());
		int topIndex = getAxisIndex(shapeAABB.getTop());
		int bottomIndex = getAxisIndex(shapeAABB.getBottom());

		FixtureCollection result;

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				FixtureCollection* bodies = getTile(i, j);
				if(bodies)
				{
					for(FixtureIterator i = bodies->begin(); i != bodies->end(); ++i)
					{
						const Fixture* body = *i;
						if(body->getFilter().canCollide(mask, group) && (!checkIntersection || intersects(shape, body->getGlobalShape())))
							result.push_back(body);
					}
				}
			}
		}

		return result;
	}
}