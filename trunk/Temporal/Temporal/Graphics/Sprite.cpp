#include "Sprite.h"
#include "Graphics.h"

namespace Temporal
{
	void Sprite::update(void)
	{
		bool animationEnded = isEnded();
		_update = (_update + 1) % UPDATES_PER_FRAME;
		if(_update == 0 && (!animationEnded || _repeat))
		{
			int modifier = _rewind ? -1 : 1;
			_frame = (getFramesCount() + _frame + modifier) % getFramesCount();
		}
	}

	void Sprite::draw(const Vector& location, Orientation::Type orientation) const
	{
		bool mirrored = orientation != SPRITE_ORIENTATION;
		const Frame& frame = *_spritesheet._elements[_animation]->_elements[_frame];
		float anchoredX = location.getX() - orientation * SPRITE_ORIENTATION * frame.getOffset().getX();
		float anchoredY = location.getY() - frame.getOffset().getY();

		Vector anchoredLocation(anchoredX, anchoredY);

		const Texture& texture = _spritesheet.getTexture();

		Graphics::get().drawTexture(texture, frame.getBounds(), anchoredLocation, mirrored, 0.0f);
	}

	void Sprite::reset(int animation, bool rewind, bool repeat)
	{
		_update = 0;
		_animation = animation;
		_rewind = rewind;
		_repeat = repeat;
		_frame = !_rewind ? 0 : getFramesCount() - 1;
	}

	bool Sprite::isEnded(void) const
	{
		return _update == UPDATES_PER_FRAME - 1 && (!_rewind ? _frame == getFramesCount() - 1 : _frame == 0);
	}
}