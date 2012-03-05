#include "StaticBodiesIndex.h"
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	// TODO: Move to static bodies
	void StaticBodiesIndex::draw(void) const
	{
		for(int i = 0; i < _gridWidth; ++i)
			for(int j = 0; j < _gridHeight; ++j)
				if(get(i, j) != NULL)
					Graphics::get().drawRect(Rect(getTileCenter(i, j), Vector(_tileSize, _tileSize)), Color(0.0f, 0.0f, 1.0f, 0.3f));
	}

	void StaticBodiesIndex::init(const Vector& worldSize, float tileSize)
	{
		_tileSize = tileSize;
		_gridWidth = getAxisIndex(worldSize.getWidth());
		_gridHeight = getAxisIndex(worldSize.getHeight());
		
		int size = getGridSize();
		_grid = new std::vector<StaticBody*>*[size];
		for(int i = 0; i < size; ++i)
			_grid[i] = NULL;
	}

	bool StaticBodiesIndex::add(void* caller, void* data, int i, int j)
	{
		StaticBodiesIndex* staticBodiesIndex = (StaticBodiesIndex*)caller;
		StaticBody* staticBody = (StaticBody*)data;

		int index = staticBodiesIndex->getIndex(i, j);
		std::vector<StaticBody*>* tile = staticBodiesIndex->_grid[index];
		if(tile == NULL)
		{
			tile = new std::vector<StaticBody*>();
			staticBodiesIndex->_grid[index] = tile;
		}
		tile->push_back(staticBody);
		return true;
	}

	void StaticBodiesIndex::add(StaticBody* staticBody)
	{
		Rect bounds = staticBody->getBounds();
		iterateTiles(bounds, this, staticBody, add);
	}

	std::vector<StaticBody*>* StaticBodiesIndex::get(int x, int y) const
	{
		int index = getIndex(x, y);
		if(index < 0 || index >= getGridSize())
			return NULL;
		else
			return _grid[index];
	}

	void StaticBodiesIndex::iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleTile)(void* caller, void* data, int i, int j)) const
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

	bool StaticBodiesIndex::iterateStaticBodies(void* caller, void* data, int i, int j)
	{
		StaticBodiesIndex* staticBodiesIndex = (StaticBodiesIndex*)caller;
		IterateStaticBodiesHelper* helper = (IterateStaticBodiesHelper*)data;

		std::vector<StaticBody*>* staticBodies = staticBodiesIndex->get(i, j);
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

	void StaticBodiesIndex::iterateTiles(const Rect& rect, void* caller, void* data, bool(*handleStaticBody)(void* caller, void* data, const StaticBody&)) const
	{
		IterateStaticBodiesHelper helper;
		helper.caller = caller; 
		helper.data = data;
		helper.handleStaticBody = handleStaticBody;
		iterateTiles(rect, (void*)this, &helper, iterateStaticBodies);
	}
}