#pragma once

#include <Temporal\Base\Base.h>
#include "VisualEntity.h"

namespace Temporal
{
	class BackgroundEntity : public VisualEntity
	{
	public:
		// TODO: Use something different then spritesheet
		BackgroundEntity(const Vector& location, const SpriteSheet& spritesheet, VisualLayer::Type visualLayer) : _location(location), VisualEntity(spritesheet, visualLayer) {}

		virtual const Vector& getCenter(void) const { return _location; }
	private:
		const Vector _location;
	};
}