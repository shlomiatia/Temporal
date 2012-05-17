#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "Message.h"
#include "MessageParams.h"

namespace Temporal
{
	static const Hash TIMER_SERIALIZATION = Hash("ANM_SER_TIMER");
	static const Hash REPEAT_SERIALIZATION = Hash("ANM_SER_REPEAT");
	static const Hash REWIND_SERIALIZATION = Hash("ANM_SER_REWIND");

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
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			serialization.serialize(TIMER_SERIALIZATION, _timer.getElapsedTimeInMillis());
			serialization.serialize(REPEAT_SERIALIZATION, _repeat);
			serialization.serialize(REWIND_SERIALIZATION, _rewind);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			_timer.reset(serialization.deserializeFloat(TIMER_SERIALIZATION));
			_repeat = serialization.deserializeBool(REPEAT_SERIALIZATION);
			_rewind = serialization.deserializeBool(REWIND_SERIALIZATION);
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
		if(!_repeat && animationEnded)
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