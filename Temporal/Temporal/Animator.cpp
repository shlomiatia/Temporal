#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "Message.h"
#include "MessageParams.h"
#include "Animation.h"
#include "AnimationSample.h"

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
		_timer.update(framePeriodInMillis);
		const Animation& animation = *_animations.at(_animationId);
		const AnimationSample& animationSample = animation.get(0);

		float duration = animationSample.getDuration();
		float normalizedTime = duration == 0.0f ? 1.0f : _timer.getElapsedTimeInMillis() / duration;
		if(normalizedTime <= 1.0f  || _repeat)
		{
			float normalizedSpriteIndex = !_rewind ? normalizedTime : 1.0f - normalizedTime;
			sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &normalizedSpriteIndex));
		}
		bool animationEnded = normalizedTime >= 1.0f;
		if(!_repeat && animationEnded)
			sendMessageToOwner(Message(MessageID::ANIMATION_ENDED));
	}

	void Animator::reset(const ResetAnimationParams& resetAnimationParams)
	{
		_timer.reset();
		_animationId = resetAnimationParams.getAnimationID();
		_rewind = resetAnimationParams.getRewind();
		_repeat = resetAnimationParams.getRepeat();
		sendMessageToOwner(Message(MessageID::SET_SPRITE_GROUP_ID, &_animationId));
		float normalizedTime = _rewind ? 1.0f : 0.0f;
		sendMessageToOwner(Message(MessageID::SET_SPRITE_ID, &normalizedTime));
	}
}