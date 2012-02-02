#include "Animator.h"
#include "Sprite.h"

namespace Temporal
{
	void Animator::handleMessage(Message& message)
	{
		switch(message.getID())
		{
			case(MessageID::RESET_ANIMATION):
			{
				reset(message.getParam<ResetAnimationParams>());
				break;
			}
			case(MessageID::UPDATE):
			{
				update();
				break;
			}
		}
	}

	void Animator::update(void)
	{
		const SpriteGroup& spriteGroup = getSpriteGroup();
		int framesCount = spriteGroup.getSize();
		bool animationEnded = _update == UPDATES_PER_FRAME - 1 && (!_rewind ? _frameID == framesCount - 1 : _frameID == 0);
		_update = (_update + 1) % UPDATES_PER_FRAME;
		if(_update == 0 && (!animationEnded || _repeat))
		{
			int modifier = _rewind ? -1 : 1;
			_frameID = (framesCount + _frameID + modifier) % framesCount;
			sendMessage(Message(MessageID::SET_SPRITE_ID, &_frameID));
		}
		if(animationEnded)
			sendMessage(Message(MessageID::ANIMATION_ENDED));
	}

	const SpriteGroup& Animator::getSpriteGroup(void) const
	{
		Message getSpriteGroup(MessageID::GET_SPRITE_GROUP);
		sendMessage(getSpriteGroup);
		const SpriteGroup& spriteGroup = getSpriteGroup.getParam<SpriteGroup>();
		return spriteGroup;
	}

	void Animator::reset(const ResetAnimationParams& resetAnimationParams)
	{
		_update = 0;
		_rewind = resetAnimationParams.getRewind();
		_repeat = resetAnimationParams.getRepeat();
		int animationID = resetAnimationParams.getAnimationID();
		sendMessage(Message(MessageID::SET_SPRITE_GROUP_ID, &animationID));
		const SpriteGroup& spriteGroup = getSpriteGroup();
		_frameID = !_rewind ? 0 : spriteGroup.getSize() - 1;
		sendMessage(Message(MessageID::SET_SPRITE_ID, &_frameID));
	}
}