#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include <math.h>

namespace Temporal
{
	static const Hash ANIMATION_ID_SERIALIZATION = Hash("ANM_SER_ANIMATION_ID");
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
			const ResetAnimationParams& resetAnimationParams = getResetAnimationParams(message.getParam());
			reset(resetAnimationParams);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = getFloatParam(message.getParam());
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			serialization.serialize(TIMER_SERIALIZATION, _timer.getElapsedTimeInMillis());
			serialization.serialize(ANIMATION_ID_SERIALIZATION, _animationId);
			serialization.serialize(REPEAT_SERIALIZATION, _repeat);
			serialization.serialize(REWIND_SERIALIZATION, _rewind);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			_timer.reset(serialization.deserializeFloat(TIMER_SERIALIZATION));
			_animationId = Hash(serialization.deserializeUInt(ANIMATION_ID_SERIALIZATION));
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
		if((animationDuration == 0.0f || totalPeriod > animationDuration) && !_repeat)
		{
			sendMessageToOwner(Message(MessageID::ANIMATION_ENDED));			
			return;
		}
		float relativePeriod =  fmod(totalPeriod, animationDuration);
		int offset = 1;
		if(_rewind)
		{
			offset = -1;
			relativePeriod = animationDuration - relativePeriod;
		}
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			const Hash& sceneNodeID = i->first;
			SceneNodeBinding& binding = *i->second;
			if(binding.getSceneNode().isTransformOnly())
				continue;
			int index = binding.getIndex();
			const SceneNodeSampleCollection& sceneNodeSamples = animation.get(sceneNodeID);
			int size = sceneNodeSamples.size();
			const SceneNodeSample* currentSample = sceneNodeSamples.at(index);
			while(currentSample->getEndTime() != 0.0f && (currentSample->getStartTime() > relativePeriod || currentSample->getEndTime() < relativePeriod))
			{
				index = (size + index + offset) % size;
				binding.setIndex(index);

				currentSample = sceneNodeSamples.at(index);
			}
			int nextIndex = (size + index + 1) % size;
			const SceneNodeSample& nextSample = *sceneNodeSamples.at(nextIndex);
			float samplePeriod = relativePeriod - currentSample->getStartTime();
			float sampleDuration = currentSample->getDuration();
			float interpolation = sampleDuration == 0.0f ? 0.0f : samplePeriod / sampleDuration;
			
			Vector translation = currentSample->getTranslation() * (1 - interpolation) + nextSample.getTranslation() * interpolation;
			float rotation = currentSample->getRotation() * (1 - interpolation) + nextSample.getRotation() * interpolation;
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