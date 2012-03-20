#include "Grid.h"
#include "StaticBody.h"
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	void Grid::draw(void) const
	{
		for(int i = 0; i < _gridWidth; ++i)
			for(int j = 0; j < _gridHeight; ++j)
				if(getTile(i, j) != NULL)
					Graphics::get().drawRect(Rect(getTileCenter(i, j), Vector(_tileSize, _tileSize)), Color(0.0f, 0.0f, 1.0f, 0.3f));
	}

	void Grid::init(const Vector& worldSize, float tileSize)
	{
		_tileSize = tileSize;
		_gridWidth = getAxisIndex(worldSize.getWidth());
		_gridHeight = getAxisIndex(worldSize.getHeight());
		
		int size = getSize();
		_grid = new std::vector<const StaticBody*>*[size];
		for(int i = 0; i < size; ++i)
			_grid[i] = NULL;
	}

	void Grid::dispose(void)
	{
		int size = getSize();
		for(int i = 0; i < size; ++i)
			if(_grid[i] != NULL)
				delete _grid[i];
	}

	bool Grid::add(void* caller, void* data, int index)
	{
		Grid* grid = (Grid*)caller;
		const StaticBody* staticBody = (StaticBody*)data;

		std::vector<const StaticBody*>* tile = grid->getTile(index);
		if(tile == NULL)
		{
			tile = new std::vector<const StaticBody*>();
			grid->_grid[index] = tile;
		}
		tile->push_back(staticBody);
		return true;
	}

	void Grid::add(const StaticBody* staticBody)
	{
		Rect bounds = staticBody->getBounds();
		iterateTiles(bounds, this, (void*)staticBody, add);
	}

	std::vector<const StaticBody*>* Grid::getTile(int i, int j) const
	{
		int index = getIndex(i, j);
		return getTile(index);
	}

	std::vector<const StaticBody*>* Grid::getTile(int index) const
	{
		if(index < 0 || index >= getSize())
			return NULL;
		else
			return _grid[index];
	}

	void Grid::iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int index)) const
	{
		int leftIndex = getAxisIndex(rect.getLeft());
		int rightIndex = getAxisIndex(rect.getRight());
		int topIndex = getAxisIndex(rect.getTop());
		int bottomIndex = getAxisIndex(rect.getBottom());

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

		std::vector<const StaticBody*>* staticBodies = grid.getTile(index);
		if(staticBodies != NULL)
		{
			for(unsigned int index = 0; index < staticBodies->size(); ++index)
			{
				const StaticBody& staticBody = *(*staticBodies)[index];
				if(!helper->handleStaticBody(helper->caller, helper->data, staticBody))
					return false;
			}
		}
		return true;
	}

	void Grid::iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const
	{
		IterateStaticBodiesHelper helper;
		helper.caller = caller; 
		helper.data = data;
		helper.handleStaticBody = handleStaticBody;
		iterateTiles(rect, (void*)this, &helper, iterateStaticBodies);
	}
}