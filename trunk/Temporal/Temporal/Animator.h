#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Timer.h"
#include "EntitySystem.h"
#include "Hash.h"
#include "Animation.h"
#include "SceneNode.h"
#include <memory>
#include <vector>

namespace Temporal
{
	class Animator;
	class ResetAnimationParams;
	class SceneNode;
	class AnimationParams;

	class SRT
	{
	public:
		explicit SRT(float rotation = 0.0f, Vector translation = Vector::Zero) : _rotation(rotation), _translation(translation) {}

		float getRotation() const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }
		const Vector& getTranslation() const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }

	private:
		float _rotation;
		Vector _translation;

		SRT(const SRT&);
		SRT& operator=(const SRT&);
	};

	class SingleAnimator
	{
	public:
		SingleAnimator() : _owner(0) {}
		explicit SingleAnimator(const Animator& owner) : _owner(&owner), _animationId(Hash::INVALID), _isRewind(false), _weight(1.0f) {}

		void reset(Hash animationId = Hash::INVALID, bool isRewind = false, float weight = 1.0f, float time = -1.0f) 
			{ _animationId = animationId; _isRewind = isRewind; _weight = weight; if(time != -1.0f) _timer.reset(time); }
		Hash getAnimationId() const { return _animationId; }
		float getTime() const { return _timer.getElapsedTime(); }
		float getNormalizedTime() const;
		void setTime(float time) { _timer.reset(time); }
		float getWeight() const { return _weight; }
		void update(float time) { _timer.update(time); }
		
		bool animate(const SceneNode& sceneNode, SRT& srt);
		bool isEnded() const;
		bool isCrossFade() const;

	private:
		const Animator* _owner;
		Hash _animationId;
		bool _isRewind;
		float _weight;
		Timer _timer;

		SingleAnimator(const SingleAnimator&);
		SingleAnimator& operator=(const SingleAnimator&);

		friend class SerializationAccess;
	};

	typedef std::vector<SingleAnimator*> SingleAnimatorCollection;
	typedef SingleAnimatorCollection::const_iterator SingleAnimatorIterator;

	class CompositeAnimator
	{
	public:
		CompositeAnimator(const Animator& owner);
		~CompositeAnimator();
		
		float getTime() const { return _singleAnimators[0]->getTime(); }
		float getNormalizedTime() const { return _singleAnimators[0]->getNormalizedTime(); }
		bool isActive() const { return _singleAnimators[0]->getAnimationId() != Hash::INVALID; }
		bool isCrossFade() const { return isActive() && _singleAnimators[0]->isCrossFade(); };

		void reset(Hash animationId = Hash::INVALID, bool isRewind = false, int layer = 0, float weight = 1.0f, float time = -1.0f);
		void setTime(float time);
		bool isEnded() const;
		void update(float time);
		bool animate(const SceneNode& sceneNode, SRT& srt);
		
	private:
		SingleAnimatorCollection _singleAnimators;

		CompositeAnimator(const CompositeAnimator&);
		CompositeAnimator& operator=(const CompositeAnimator&);

		friend class SerializationAccess;
	};

	class Animator : public Component
	{
	public:
		Animator(const char* animationSetFile = "") : _animationSetFile(animationSetFile), _isPaused(false), _useAnimator2(false), _isDisableCrossFade(false), _animator1(*this), _animator2(*this) {}
		
		const Animation& getAnimation(Hash animationId) const { return _animationSet->get(animationId); }
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile.c_str()); }

		static const Hash TYPE;
	private:
		static const float CROSS_FADE_DURATION;

		std::string _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeCollection _sceneNodes;
		bool _isPaused;
		bool _isDisableCrossFade;
		bool _useAnimator2;
		CompositeAnimator _animator1;
		CompositeAnimator _animator2;

		CompositeAnimator& getPreviousAnimator() { return _useAnimator2 ? _animator1 : _animator2; }
		const CompositeAnimator& getPreviousAnimator() const { return _useAnimator2 ? _animator1 : _animator2; }
		CompositeAnimator& getCurrentAnimator() { return _useAnimator2 ? _animator2 : _animator1; }
		const CompositeAnimator& getCurrentAnimator() const { return _useAnimator2 ? _animator2 : _animator1; }
		bool isCrossFade() const;
		void update();
		void reset(AnimationParams& animationParams);
		
		friend class SerializationAccess;
	};
}
#endif