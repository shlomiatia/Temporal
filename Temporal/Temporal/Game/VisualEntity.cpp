#include "VisualEntity.h"

namespace Temporal
{
	VisualEntity::VisualEntity(const SpriteSheet& spritesheet, VisualLayer::Type visualLayer) : _sprite(spritesheet), _visualLayer(visualLayer) {}

	void VisualEntity::update(void)
	{
		getSprite().update();
	}

	void VisualEntity::draw(void) const
	{
		getSprite().draw(getCenter(), getOrientation()); 
	}
}