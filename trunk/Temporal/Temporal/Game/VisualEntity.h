#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Graphics\SpriteSheet.h>
#include <Temporal\Graphics\Animator.h>
#include "Enums.h"

namespace Temporal
{
	class VisualEntity
	{
	public:
		VisualEntity(const SpriteSheet& spritesheet, VisualLayer::Type visualLayer) : _animator(spritesheet), _visualLayer(visualLayer) {};
		virtual ~VisualEntity(void) {}

		const Animator& getAnimator(void) const { return _animator; }
		Animator& getAnimator(void) { return _animator; }
		VisualLayer::Type getVisualLayer(void) const { return _visualLayer; }

		virtual const Vector& getCenter(void) const = 0;
		virtual Orientation::Type getOrientation(void) const { return Animator::ORIENTATION; }

		virtual void update(void);
		virtual void draw(void) const;
	private:
		Animator _animator;
		const VisualLayer::Type _visualLayer;

		VisualEntity(const VisualEntity&);
		VisualEntity& operator=(const VisualEntity&);
	};
}