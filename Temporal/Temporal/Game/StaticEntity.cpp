#include "StaticEntity.h"

namespace Temporal
{
	void StaticEntity::draw(void) const
	{
		const float PHYSICS_TILE_SIZE = 16.0f;
		float xc = _body.getBounds().getWidth() / PHYSICS_TILE_SIZE + 1.0f;
		for(float x = 0.0f; x < xc; ++x)
			for(float y = 0.0f; y < _body.getBounds().getHeight() / PHYSICS_TILE_SIZE; ++y)
			{
				float centerX = _body.getBounds().getLeft() + x * PHYSICS_TILE_SIZE - PHYSICS_TILE_SIZE / 4.0f;
				float centerY = _body.getBounds().getBottom() + y * PHYSICS_TILE_SIZE + PHYSICS_TILE_SIZE / 2.0f;
				float angle = xc <= 2.0f && y > 0.0f ? 90.0f : 0.0f;
				Orientation::Type orientation = xc > 2.0f ? getOrientation() : (Orientation::Type)(-1 * getOrientation());
				if(x <= xc / 2.0f + 1.0f)
					getSprite().draw(Vector(centerX, centerY), orientation, angle); 
				if(x >= xc / 2.0f)
					getSprite().draw(Vector(centerX, centerY), (Orientation::Type)(-1 * orientation), angle); 
			}
	}
}