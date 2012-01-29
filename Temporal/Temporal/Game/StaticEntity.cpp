#include "StaticEntity.h"

namespace Temporal
{
	void StaticEntity::draw(void) const
	{
		const float TILE_SIZE = 32.0f;
		for(float x = -1.0f; x <= _body.getBounds().getWidth() / TILE_SIZE; ++x)
			for(float y = 0.0f; y <= _body.getBounds().getHeight() / TILE_SIZE; ++y)
			{
				float centerX = _body.getBounds().getLeft() + x * TILE_SIZE + TILE_SIZE / 2.0f ;
				float centerY = _body.getBounds().getBottom() + y * TILE_SIZE + TILE_SIZE / 4.0f;
				getSprite().draw(Vector(centerX, centerY), getOrientation()); 
			}
	}
}