#include "StaticBodiesIndex.h"
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	void StaticBodiesIndex::draw(void)
	{
		for(int i = 0; i < _gridWidth; ++i)
			for(int j = 0; j < _gridHeight; ++j)
				if(get(i, j) != NULL)
					Graphics::get().drawRect(Rect(i * _tileSize + _tileSize / 2.0f, j * _tileSize + _tileSize / 2.0f, _tileSize, _tileSize), Color(0.0f, 1.0f, 0.0f, 0.1f));
	}

	StaticBodiesIndex::StaticBodiesIndex(const Vector& worldSize, float tileSize)
		: _tileSize(tileSize)
	{

		_gridWidth = getIndex(worldSize.getWidth());
		_gridHeight = getIndex(worldSize.getHeight());
		
		int size = _gridWidth * _gridHeight;
		_grid = new std::vector<StaticBody*>*[size];
		for(int i = 0; i < size; ++i)
			_grid[i] = NULL;
	}

	void StaticBodiesIndex::add(StaticBody* staticBody)
	{
		Rect bounds = staticBody->getBounds();
		int leftIndex = getIndex(bounds.getLeft());
		int rightIndex = getIndex(bounds.getRight());
		int topIndex = getIndex(bounds.getTop());
		int bottomIndex = getIndex(bounds.getBottom());

		// TODO: Validate

		for(int xIndex = leftIndex; xIndex <= rightIndex; ++xIndex)
			for(int yIndex = bottomIndex; yIndex <= topIndex; ++yIndex)
			{
				// TODO: Merge functions
				int index = yIndex * _gridWidth + xIndex;
				std::vector<StaticBody*>* tile = _grid[index];
				if(tile == NULL)
				{
					tile = new std::vector<StaticBody*>();
					_grid[index] = tile;
				}
				tile->push_back(staticBody);
			}

	}

	std::vector<StaticBody*>* StaticBodiesIndex::get(int x, int y) const
	{
		// TODO: Validate;
		int index = y * _gridWidth + x;
		return _grid[index];
	}
}