#include "VisualEntity.h"

namespace Temporal
{
	void VisualEntity::update(void)
	{
		getAnimator().update();
	}

	void VisualEntity::draw(void) const
	{
		getAnimator().draw(getCenter(), getOrientation()); 
	}
}