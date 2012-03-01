#pragma once

#include <math.h>
#include <vector>
#include <Temporal\Base\Base.h>
#include "StaticBody.h"

namespace Temporal
{
	class StaticBodiesIndex
	{
	public:
		StaticBodiesIndex(const Vector& worldSize, float tileSize);

		int getIndex(float value) const { return (int)(value / _tileSize); }
		float getTileSize(void) const { return _tileSize; }
		
		// TODO: Const stuff
		void add(StaticBody* staticBody);
		std::vector<StaticBody*>* get(int x, int y) const;

		void draw(void);

	private:
		std::vector<StaticBody*>** _grid;
		float _tileSize;
		int _gridWidth;
		int _gridHeight;

		StaticBodiesIndex(const StaticBodiesIndex&);
		StaticBodiesIndex& operator=(const StaticBodiesIndex&);
	};
}

