#pragma once
#include <Temporal\Base\Base.h>
#include "SpriteSheet.h"
#include "Texture.h"

namespace Temporal
{
	class Sprite
	{
	public:
		Sprite(const SpriteSheet& spritesheet) :
			_spritesheet(spritesheet) {}
		
		void update(void);
		void draw(const Vector& location, Orientation::Type orientation) const;

		void reset(int animation, bool rewind = false, bool repeat = false);
		void play(void) { _paused = false; }
		void pause(void) { _paused = true; }
		
		bool isAnimationEnded(void) const;
	private:
		static const int UPDATES_PER_FRAME = 2;
		static const Orientation::Type SPRITE_ORIENTATION = Orientation::LEFT;

		const SpriteSheet& _spritesheet;

		bool _rewind;
		bool _repeat;
		bool _paused;

		int _update;
		int _animation;
		int _frame;

		int getFramesCount(void) const { return _spritesheet._elements[_animation]->_elementsCount; }

		Sprite(const Sprite&);
		Sprite& operator=(const Sprite&);
	};
}