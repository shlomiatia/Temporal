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
			bindings[node.getID()] = new SceneNodeBinding(node);

		for(SceneNodeIterator i = node.getChildren().begin(); i != node.getChildren().end(); ++i)
		{
			bindSceneNodes(bindings, **i);
		}
	}

	Animator::~Animator()
	{
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
			delete i->second;
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
			float frame = getFloatParam(message.getParam());			
				_timer.reset(frame);
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
		int animationDuration = animation.getDuration();
		float currentFrame = currentTime * FPS;

		if(currentFrame > animationDuration && !animation.Repeat())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));			
			return;
		}
		
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			SceneNodeBinding& binding = **i;
			float relativeFrame = fmod(currentFrame, animationDuration);
			Direction::Enum direction = Direction::FORWARD;
			if(animation.Rewind())
			{
				relativeFrame = animationDuration - relativeFrame;
				direction = Direction::BACKWARD;
			}
			const SceneNodeSample* currentSample = binding.getSample();
			while(currentSample->getParent().getFrame() > relativeFrame || currentSample->getNext()->getParent().getFrame() < relativeFrame)
				currentSample = currentSample->getSibling(direction);
			binding.setSample(currentSample);
			const SceneNodeSample* nextSample = currentSample->getNext();
			float sampleOffset = relativeFrame - currentSample->getParent().getFrame();
			float sampleDuration = nextSample->getParent().getFrame() - currentSample->getParent().getFrame();
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
		const Animation& animation = _animationSet->get(_animationId);
		const SceneGraphSample& sceneGraphSample = **animation.getSamples().begin();
		_timer.reset();
		_animationId = animationId;
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
			SceneNodeBinding& binding = **i;
			const SceneNodeSample* sceneNodeSample = sceneGraphSample.getSamples().at(binding.getSceneNode().getID());
			binding.setSample(sceneNodeSample);
		}
		update();
	}
}