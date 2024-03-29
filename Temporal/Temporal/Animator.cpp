#include "Animator.h"
#include "SpriteSheet.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include "Math.h"
#include "AnimationUtils.h"
#include <math.h>

namespace Temporal
{
	const float FPS = 15;
	const float Animator::CROSS_FADE_DURATION = 0.15f;

	float frameToTime(float frame) { return frame / FPS; }
	float timeToFrame(float time) { return time * FPS; }

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
		float rotation = AnimationUtils::transition(interpolation, sourceRotation, targetRotation);

		Vector translation = AnimationUtils::transition(interpolation, sourceTranslation, targetTranslation);

		result.setRotation(rotation);
		result.setTranslation(translation);
	}

	void SingleAnimator::reset(Hash animationId, bool isRewind, float weight, float normalizedTime)
	{ 
		_animationId = animationId;
		_isRewind = isRewind;
		_weight = weight;
		int frames = animationId == Hash::INVALID ? 0 : _owner->getAnimation(_animationId).getFrames();
		float time = frameToTime(normalizedTime * frames);
		_timer.reset(time);
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
		srt.setSpriteGroupId(currentSample->getSpriteGroupId());

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

	void CompositeAnimator::init(const Animator& owner)
	{
		while(_singleAnimators.size() < 3)
			_singleAnimators.push_back(new SingleAnimator());
		for (int i = 0; i < _singleAnimators.size(); ++i)
			_singleAnimators[i]->init(&owner);
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
			_animator.init(*this);
		}
		else if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			SceneNode* sceneNode = static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			createSceneNodesList(_sceneNodes, sceneNode);
			update();
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
			_animator.setTime(time);
		}
		else if(message.getID() == MessageID::GET_ANIMATION_NORMALIZED_TIME)
		{
			*static_cast<float*>(message.getParam()) = _animator.getNormalizedTime();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_isPaused)
			{
				float framePeriod = getFloatParam(message.getParam());
				_animator.update(framePeriod * _scale);
			}

			update();
		}
	}

	void Animator::reset(AnimationParams& animationParams)
	{
		bool mainLayer = animationParams.getLayer() == 0;
		for (SceneNodeIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			SceneNode& sceneNode = **i;
			SRT& srt = _previous[sceneNode.getID()];
			srt.setRotation(sceneNode.getRotation());
			srt.setTranslation(sceneNode.getTranslation());
			srt.setSpriteGroupId(sceneNode.getSpriteGroupId());
		}
		_isCrossFade = _isInitialized && !_isDisableCrossFade && _animator.isCrossFade() && getAnimation(animationParams.getAnimationId()).isCrossFade();
		_isInitialized = true;
		_animator.reset(animationParams.getAnimationId(), animationParams.isRewind(), animationParams.getLayer(), animationParams.getWeight(), animationParams.getNormalizedTime());
		if(mainLayer)
			update();
	}

	void Animator::update()
	{
		for(SceneNodeIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			SceneNode& sceneNode = **i;

			SRT currentSRT;

			if(!_animator.animate(sceneNode, currentSRT))
				continue;
			
			float time = _animator.getTime();
			if (_isCrossFade && time <= CROSS_FADE_DURATION)
			{
				SRT& previousSRT = _previous[sceneNode.getID()];
 				
				float interpolation = time / CROSS_FADE_DURATION;

				lerp(interpolation, sceneNode, previousSRT.getRotation(), currentSRT.getRotation(), previousSRT.getTranslation(), currentSRT.getTranslation(), currentSRT);
			}

			if(currentSRT.getSpriteGroupId() != Hash::INVALID)
				sceneNode.setSpriteGroupId(currentSRT.getSpriteGroupId());
			/*sceneNode.setSpriteInterpolation(interpolation);*/
			sceneNode.setRotation(currentSRT.getRotation());
			sceneNode.setTranslation(currentSRT.getTranslation());
		}

		if(_animator.isEnded())
		{
			raiseMessage(Message(MessageID::ANIMATION_ENDED));
		}
	}
}