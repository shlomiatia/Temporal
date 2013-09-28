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
<<<<<<< .mine
		if(!node.isTransformOnly())
			bindings.push_back(new SceneNodeBinding(node));

=======
		if(!node.isTransformOnly())
			bindings[node.getID()] = new SceneNodeBinding(node);

>>>>>>> .r343
		for(SceneNodeIterator i = node.getChildren().begin(); i != node.getChildren().end(); ++i)
		{
			bindSceneNodes(bindings, **i);
		}
	}

<<<<<<< .mine
	Animator::~Animator()
	{
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
			delete *i;
	}

=======
	Animator::~Animator()
	{
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
			delete i->second;
	}

>>>>>>> .r343
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
<<<<<<< .mine
		float animationDuration = animation.getDuration();
		float currentIndex = currentTime * FPS;

		if(currentIndex > animationDuration && !animation.Repeat())
=======
		int animationDuration = animation.getDuration();
		float currentFrame = currentTime * FPS;

		if(currentFrame > animationDuration && !animation.Repeat())
>>>>>>> .r343
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));			
			return;
		}
		float relativeIndex = fmod(currentIndex, animationDuration);
			Direction::Enum direction = Direction::FORWARD;
		if(animation.Rewind())
		{
			relativeIndex = animationDuration - relativeIndex;
			direction = Direction::BACKWARD;
		}
		for(SceneNodeBindingIterator i = _bindings.begin(); i != _bindings.end(); ++i)
		{
<<<<<<< .mine
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
=======
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
>>>>>>> .r343
			
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
<<<<<<< .mine
		_animationId = animationId;
		const Animation& animation = _animationSet->get(_animationId);
		const SceneGraphSample& sceneGraphSample = **animation.getSamples().begin();
=======
		const Animation& animation = _animationSet->get(_animationId);
		const SceneGraphSample& sceneGraphSample = **animation.getSamples().begin();
>>>>>>> .r343
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