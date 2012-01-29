#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Physics\Body.h>
#include "VisualEntity.h"

namespace Temporal
{
	class StaticEntity : public VisualEntity
	{
	public:
		StaticEntity(const StaticBody& body, const SpriteSheet& spritesheet, VisualLayer::Type visualLayer) : VisualEntity(spritesheet, visualLayer), _body(body) {}

		virtual const Vector& getCenter(void) const { return _body.getBounds().getCenter(); }

		virtual void draw(void) const;
	private:
		const StaticBody& _body;
	};
}