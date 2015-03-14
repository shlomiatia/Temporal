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
			_currentAnimator.getTimer().reset(time);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_isPaused)
			{
				float framePeriod = getFloatParam(message.getParam());
				_currentAnimator.getTimer().update(framePeriod);
				// For load
				/*if(_animationId != _memoryAnimationId)
				{
					reset(AnimationParams(_animationId, _isRewined));
					_previousAnimationId = Hash::INVALID;
					_previousIsRewined = false;
					_previousTimer.reset();
				}*/
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

	void Animator::update()
	{
		for(SceneNodeIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			SceneNode& sceneNode = **i;

			SRT currentSRT;

			if(!_currentAnimator.animate(sceneNode, currentSRT))
				continue;
			
			float time = _currentAnimator.getTimer().getElapsedTime();
			if(_crossFade &&  time <= CROSS_FADE_DURATION)
			{
 				SRT previousSRT;
				_previousAnimator.animate(sceneNode, previousSRT);

				float animationsInterpolation = time / CROSS_FADE_DURATION;

				Vector translation(
					easeInOutBezier(animationsInterpolation, previousSRT.getTranslation().getX(), currentSRT.getTranslation().getX()),
					easeInOutBezier(animationsInterpolation, previousSRT.getTranslation().getY(), currentSRT.getTranslation().getY()));

				float previousRotation = AngleUtils::degree().normalize(previousSRT.getRotation());
				float rotation = AngleUtils::degree().normalize(currentSRT.getRotation());
				rotation = getTargetRotation(sceneNode, previousRotation, rotation);
				rotation = easeInOutBezier(animationsInterpolation, previousRotation, rotation);

				currentSRT.setRotation(rotation);
				currentSRT.setTranslation(translation);
			}

			/*if(currentSRT.getSpriteGroupId() != Hash::INVALID)
				sceneNode.setSpriteGroupId(currentSRT.getSpriteGroupId());
			sceneNode.setSpriteInterpolation(interpolation);*/
			sceneNode.setRotation(currentSRT.getRotation());
			sceneNode.setTranslation(currentSRT.getTranslation());
		}

		if(_currentAnimator.isEnded())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));
		}
	}

	void Animator::reset(AnimationParams& animationParams)
	{
		const Animation& nextAnimation = _animationSet->get(animationParams.getAnimationId());
		bool previousCrossFadeFinished = _currentAnimator.getTimer().getElapsedTime() > CROSS_FADE_DURATION;
		_crossFade = _currentAnimator.getAnimation() && _currentAnimator.getAnimation()->isCrossFade() && nextAnimation.isCrossFade();
		if(!_crossFade)
		{
			_previousAnimator.reset(0, false);
		}
		else if(previousCrossFadeFinished || !_previousAnimator.getAnimation())
		{
			_previousAnimator = _currentAnimator;
		}
		_currentAnimator.reset(&nextAnimation, animationParams.isRewind());
		if(!_crossFade || previousCrossFadeFinished)
		{
			float time = frameToTime(animationParams.getInterpolation() * nextAnimation.getDuration());	
			_currentAnimator.getTimer().reset(time);
		}

		update();
	}

	bool SingleAnimator::animate(const SceneNode& sceneNode, SRT& srt)
	{
		float time = _timer.getElapsedTime();
		float frame = time * 15.0f;
		int animationDuration = _animation->getDuration();

		// If animation doesn't repeat, clamp it. Otherwise, calculate cyclic index
		float relativeIndex = (frame >= animationDuration && !_animation->isRepeat()) ? animationDuration : fmod(frame, animationDuration);
		Direction::Enum direction = Direction::FORWARD;

		// If animation rewind, calculate mirror index
		if(_isRewind)
		{
			relativeIndex = animationDuration - relativeIndex;
			direction = Direction::BACKWARD;
		}
		const SceneNodeSampleCollection& sceneGraphSampleCollection = (**_animation->getSamples().begin()).getSamples();
		SceneNodeSampleIterator sceneNodeSampleIterator = sceneGraphSampleCollection.find(sceneNode.getID());
		if(sceneNodeSampleIterator == sceneGraphSampleCollection.end())
			return false;
		const SceneNodeSample* currentSample = sceneNodeSampleIterator->second;
		while(!(relativeIndex >= currentSample->getParent().getIndex() && 
		       (relativeIndex < currentSample->getNext()->getParent().getIndex() ||
			    currentSample->getParent().getIndex() >= currentSample->getNext()->getParent().getIndex())))
		{
			currentSample = currentSample->getSibling(direction);
		}
			   
		// If animation doesn't repeat, use current sample for both samples. Otherwise use current and next
		const SceneNodeSample* nextSample = (currentSample->getParent().getIndex() > currentSample->getNext()->getParent().getIndex() && !_animation->isRepeat()) ? currentSample : currentSample->getNext();
		float sampleOffset = relativeIndex - currentSample->getParent().getIndex();
		int sampleDuration = animationDuration == 0 ? 0 : (animationDuration + nextSample->getParent().getIndex() - currentSample->getParent().getIndex()) % animationDuration;
		
		float interpolation = animationDuration == 0 ? 0 : (sampleDuration == 0 ? sampleOffset / animationDuration : sampleOffset / sampleDuration);
		
		Vector translation(
			easeInOutBezier(interpolation, currentSample->getTranslation().getX(), nextSample->getTranslation().getX()),
			easeInOutBezier(interpolation, currentSample->getTranslation().getY(), nextSample->getTranslation().getY()));
		
		float sourceRotation = currentSample->getRotation();
		float targetRotation = nextSample->getRotation();
		targetRotation = getTargetRotation(sceneNode, sourceRotation, targetRotation);
		float rotation = easeInOutBezier(interpolation, sourceRotation, targetRotation);

		srt.setRotation(rotation);
		srt.setTranslation(translation);
		//srt.setSpriteGroupId(currentSample->getSpriteGroupId());
		return true;
	}

	bool SingleAnimator::isEnded() const
	{
		int animationDuration = _animation->getDuration();
		float time = _timer.getElapsedTime();
		float frame = time * 15.0f;
		return frame >= animationDuration && !_animation->isRepeat();
	}
}