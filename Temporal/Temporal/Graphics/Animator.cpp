#include "Animator.h"
#include "Sprite.h"

namespace Temporal
{
	void Animator::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::RESET_ANIMATION)
		{
			reset(message.getParam<ResetAnimationParams>());
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
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
			sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &_frameID));
		}
		if(animationEnded)
			sendMessageToOwner(Message(MessageID::ANIMATION_ENDED));
	}

	const SpriteGroup& Animator::getSpriteGroup(void) const
	{
		const SpriteGroup& spriteGroup = sendQueryMessageToOwner<SpriteGroup>(Message(MessageID::GET_SPRITE_GROUP));
		return spriteGroup;
	}

	void Animator::reset(const ResetAnimationParams& resetAnimationParams)
	{
		_update = 0;
		_rewind = resetAnimationParams.getRewind();
		_repeat = resetAnimationParams.getRepeat();
		int animationID = resetAnimationParams.getAnimationID();
		sendMessageToOwner(Message(MessageID::SET_SPRITE_GROUP_ID, &animationID));
		const SpriteGroup& spriteGroup = getSpriteGroup();
		_frameID = !_rewind ? 0 : spriteGroup.getSize() - 1;
		sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &_frameID));
	}
}