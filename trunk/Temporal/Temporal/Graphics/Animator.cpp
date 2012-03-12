#include "Animator.h"
#include "SpriteGroup.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageParams.h>

namespace Temporal
{
	void Animator::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::RESET_ANIMATION)
		{
			const ResetAnimationParams& resetAnimationParams = *(const ResetAnimationParams* const)message.getParam();
			reset(resetAnimationParams);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(const float* const)message.getParam();
			update(framePeriodInMillis);
		}
	}

	// TODO: Maybe reset every animation change SLOTH
	void Animator::update(float framePeriodInMillis)
	{
		const SpriteGroup& spriteGroup = getSpriteGroup();
		int framesCount = spriteGroup.getSize();
		_timer.update(framePeriodInMillis);
		bool framePassed = _timer.getElapsedTimeInMillis() > FRAME_PERIOD;
		if(framePassed) _timer.reset();
		bool animationEnded = framePassed && (!_rewind ? _frameID == framesCount - 1 : _frameID == 0);
		if(framePassed && (!animationEnded || _repeat))
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
		const SpriteGroup& spriteGroup = *(const SpriteGroup* const)sendMessageToOwner(Message(MessageID::GET_SPRITE_GROUP));
		return spriteGroup;
	}

	void Animator::reset(const ResetAnimationParams& resetAnimationParams)
	{
		_timer.reset();
		_rewind = resetAnimationParams.getRewind();
		_repeat = resetAnimationParams.getRepeat();
		int animationID = resetAnimationParams.getAnimationID();
		sendMessageToOwner(Message(MessageID::SET_SPRITE_GROUP_ID, &animationID));
		const SpriteGroup& spriteGroup = getSpriteGroup();
		_frameID = !_rewind ? 0 : spriteGroup.getSize() - 1;
		sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &_frameID));
	}
}