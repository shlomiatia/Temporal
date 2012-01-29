#include "VisualEntity.h"

namespace Temporal
{
	void VisualEntity::update(void)
	{
		getSprite().update();
	}

	void VisualEntity::draw(void) const
	{
		getSprite().draw(getCenter(), getOrientation()); 
	}
}