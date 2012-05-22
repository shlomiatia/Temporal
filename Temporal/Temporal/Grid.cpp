#include "Grid.h"
#include "StaticBody.h"
#include "Shape.h"
#include "AABB.h"
#include "Graphics.h"
#include "ShapeOperations.h"
#include "DirectedInterval.h"

namespace Temporal
{
	void Grid::init(const Size& worldSize, float tileSize)
	{
		_tileSize = tileSize;
		_gridWidth = getAxisIndex(worldSize.getWidth());
		_gridHeight = getAxisIndex(worldSize.getHeight());
		
		int size = getSize();
		_grid = new StaticBodyCollection*[size];
		for(int i = 0; i < size; ++i)
			_grid[i] = NULL;
	}

	void Grid::dispose(void)
	{
		int size = getSize();
		for(int i = 0; i < size; ++i)
			if(_grid[i] != NULL)
				delete _grid[i];
		delete _grid;
	}

	void Grid::draw(void) const
	{
		for(int i = 0; i < _gridWidth; ++i)
			for(int j = 0; j < _gridHeight; ++j)
				if(getTile(i, j) != NULL)
					Graphics::get().draw(AABB(getTileCenter(i, j), Size(_tileSize, _tileSize)), Color(0.0f, 0.0f, 1.0f, 0.3f));
	}

	bool Grid::add(void* caller, void* data, int index)
	{
		Grid* grid = (Grid*)caller;
		const StaticBody* staticBody = (StaticBody*)data;

		StaticBodyCollection* tile = grid->getTile(index);
		if(tile == NULL)
		{
			tile = new StaticBodyCollection();
			grid->_grid[index] = tile;
		}
		tile->push_back(staticBody);
		return true;
	}

	void Grid::add(const StaticBody* staticBody)
	{
		const Shape& shape = staticBody->getShape();
		iterateTiles(shape, this, (void*)staticBody, add);
	}

	StaticBodyCollection* Grid::getTile(int i, int j) const
	{
		int index = getIndex(i, j);
		return getTile(index);
	}

	StaticBodyCollection* Grid::getTile(int index) const
	{
		if(index < 0 || index >= getSize())
			return NULL;
		else
			return _grid[index];
	}

	bool Grid::cast(const DirectedInterval& dirInt, Point& pointOfIntersection)
	{
		const Point& origin = dirInt.getOrigin();
		const Point& destination = dirInt.getTarget();
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
			StaticBodyCollection* staticBodies = getTile(i, j);
			if(staticBodies != NULL)
			{
				for(StaticBodyIterator iterator = staticBodies->begin(); iterator != staticBodies->end(); ++iterator)
				{
					const StaticBody& body = **iterator;
					if(intersects(dirInt, body.getShape(), &pointOfIntersection))
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

	void Grid::iterateTiles(const Shape& shape, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int index)) const
	{
		int leftIndex = getAxisIndex(shape.getLeft());
		int rightIndex = getAxisIndex(shape.getRight());
		int topIndex = getAxisIndex(shape.getTop());
		int bottomIndex = getAxisIndex(shape.getBottom());

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				int index = getIndex(i, j);
				if(!handleTile(caller, data, index))
					return;
			}
		}
	}

	struct IterateStaticBodiesHelper
	{
		void* caller;
		void* data;
		bool(*handleStaticBody)(void* caller, void* data, const StaticBody& staticBody);
	};

	bool Grid::iterateStaticBodies(void* caller, void* data, int index)
	{
		const Grid& grid = *(Grid*)caller;
		IterateStaticBodiesHelper* helper = (IterateStaticBodiesHelper*)data;

		StaticBodyCollection* staticBodies = grid.getTile(index);
		if(staticBodies != NULL)
		{
			for(StaticBodyIterator i = staticBodies->begin(); i != staticBodies->end(); ++i)
			{
				const StaticBody& staticBody = **i;
				if(!helper->handleStaticBody(helper->caller, helper->data, staticBody))
					return false;
			}
		}
		return true;
	}

	void Grid::iterateTiles(const Shape& shape, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const
	{
		IterateStaticBodiesHelper helper;
		helper.caller = caller; 
		helper.data = data;
		helper.handleStaticBody = handleStaticBody;
		iterateTiles(shape, (void*)this, &helper, iterateStaticBodies);
	}
}