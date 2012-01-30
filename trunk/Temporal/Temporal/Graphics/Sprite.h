#pragma once
#include <Temporal\Base\Base.h>
#include "SpriteSheet.h"
#include "Texture.h"

namespace Temporal
{
	class Sprite
	{
	public:
		static const Orientation::Type ORIENTATION = Orientation::LEFT;

		Sprite(const SpriteSheet& spritesheet) :
			_spritesheet(spritesheet), _rewind(false), _repeat(false), _paused(false), _update(0), _animation(0), _frame(0)  {}
		
		void update(void);
		void draw(const Vector& location, Orientation::Type orientation, float rotation = 0.0f) const;

		void reset(int animation, bool rewind = false, bool repeat = false);
		void play(void) { _paused = false; }
		void pause(void) { _paused = true; }
		
		bool isEnded(void) const;
	private:
		// TODO: Use time instad
		static const int UPDATES_PER_FRAME = 4;
		
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