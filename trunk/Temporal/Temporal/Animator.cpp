#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "Message.h"
#include "MessageParams.h"
#include "SceneNode.h"
#include <math.h>

namespace Temporal
{
	static const Hash TIMER_SERIALIZATION = Hash("ANM_SER_TIMER");
	static const Hash REPEAT_SERIALIZATION = Hash("ANM_SER_REPEAT");
	static const Hash REWIND_SERIALIZATION = Hash("ANM_SER_REWIND");

	void bindSceneNodes(SceneNodeBindingCollection& bindings, SceneNode& node)
	{
		bindings[node.getID()] = new SceneNodeBinding(node);
		for(SceneNodeIterator i = node.getChildren().begin(); i != node.getChildren().end(); ++i)
		{
			bindSceneNodes(bindings, **i);
		}
	}

	Animator::Animator(const AnimationCollection& animations, SceneNode& root) :
		_animations(animations), _animationId(Hash::INVALID), _rewind(false), _repeat(false)
	{
		bindSceneNodes(_bindings, root);
	}

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
		float totalPeriod = _timer.getElapsedTimeInMillis();
		const Animation& animation = *_animations.at(_animationId);
		float animationDuration = animation.getDuration();
		if(animationDuration != 0.0f && totalPeriod > animationDuration && !_repeat)
		{
			sendMessageToOwner(Message(MessageID::ANIMATION_ENDED));			
			return;
		}

		float relativePeriod =  fmod(totalPeriod, animationDuration);
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			const Hash& sceneNodeID = i->first;
			SceneNodeBinding& binding = *i->second;
			int index = binding.getIndex();
			const SceneNodeSampleCollection& sceneNodeSamples = animation.get(sceneNodeID);
			const SceneNodeSample* currentSample = sceneNodeSamples.at(index);
			while(currentSample->getEndTime() != 0.0f && (currentSample->getStartTime() > relativePeriod || currentSample->getEndTime() <= relativePeriod))
			{
				index = (index + 1) % sceneNodeSamples.size();
				binding.setIndex(index);
				currentSample = sceneNodeSamples.at(index);
			}
			index = (index + 1) % sceneNodeSamples.size();
			const SceneNodeSample& nextSample = *sceneNodeSamples.at(index);
			float samplePeriod = relativePeriod - currentSample->getStartTime();
			float sampleDuration = currentSample->getDuration();
			float interpolation = sampleDuration == 0.0f ? 0.0f : samplePeriod / sampleDuration;
			Vector translation = currentSample->getTranslation() + (nextSample.getTranslation() - currentSample->getTranslation()) * interpolation;
			float rotation = currentSample->getRotation() + (nextSample.getRotation() - currentSample->getRotation()) * interpolation;
			SceneNode& sceneNode = binding.getSceneNode();
			sceneNode.setTranslation(translation);
			sceneNode.setRotation(rotation);
			sceneNode.setSpriteInterpolation(interpolation);
			sceneNode.setSpriteGroupID(currentSample->getSpriteGroupId());
		}
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