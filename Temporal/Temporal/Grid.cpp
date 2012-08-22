#include "Grid.h"
#include "Shapes.h"
#include "Graphics.h"
#include "ShapeOperations.h"
#include "Fixture.h"
#include "Vector.h"
#include "CollisionFilter.h"
#include <algorithm>

namespace Temporal
{
	AABB Grid::getTileAABB(int i, int j) const
	{
		return AABB(Vector(getTileAxisCenter(i), getTileAxisCenter(j)), Size(_tileSize, _tileSize));
	}

	void Grid::init(const Size& worldSize, float tileSize)
	{
		_tileSize = tileSize;
		_gridWidth = getAxisIndex(worldSize.getWidth()) + 1;
		_gridHeight = getAxisIndex(worldSize.getHeight()) + 1;
		
		int size = getSize();
		_grid = new FixtureCollection*[size];
		for(int i = 0; i < size; ++i)
			_grid[i] = NULL;
	}

	void Grid::dispose()
	{
		int size = getSize();
		for(int i = 0; i < size; ++i)
			if(_grid[i] != NULL)
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
				if(fixtures != NULL && fixtures->size() != 0)
				{
					Graphics::get().draw(getTileAABB(i, j), Color(0.0f, 0.0f, 1.0f, 0.3f));
				}
			}
		}
	}

	void Grid::add(const Fixture* body, int i, int j)
	{
		int index = getIndex(i, j);
		FixtureCollection* bodies = getTile(i, j);
		if(bodies == NULL)
		{
			bodies = new FixtureCollection();
			_grid[index] = bodies;
		}
		bodies->push_back(body);
	}

	void Grid::add(const Fixture* body)
	{
		const Shape& shape = body->getGlobalShape();
		int leftIndex = getAxisIndex(shape.getLeft());
		int rightIndex = getAxisIndex(shape.getRight());
		int topIndex = getAxisIndex(shape.getTop());
		int bottomIndex = getAxisIndex(shape.getBottom());

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				AABB tile = getTileAABB(i, j);
				if(intersects(tile, body->getGlobalShape()))
					add(body, i, j);
			}
		}
	}

	void Grid::update(const Shape& previous, const Fixture* body)
	{
		const Shape& shape = body->getGlobalShape();
		int leftIndex = getAxisIndex(shape.getLeft());
		int rightIndex = getAxisIndex(shape.getRight());
		int topIndex = getAxisIndex(shape.getTop());
		int bottomIndex = getAxisIndex(shape.getBottom());

		int leftRemoveIndex = getAxisIndex(previous.getLeft());
		int rightRemoveIndex = getAxisIndex(previous.getRight());
		int topRemoveIndex = getAxisIndex(previous.getTop());
		int bottomRemoveIndex = getAxisIndex(previous.getBottom());

		for(int i = leftRemoveIndex; i <= rightRemoveIndex; ++i)
		{
			for(int j = bottomRemoveIndex; j <= topRemoveIndex; ++j)
			{
				if(i >= leftIndex && i <= rightIndex && j >= bottomIndex && j <= topIndex)
					continue;
				FixtureCollection* bodies = getTile(i, j);
				if(bodies == NULL)
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
			return NULL;
		else
			return _grid[index];
	}

	bool Grid::cast(const Vector& rayOrigin, const Vector& rayDirection, RayCastResult& result, int mask, int group) const
	{
		float maxSize = std::max(_gridWidth * _tileSize, _gridHeight * _tileSize);
		DirectedSegment ray = DirectedSegment(rayOrigin, maxSize * rayDirection);
		const Vector& origin = ray.getOrigin();
		const Vector& destination = ray.getTarget();
		float x1 = origin.getX();
		float y1 = origin.getY();
		float x2 = destination.getX();
		float y2 = destination.getY();
		
		int i = Grid::get().getAxisIndex(x1);
		int j = Grid::get().getAxisIndex(y1);

		// Determine end grid cell coordinates (iend, jend)
		int iend = Grid::get().getAxisIndex(x2);
		int jend = Grid::get().getAxisIndex(y2);

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

		float minDistance = ray.getVector().getLength();

		// Main loop. Visits cells until last cell reached
		while(true)
		{
			FixtureCollection* bodies = getTile(i, j);
			AABB tile = getTileAABB(i, j);
			if(bodies != NULL)
			{
				float distance = 0;
				Vector pointOfIntersection = Vector::Zero;
				for(FixtureIterator iterator = bodies->begin(); iterator != bodies->end(); ++iterator)
				{
					const Fixture& body = **iterator;
					if(body.getFilter().canCollide(mask, group) &&
					   intersects(ray, body.getGlobalShape(), &pointOfIntersection, &distance) &&
					   distance < minDistance &&
					   tile.contains(pointOfIntersection))
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

	FixtureCollection Grid::iterateTiles(const Shape& shape, int mask, int group) const
	{
		int leftIndex = getAxisIndex(shape.getLeft());
		int rightIndex = getAxisIndex(shape.getRight());
		int topIndex = getAxisIndex(shape.getTop());
		int bottomIndex = getAxisIndex(shape.getBottom());

		FixtureCollection result;

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				FixtureCollection* bodies = getTile(i, j);
				if(bodies != NULL)
				{
					for(FixtureIterator i = bodies->begin(); i != bodies->end(); ++i)
					{
						const Fixture* body = *i;
						if(body->getFilter().canCollide(mask, group))
							result.push_back(body);
					}
				}
			}
		}

		return result;
	}
}