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
			const ResetAnimationParams& resetAnimationParams = *(ResetAnimationParams*)message.getParam();
			reset(resetAnimationParams);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(float*)message.getParam();
			update(framePeriodInMillis);
		}
	}

	void Animator::update(float framePeriodInMillis)
	{
		const SpriteGroup& spriteGroup = getSpriteGroup();
		int framesCount = spriteGroup.getSize();
		_timer.update(framePeriodInMillis);
		int framesPassed = (int)(_timer.getElapsedTimeInMillis() / FRAME_PERIOD);
		int framesDelta = framesPassed % framesCount;
		int initialFrame = getInitialFrame();
		int frameID = initialFrame + (_rewind ? -1 : 1) * framesDelta;
		if(framesPassed < framesCount  || _repeat)
			sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &frameID));
		bool animationEnded = frameID == initialFrame && framesPassed > 0;
		if(animationEnded)
			sendMessageToOwner(Message(MessageID::ANIMATION_ENDED));
	}

	const SpriteGroup& Animator::getSpriteGroup(void) const
	{
		const SpriteGroup& spriteGroup = *(SpriteGroup*)sendMessageToOwner(Message(MessageID::GET_SPRITE_GROUP));
		return spriteGroup;
	}

	void Animator::reset(const ResetAnimationParams& resetAnimationParams)
	{
		_timer.reset();
		_rewind = resetAnimationParams.getRewind();
		_repeat = resetAnimationParams.getRepeat();
		int animationID = resetAnimationParams.getAnimationID();
		sendMessageToOwner(Message(MessageID::SET_SPRITE_GROUP_ID, &animationID));
		int initialFrame = getInitialFrame();
		sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &initialFrame));
	}

	int Animator::getInitialFrame(void) const
	{
		const SpriteGroup& spriteGroup = getSpriteGroup();
		return !_rewind ? 0 : spriteGroup.getSize() - 1;
	}
}