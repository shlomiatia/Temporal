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
	const float FPS = 15;
	const float Animator::CROSS_FADE_DURATION = 0.15f;

	float frameToTime(float frame) { return frame / FPS; }
	float timeToFrame(float time) { return time * FPS; }

	float easeInOutBezier(float interpolation, float startValue, float endValue)
	{
		float inverseInterpolation = 1 - interpolation;
		return startValue * inverseInterpolation + endValue * interpolation;

		// TODO: Check if it's usefull. If so, interpolation between aim/fire should be linear
		/*return startValue * (powf(inverseInterpolation, 3.0f) + 3 * powf(inverseInterpolation, 2.0f) * interpolation) + 
			   endValue * (powf(interpolation, 3.0f) + 3 * powf(interpolation, 2.0f) * inverseInterpolation);*/
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

	void lerp(float interpolation, const SceneNode& sceneNode, float sourceRotation, float targetRotation, const Vector& sourceTranslation, const Vector& targetTranslation, SRT& result)
	{
		sourceRotation = AngleUtils::degree().normalize(sourceRotation);
		targetRotation = AngleUtils::degree().normalize(targetRotation);
		targetRotation = getTargetRotation(sceneNode, sourceRotation, targetRotation);
		float rotation = easeInOutBezier(interpolation, sourceRotation, targetRotation);

		Vector translation(
			easeInOutBezier(interpolation, sourceTranslation.getX(), targetTranslation.getX()),
			easeInOutBezier(interpolation, sourceTranslation.getY(), targetTranslation.getY()));

		result.setRotation(rotation);
		result.setTranslation(translation);
	}

	void SingleAnimator::reset(Hash animationId, bool isRewind, float weight, float normalizedTime)
	{ 
		_animationId = animationId;
		_isRewind = isRewind;
		_weight = weight;
		if(normalizedTime != -1.0f)
		{
			float time = frameToTime(normalizedTime * _owner->getAnimation(_animationId).getFrames());	
			_timer.reset(time); 
		}
	}

	float SingleAnimator::getNormalizedTime() const
	{
		return timeToFrame(getTime()) / _owner->getAnimation(_animationId).getFrames();
	}

	bool SingleAnimator::animate(const SceneNode& sceneNode, SRT& srt)
	{
		const Animation& animation = _owner->getAnimation(_animationId);
		float time = _timer.getElapsedTime();
		float frame = timeToFrame(time);
		int animationDuration = animation.getFrames();

		// If animation doesn't repeat, clamp it. Otherwise, calculate cyclic index
		float relativeIndex = (frame >= animationDuration && !animation.isRepeat()) ? animationDuration : fmod(frame, animationDuration);
		Direction::Enum direction = Direction::FORWARD;

		// If animation rewind, calculate mirror index
		if(_isRewind)
		{
			relativeIndex = animationDuration - relativeIndex;
			direction = Direction::BACKWARD;
		}
		const HashSceneNodeSampleMap& sceneGraphSampleList = (**animation.getSamples().begin()).getSamples();
		SceneNodeSampleIterator sceneNodeSampleIterator = sceneGraphSampleList.find(sceneNode.getID());
		if(sceneNodeSampleIterator == sceneGraphSampleList.end() || sceneNodeSampleIterator->second->isIgnore())
			return false;
		const SceneNodeSample* currentSample = sceneNodeSampleIterator->second;
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
		
		float interpolation = animationDuration == 0 ? 0 : (sampleDuration == 0 ? sampleOffset / animationDuration : sampleOffset / sampleDuration);

		lerp(interpolation, sceneNode, currentSample->getRotation(), nextSample->getRotation(), currentSample->getTranslation(), nextSample->getTranslation(), srt);
		//srt.setSpriteGroupId(currentSample->getSpriteGroupId());

		return true;
	}

	bool SingleAnimator::isEnded() const
	{
		const Animation& animation = _owner->getAnimation(_animationId);
		int animationDuration = animation.getFrames();
		float time = _timer.getElapsedTime();
		float frame = timeToFrame(time);
		return frame >= animationDuration && !animation.isRepeat();
	}

	bool SingleAnimator::isCrossFade() const
	{
		const Animation& animation = _owner->getAnimation(_animationId);
		return animation.isCrossFade();
	}

	CompositeAnimator::CompositeAnimator(const Animator& owner)
	{
		for(int i = 0; i < 3; ++i)
			_singleAnimators.push_back(new SingleAnimator(owner));
	}

	CompositeAnimator::~CompositeAnimator()
	{
		for(SingleAnimatorIterator i = _singleAnimators.begin(); i != _singleAnimators.end(); ++i)
		{
			delete *i;
		}
	}

	void CompositeAnimator::reset(Hash animationId, bool isRewind, int layer, float weight, float normalizedTime)
	{
		_singleAnimators[layer]->reset(animationId, isRewind, weight, normalizedTime);
		if(layer == 0)
		{
			for(SingleAnimatorIterator i = _singleAnimators.begin() + 1; i != _singleAnimators.end(); ++i)
				(**i).reset();
		}
	}

	bool CompositeAnimator::isEnded() const
	{
		for(SingleAnimatorIterator i = _singleAnimators.begin(); i != _singleAnimators.end(); ++i)
		{
			const SingleAnimator& animator = (**i);
			if(animator.getAnimationId() != Hash::INVALID && animator.isEnded())
				return true;
		}
		return false;
	}

	void CompositeAnimator::setTime(float time)
	{
		for(SingleAnimatorIterator i = _singleAnimators.begin(); i != _singleAnimators.end(); ++i)
			(**i).setTime(time);
	}

	void CompositeAnimator::update(float time)
	{
		for(SingleAnimatorIterator i = _singleAnimators.begin(); i != _singleAnimators.end(); ++i)
			(**i).update(time);
	}

	bool CompositeAnimator::animate(const SceneNode& sceneNode, SRT& srt)
	{
		
		bool result = _singleAnimators[0]->animate(sceneNode, srt);
		for(SingleAnimatorIterator i = _singleAnimators.begin() + 1; i != _singleAnimators.end() && (**i).getAnimationId() != Hash::INVALID; ++i)
		{
			SingleAnimator& singleAnimator = **i;
			SRT temp;
			if(singleAnimator.animate(sceneNode, temp))
			{
				lerp(singleAnimator.getWeight(), sceneNode, srt.getRotation(), temp.getRotation(), srt.getTranslation(), temp.getTranslation(), srt);
			}
		}
		return result;
	}

	void createSceneNodesList(SceneNodeList& sceneNodes, SceneNode* node)
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
			reset(AnimationParams(Hash("POP_ANM_BASE")));
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
			getCurrentAnimator().setTime(time);
		}
		else if(message.getID() == MessageID::GET_ANIMATION_NORMALIZED_TIME)
		{
			*static_cast<float*>(message.getParam()) = getCurrentAnimator().getNormalizedTime();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_isPaused)
			{
				float framePeriod = getFloatParam(message.getParam());
				getCurrentAnimator().update(framePeriod);
			}

			update();
		}
	}

	void Animator::reset(AnimationParams& animationParams)
	{
		const Animation& nextAnimation = _animationSet->get(animationParams.getAnimationId());
		bool mainLayer = animationParams.getLayer() == 0.0f;
		bool previousCrossFadeFinished = getCurrentAnimator().getTime() > CROSS_FADE_DURATION || !getPreviousAnimator().isCrossFade();
		bool isCrossFade = !_isDisableCrossFade && getCurrentAnimator().isCrossFade() && nextAnimation.isCrossFade();
		if(mainLayer)
		{
			if(!isCrossFade)
			{
				getPreviousAnimator().reset();
			}
			else if(previousCrossFadeFinished || !getPreviousAnimator().isActive())
			{
				_useAnimator2 = !_useAnimator2;
			}
		}
		float normalizedTime = -1.0f;
		if(!mainLayer || !isCrossFade || previousCrossFadeFinished)
		{
			normalizedTime = animationParams.getNormalizedTime();
		}
		getCurrentAnimator().reset(animationParams.getAnimationId(), animationParams.isRewind(), animationParams.getLayer(), animationParams.getWeight(), normalizedTime);
		if(mainLayer)
			update();
	}

	void Animator::update()
	{
		for(SceneNodeIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			SceneNode& sceneNode = **i;

			SRT currentSRT;

			if(!getCurrentAnimator().animate(sceneNode, currentSRT))
				continue;
			
			float time = getCurrentAnimator().getTime();
			if(isCrossFade() && time <= CROSS_FADE_DURATION)
			{
 				SRT previousSRT;
				getPreviousAnimator().animate(sceneNode, previousSRT);

				float interpolation = time / CROSS_FADE_DURATION;

				lerp(interpolation, sceneNode, previousSRT.getRotation(), currentSRT.getRotation(), previousSRT.getTranslation(), currentSRT.getTranslation(), currentSRT);
			}

			/*if(currentSRT.getSpriteGroupId() != Hash::INVALID)
				sceneNode.setSpriteGroupId(currentSRT.getSpriteGroupId());
			sceneNode.setSpriteInterpolation(interpolation);*/
			sceneNode.setRotation(currentSRT.getRotation());
			sceneNode.setTranslation(currentSRT.getTranslation());
		}

		if(getCurrentAnimator().isEnded())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));
		}
	}

	bool Animator::isCrossFade() const
	{
		return !_isDisableCrossFade && getCurrentAnimator().isCrossFade() && getPreviousAnimator().isCrossFade();
	}
}