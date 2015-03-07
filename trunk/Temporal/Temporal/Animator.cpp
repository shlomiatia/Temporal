#include "Animator.h"
#include "SpriteSheet.h"
#include "Serialization.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include "Math.h"
#include <math.h>

namespace Temporal
{
	const Hash Animator::TYPE = Hash("animator");
	const float Animator::FPS = 15;
	const float Animator::CROSS_FADE_DURATION = 0.15f;
	
	void createSceneNodesList(SceneNodeCollection& sceneNodes, SceneNode* node)
	{
		if(!node->isTransformOnly())
			sceneNodes.push_back(node);

		for(SceneNodeIterator i = node->getChildren().begin(); i != node->getChildren().end(); ++i)
		{
			createSceneNodesList(sceneNodes, *i);
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
			SceneNode* sceneNode = static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			createSceneNodesList(_sceneNodes, sceneNode);
		}
		else if(message.getID() == MessageID::RESET_ANIMATION)
		{
			AnimationParams& params = getAnimationParams(message.getParam());
			reset(params);
		}
		else if(message.getID() == MessageID::TOGGLE_ANIMATION)
		{
			_isPaused = getBoolParam(message.getParam());
		}
		else if(message.getID() == MessageID::SET_ANIMATION_FRAME)
		{
			int index = getIntParam(message.getParam());
			float time = frameToTime(static_cast<float>(index));
			_timer.reset(time);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_isPaused)
			{
				float framePeriod = getFloatParam(message.getParam());
				_timer.update(framePeriod);
				// For load
				if(_animationId != _memoryAnimationId)
				{
					reset(AnimationParams(_animationId, _isRewined));
					_previousAnimationId = Hash::INVALID;
					_previousIsRewined = false;
					_previousTimer.reset();
				}
			}

			update();
		}
	}

	float easeInOutBezier(float interpolation, float startValue, float endValue)
	{
		float inverseInterpolation = 1 - interpolation;
		return startValue * (powf(inverseInterpolation, 3.0f) + 3 * powf(inverseInterpolation, 2.0f) * interpolation) + 
			   endValue * (powf(interpolation, 3.0f) + 3 * powf(interpolation, 2.0f) * inverseInterpolation);
	}

	float getTargetRotation(const SceneNode& sceneNode, float sourceRotation, float targetRotation)
	{
		bool notShortestPath = fabsf(targetRotation - sourceRotation) > 180.0f;

		float clockwiseDist = AngleUtils::degree().clockwiseDistance(sourceRotation, targetRotation);
		float counterclockwiseDist = AngleUtils::degree().counterclockwiseDistance(sourceRotation, targetRotation);
		bool clockwise = clockwiseDist < counterclockwiseDist;
		float minDist = clockwise ? clockwiseDist : counterclockwiseDist;
		float centerDist = clockwise ? AngleUtils::degree().clockwiseDistance(sceneNode.getCenter(), sourceRotation) : AngleUtils::degree().counterclockwiseDistance(sceneNode.getCenter(), sourceRotation);
		bool outsideBoneRange = (sceneNode.getRadius() > 0.0f && centerDist < 180.0f && centerDist + minDist > sceneNode.getRadius());

		// XOR
		if(notShortestPath != outsideBoneRange)
		{
			targetRotation = AngleUtils::degree().turn(targetRotation);
		}
		return targetRotation;
	}

	void animateSceneNode(const Animation& animation, bool isRewind, float currentIndex, const SceneNode& sceneNode, Hash& spriteGroupId, float& interpolation, Vector& translation, float& rotation)
	{
		int animationDuration = animation.getDuration();

		// If animation doesn't repeat, clamp it. Otherwise, calculate cyclic index
		float relativeIndex = (currentIndex >= animationDuration && !animation.isRepeat()) ? animationDuration : fmod(currentIndex, animationDuration);
		Direction::Enum direction = Direction::FORWARD;

		// If animation rewind, calculate mirror index
		if(isRewind)
		{
			relativeIndex = animationDuration - relativeIndex;
			direction = Direction::BACKWARD;
		}
		const SceneGraphSample& sceneGraphSample = **animation.getSamples().begin();
		const SceneNodeSample* currentSample = sceneGraphSample.getSamples().at(sceneNode.getID());
		while(!(relativeIndex >= currentSample->getParent().getIndex() && 
		       (relativeIndex < currentSample->getNext()->getParent().getIndex() ||
			    currentSample->getParent().getIndex() >= currentSample->getNext()->getParent().getIndex())))
		{
			currentSample = currentSample->getSibling(direction);
		}
			   
		// If animation doesn't repeat, use current sample for both samples. Otherwise use current and next
		const SceneNodeSample* nextSample = (currentSample->getParent().getIndex() > currentSample->getNext()->getParent().getIndex() && !animation.isRepeat()) ? currentSample : currentSample->getNext();
		float sampleOffset = relativeIndex - currentSample->getParent().getIndex();
		int sampleDuration = animationDuration == 0 ? 0 : (animationDuration + nextSample->getParent().getIndex() - currentSample->getParent().getIndex()) % animationDuration;

		spriteGroupId = currentSample->getSpriteGroupId();
		interpolation = animationDuration == 0 ? 0 : (sampleDuration == 0 ? sampleOffset / animationDuration : sampleOffset / sampleDuration);
		
		translation.setX(easeInOutBezier(interpolation, currentSample->getTranslation().getX(), nextSample->getTranslation().getX()));
		translation.setY(easeInOutBezier(interpolation, currentSample->getTranslation().getY(), nextSample->getTranslation().getY()));

		float sourceRotation = currentSample->getRotation();
		float targetRotation = nextSample->getRotation();
		
		targetRotation = getTargetRotation(sceneNode, sourceRotation, targetRotation);
		rotation = easeInOutBezier(interpolation, sourceRotation, targetRotation);
	}

	void Animator::update()
	{
		const Animation& animation = _animationSet->get(_animationId);
		float time = _timer.getElapsedTime();
		float frame = timeToFrame(time);
		
		for(SceneNodeIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			SceneNode& sceneNode = **i;

			Hash spriteGroupId;
			float interpolation = 0.0f;;
			Vector translation;
			float rotation = 0.0f;

			animateSceneNode(animation, _isRewined, frame, sceneNode, spriteGroupId, interpolation, translation, rotation);
			
			if(_previousAnimationId != Hash::INVALID && time <= CROSS_FADE_DURATION && animation.isCrossFade() && _animationSet->get(_previousAnimationId).isCrossFade() && !_isDisableCrossFade)
			{
				const Animation& previousAnimation = _animationSet->get(_previousAnimationId);	
				float previousTime = _previousTimer.getElapsedTime();
				float previousFrame = timeToFrame(previousTime);

				Hash previousSpriteGroupId;
				float previousInterpolation = 0.0f;;
				Vector previousTranslation;
				float previousRotation = 0.0f;

				animateSceneNode(previousAnimation, _previousIsRewined, previousFrame, sceneNode, previousSpriteGroupId, previousInterpolation, previousTranslation, previousRotation);

				float animationsInterpolation = time / CROSS_FADE_DURATION;

				translation.setX(easeInOutBezier(animationsInterpolation, previousTranslation.getX(), translation.getX()));
				translation.setY(easeInOutBezier(animationsInterpolation, previousTranslation.getY(), translation.getY()));

				previousRotation = AngleUtils::degree().normalize(previousRotation);
				rotation = AngleUtils::degree().normalize(rotation);
				rotation = getTargetRotation(sceneNode, previousRotation, rotation);
				rotation = easeInOutBezier(animationsInterpolation, previousRotation, rotation);
			}

			if(spriteGroupId != Hash::INVALID)
				sceneNode.setSpriteGroupId(spriteGroupId);
			sceneNode.setSpriteInterpolation(interpolation);
			sceneNode.setTranslation(translation);
			sceneNode.setRotation(rotation);
		}

		int animationDuration = animation.getDuration();
		if(frame >= animationDuration && !animation.isRepeat())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));
		}
	}

	void Animator::reset(AnimationParams& animationParams)
	{
		if(_timer.getElapsedTime() > CROSS_FADE_DURATION)
		{
			_previousAnimationId = _animationId;
			_previousIsRewined = _isRewined;
			_previousTimer.reset(_timer.getElapsedTime());
		}

		_animationId = animationParams.getAnimationId();
		_isRewined = animationParams.isRewind();
		const Animation& animation = _animationSet->get(_animationId);
		float time = frameToTime(animationParams.getInterpolation() * animation.getDuration());
		_timer.reset(time);
		
		_memoryAnimationId = _animationId;

		update();
	}
}