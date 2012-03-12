#include "Grid.h"
#include "StaticBody.h"
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	// TODO: Do when debug drawing SLOTH
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
		_grid = new std::vector<const StaticBody* const>*[size];
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

	bool Grid::add(void* caller, void* data, int i, int j)
	{
		Grid* const grid = (Grid* const)caller;
		const StaticBody* const staticBody = (StaticBody* const)data;

		int index = grid->getIndex(i, j);
		std::vector<const StaticBody* const>* tile = grid->_grid[index];
		if(tile == NULL)
		{
			tile = new std::vector<const StaticBody* const>();
			grid->_grid[index] = tile;
		}
		tile->push_back(staticBody);
		return true;
	}

	void Grid::add(const StaticBody* const staticBody)
	{
		Rect bounds = staticBody->getBounds();
		iterateTiles(bounds, this, (void*)staticBody, add);
	}

	std::vector<const StaticBody* const>* Grid::getTile(int x, int y) const
	{
		int index = getIndex(x, y);
		if(index < 0 || index >= getSize())
			return NULL;
		else
			return _grid[index];
	}

	void Grid::iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int i, int j)) const
	{
		int leftIndex = getAxisIndex(rect.getLeft());
		int rightIndex = getAxisIndex(rect.getRight());
		int topIndex = getAxisIndex(rect.getTop());
		int bottomIndex = getAxisIndex(rect.getBottom());

		for(int i = leftIndex; i <= rightIndex; ++i)
		{
			for(int j = bottomIndex; j <= topIndex; ++j)
			{
				if(!handleTile(caller, data, i, j))
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

	bool Grid::iterateStaticBodies(void* caller, void* data, int i, int j)
	{
		const Grid& grid = *(const Grid* const)caller;
		IterateStaticBodiesHelper* helper = (IterateStaticBodiesHelper*)data;

		std::vector<const StaticBody* const>* staticBodies = grid.getTile(i, j);
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