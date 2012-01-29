#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Graphics\SpriteSheet.h>
#include <Temporal\Graphics\Sprite.h>
#include "Enums.h"

namespace Temporal
{
	class VisualEntity
	{
	public:
		VisualEntity(const SpriteSheet& spritesheet, VisualLayer::Type visualLayer);
		virtual ~VisualEntity(void) {}

		const Sprite& getSprite(void) const { return _sprite; }
		Sprite& getSprite(void) { return _sprite; }
		VisualLayer::Type getVisualLayer(void) const { return _visualLayer; }

		virtual const Vector& getCenter(void) const = 0;
		virtual Orientation::Type getOrientation(void) const { return Sprite::ORIENTATION; }

		virtual void update(void);
		virtual void draw(void) const;
	private:
		Sprite _sprite;
		const VisualLayer::Type _visualLayer;

		VisualEntity(const VisualEntity&);
		VisualEntity& operator=(const VisualEntity&);
	};
}