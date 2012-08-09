#include "Grid.h"
#include "Shapes.h"
#include "Graphics.h"
#include "ShapeOperations.h"
#include "CollisionInfo.h"
#include <algorithm>

namespace Temporal
{
	void Grid::init(const Size& worldSize, float tileSize)
	{
		_tileSize = tileSize;
		_gridWidth = getAxisIndex(worldSize.getWidth());
		_gridHeight = getAxisIndex(worldSize.getHeight());
		
		int size = getSize();
		_grid = new CollisionInfoCollection*[size];
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
			for(int j = 0; j < _gridHeight; ++j)
				if(getTile(i, j) != NULL)
					Graphics::get().draw(AABB(getTileCenter(i, j), Size(_tileSize, _tileSize)), Color(0.0f, 0.0f, 1.0f, 0.3f));
	}


	void Grid::add(CollisionInfo* body)
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
				int index = getIndex(i, j);
				CollisionInfoCollection* bodies = getTile(index);
				if(bodies == NULL)
				{
					bodies = new CollisionInfoCollection();
					_grid[index] = bodies;
				}
				bodies->push_back(body);
			}
		}
	}

	CollisionInfoCollection* Grid::getTile(int i, int j) const
	{
		int index = getIndex(i, j);
		return getTile(index);
	}

	CollisionInfoCollection* Grid::getTile(int index) const
	{
		if(index < 0 || index >= getSize())
			return NULL;
		else
			return _grid[index];
	}

	bool Grid::cast(const Point& rayOrigin, const Vector& rayDirection, Point& pointOfIntersection, int mask1, int mask2)
	{
		float maxSize = std::max(_gridWidth * _tileSize, _gridHeight * _tileSize);
		DirectedSegment ray = DirectedSegment(rayOrigin, maxSize * rayDirection);
		return cast(ray, pointOfIntersection, mask1, mask2);
	}

	bool Grid::cast(const DirectedSegment& dirSeg, Point& pointOfIntersection, int mask1, int mask2)
	{
		const Point& origin = dirSeg.getOrigin();
		const Point& destination = dirSeg.getTarget();
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

		const float tileSize = Grid::get().getTileSize();

		// Determine tx and ty, the values of t at which the directed segment
		// (x1,y1)-(x2,y2) crosses the first horizontal and vertical cell
		// boundaries, respectively. Min(tx, ty) indicates how far one can
		// travel along the segment and still remain in the current cell
		float minx = tileSize * floorf(x1/tileSize), maxx = minx + tileSize;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1)) / abs(x2 - x1);
		float miny = tileSize * floorf(y1/tileSize), maxy = miny + tileSize;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1)) / abs(y2 - y1);

		// Determine deltax/deltay, how far (in units of t) one must step
		// along the directed line segment for the horizontal/vertical
		// movement (respectively) to equal the width/height of a cell
		float deltatx = tileSize / abs(x2 - x1);
		float deltaty = tileSize / abs(y2 - y1);

		pointOfIntersection = destination;
		// Main loop. Visits cells until last cell reached
		while(true)
		{
			CollisionInfoCollection* bodies = getTile(i, j);
			if(bodies != NULL)
			{
				for(CollisionInfoIterator iterator = bodies->begin(); iterator != bodies->end(); ++iterator)
				{
					CollisionInfo& body = **iterator;
					if(body.canCollide(mask1, mask2) && intersects(dirSeg, body.getGlobalShape(), &pointOfIntersection))
					{
						return false;
					}
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
			
		return true;
	}

	CollisionInfoCollection Grid::iterateTiles(const Shape& shape, int mask1, int mask2) const
	{
		int leftIndex = getAxisIndex(shape.getLeft());
		int rightIndex = getAxisIndex(shape.getRight());
		int topIndex = getAxisIndex(shape.getTop());
		int bottomIndex = getAxisIndex(shape.getBottom());

		CollisionInfoCollection result;

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				int index = getIndex(i, j);
				CollisionInfoCollection* bodies = getTile(index);
				if(bodies != NULL)
				{
					for(CollisionInfoIterator i = bodies->begin(); i != bodies->end(); ++i)
					{
						CollisionInfo* body = *i;
						if(body->canCollide(mask1, mask2))
							result.push_back(body);
					}
				}
			}
		}

		return result;
	}
}