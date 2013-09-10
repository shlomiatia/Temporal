#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include <math.h>

namespace Temporal
{
	const Hash Animator::TYPE = Hash("animator");

	void bindSceneNodes(SceneNodeBindingCollection& bindings, SceneNode& node)
	{
		bindings[node.getID()] = new SceneNodeBinding(node);
		for(SceneNodeIterator i = node.getChildren().begin(); i != node.getChildren().end(); ++i)
		{
			bindSceneNodes(bindings, **i);
		}
	}

	void Animator::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_animationSet = ResourceManager::get().getAnimationSet(_animationSetFile.c_str());
		}
		else if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			const Renderer& renderer = *static_cast<const Renderer*>(getEntity().get(Renderer::TYPE));
			bindSceneNodes(_bindings, renderer.getRoot());
		}
		else if(message.getID() == MessageID::RESET_ANIMATION)
		{
			Hash animationId = getHashParam(message.getParam());
			reset(animationId);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
	}

	void Animator::update(float framePeriod)
	{
		_timer.update(framePeriod);
		float totalPeriod = _timer.getElapsedTime();
		const Animation& animation = _animationSet->get(_animationId);
		float animationDuration = animation.getDuration();
		if((animationDuration == 0.0f || totalPeriod > animationDuration) && !animation.Repeat())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));			
			return;
		}
		
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			Hash sceneNodeID = i->first;
			SceneNodeBinding& binding = *i->second;
			if(binding.getSceneNode().isTransformOnly())
				continue;
			int index = binding.getIndex();
			const SampleCollection& sampleSet = animation.get(sceneNodeID).getSamples();
			int size = sampleSet.size();
			const Sample* currentSample = sampleSet.at(index);
			float relativePeriod =  fmod(totalPeriod, animationDuration);
			int offset = 1;
			if(animation.Rewind())
			{
				offset = -1;
				relativePeriod = animationDuration - relativePeriod;
			}
			while(currentSample->getEndTime() != 0.0f && (currentSample->getStartTime() > relativePeriod || currentSample->getEndTime() < relativePeriod))
			{
				index = (size + index + offset) % size;
				binding.setIndex(index);

				currentSample = sampleSet.at(index);
			}
			int nextIndex = (size + index + 1) % size;
			const Sample* nextSample = sampleSet.at(nextIndex);
			float samplePeriod = relativePeriod - currentSample->getStartTime();
			float sampleDuration = currentSample->getDuration();
			float interpolation = sampleDuration == 0.0f ? 0.0f : samplePeriod / sampleDuration;
			
			Vector translation = currentSample->getTranslation() * (1 - interpolation) + nextSample->getTranslation() * interpolation;
			float rotation = currentSample->getRotation() * (1 - interpolation) + nextSample->getRotation() * interpolation;
			SceneNode& sceneNode = binding.getSceneNode();
			sceneNode.setSpriteGroupId(currentSample->getSpriteGroupId());
			sceneNode.setTranslation(translation);
			sceneNode.setRotation(rotation);
			sceneNode.setSpriteInterpolation(interpolation);
		}
	}

	void Animator::reset(Hash animationId)
	{
		_timer.reset();
		_animationId = animationId;
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			i->second->setIndex(0);
		}
		update(0.0f);
	}
}