#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include <math.h>

namespace Temporal
{
	const Hash Animator::TYPE = Hash("animator");
	const int Animator::FPS = 15;
	
	void bindSceneNodes(SceneNodeBindingCollection& bindings, SceneNode& node)
	{
		if(!node.isTransformOnly())
			bindings.push_back(new SceneNodeBinding(node));

		for(SceneNodeIterator i = node.getChildren().begin(); i != node.getChildren().end(); ++i)
		{
			bindSceneNodes(bindings, **i);
		}
	}

	Animator::~Animator()
	{
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
			delete *i;
	}

	void Animator::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_animationSet = ResourceManager::get().getAnimationSet(_animationSetFile.c_str());
		}
		else if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			SceneNode& sceneNode = *static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(_bindings, sceneNode);
		}
		else if(message.getID() == MessageID::RESET_ANIMATION)
		{
			Hash animationId = getHashParam(message.getParam());
			reset(animationId);
		}
		else if(message.getID() == MessageID::TOGGLE_ANIMATION)
		{
			_isPaused = !_isPaused;
		}
		else if(message.getID() == MessageID::SET_ANIMATION_FRAME)
		{
			int index = getIntParam(message.getParam());
				_timer.reset((float)index / (float)FPS);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_isPaused)
			{
				float framePeriod = getFloatParam(message.getParam());
				_timer.update(framePeriod);
			}
			update();
		}
	}

	void Animator::update()
	{
		float currentTime = _timer.getElapsedTime();
		const Animation& animation = _animationSet->get(_animationId);
		float animationDuration = animation.getDuration();
		float currentIndex = currentTime * FPS;

		if(currentIndex > animationDuration && !animation.repeat())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));			
			return;
		}
		float relativeIndex = fmod(currentIndex, animationDuration);
			Direction::Enum direction = Direction::FORWARD;
		if(animation.rewind())
		{
			relativeIndex = animationDuration - relativeIndex;
			direction = Direction::BACKWARD;
		}
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			SceneNodeBinding& binding = **i;
			
			const SceneNodeSample* currentSample = binding.getSample();
			if(!currentSample)
				continue;
			while(currentSample->getParent().getIndex() > relativeIndex ||
				  (currentSample->getNext()->getParent().getIndex() < relativeIndex && 
				   currentSample->getParent().getIndex() < currentSample->getNext()->getParent().getIndex()))
				currentSample = currentSample->getSibling(direction);
			binding.setSample(currentSample);
			const SceneNodeSample* nextSample = currentSample->getNext();
			float sampleOffset = relativeIndex - currentSample->getParent().getIndex();
			float sampleDuration = nextSample->getParent().getIndex() - currentSample->getParent().getIndex();
			if(sampleDuration < 0.0f)
				sampleDuration = animationDuration + sampleDuration;
			float interpolation = sampleDuration == 0.0f ? 0.0f : sampleOffset / sampleDuration;
			
			Vector translation = currentSample->getTranslation() * (1 - interpolation) + nextSample->getTranslation() * interpolation;
			float rotation = currentSample->getRotation() * (1 - interpolation) + nextSample->getRotation() * interpolation;
			SceneNode& sceneNode = binding.getSceneNode();
			if(currentSample->getSpriteGroupId() != Hash::INVALID)
				sceneNode.setSpriteGroupId(currentSample->getSpriteGroupId());
			sceneNode.setTranslation(translation);
			sceneNode.setRotation(rotation);
			sceneNode.setSpriteInterpolation(interpolation);
		}
	}

	void Animator::reset(Hash animationId)
	{
		_animationId = animationId;
		const Animation& animation = _animationSet->get(_animationId);
		const SceneGraphSample& sceneGraphSample = **animation.getSamples().begin();
		_timer.reset();
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			SceneNodeBinding& binding = **i;
			const SceneNodeSample* sceneNodeSample = sceneGraphSample.getSamples().at(binding.getSceneNode().getID());
			binding.setSample(sceneNodeSample);
		}
		update();
	}
}