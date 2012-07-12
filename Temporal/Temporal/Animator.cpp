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
		_timer.update(framePeriodInMillis);
		float animationPeriod = _timer.getElapsedTimeInMillis();
		const SceneGraphSampleCollection& animation = *_animations.at(_animationId);
		bool animationEnded = false;
		for(SceneGraphSampleIterator i = animation.begin(); i != animation.end(); ++i)
		{
			const Hash& sceneNodeID = i->first;
			const SceneNodeSampleCollection& sceneNodeSamples =  *i->second;
			int size = sceneNodeSamples.size();
			float totalSamplesDuration = 0.0f;
			for(int index = 0; index < size; ++index)
			{
				const SceneNodeSample& current = *sceneNodeSamples[index];
				float currentSampleDuration = current.getDuration();
				if(size == 1 || totalSamplesDuration + currentSampleDuration > animationPeriod)
				{
					float normalizedSampleDuration = 0.0f;
					float currentSamplePeriod = animationPeriod - totalSamplesDuration;
					if(currentSampleDuration != 0.0f)
					{
						normalizedSampleDuration = currentSamplePeriod / currentSampleDuration;
					}
					if(normalizedSampleDuration < 1.0f || _repeat)
					{
						SceneNodeParams params(sceneNodeID, Vector::Zero, 0.0f, current.getSpriteGroupId(), !_rewind ? normalizedSampleDuration : 1.0 - normalizedSampleDuration);
						sendMessageToOwner(Message(MessageID::SET_SCENE_NODE, &params));
					}
					if(currentSampleDuration == 0.0f || normalizedSampleDuration >= 1.0f)
					{
						animationEnded = true;
						_timer.reset(0.0f);
					}
					break;
				}
				totalSamplesDuration += currentSampleDuration;
			}
		}

		if(!_repeat && animationEnded)
			sendMessageToOwner(Message(MessageID::ANIMATION_ENDED));
	}

	void Animator::reset(const ResetAnimationParams& resetAnimationParams)
	{
		_timer.reset();
		_animationId = resetAnimationParams.getAnimationID();
		_rewind = resetAnimationParams.getRewind();
		_repeat = resetAnimationParams.getRepeat();
		update(0.0f);
	}
}