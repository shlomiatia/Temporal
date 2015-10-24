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
		explicit SRT(float rotation = 0.0f, Vector translation = Vector::Zero, Hash spriteGroupId = Hash::INVALID) : _rotation(rotation), _translation(translation), _spriteGroupId(spriteGroupId) {}

		float getRotation() const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }
		const Vector& getTranslation() const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }
		Hash getSpriteGroupId() const { return _spriteGroupId; }
		void setSpriteGroupId(Hash spriteGroupId) { _spriteGroupId = spriteGroupId; }

	private:
		float _rotation;
		Vector _translation;
		Hash _spriteGroupId;

		SRT(const SRT&);
		SRT& operator=(const SRT&);
	};

	class SingleAnimator
	{
	public:
		SingleAnimator() : _owner(0), _animationId(Hash::INVALID), _isRewind(false), _weight(1.0f) {}

		void init(const Animator* owner) { _owner = owner; }
		void reset(Hash animationId = Hash::INVALID, bool isRewind = false, float weight = 1.0f, float normalizedTime = -1.0f);
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

	typedef std::vector<SingleAnimator*> SingleAnimatorList;
	typedef SingleAnimatorList::const_iterator SingleAnimatorIterator;

	class CompositeAnimator
	{
	public:
		CompositeAnimator() {};
		~CompositeAnimator();
		
		void init(const Animator& animator);
		float getTime() const { return _singleAnimators[0]->getTime(); }
		float getNormalizedTime() const { return _singleAnimators[0]->getNormalizedTime(); }
		bool isActive() const { return _singleAnimators[0]->getAnimationId() != Hash::INVALID; }
		bool isCrossFade() const { return isActive() && _singleAnimators[0]->isCrossFade(); };

		void reset(Hash animationId = Hash::INVALID, bool isRewind = false, int layer = 0, float weight = 1.0f, float normalizedTime = -1.0f);
		void setTime(float time);
		bool isEnded() const;
		void update(float time);
		bool animate(const SceneNode& sceneNode, SRT& srt);
		
	private:
		SingleAnimatorList _singleAnimators;

		CompositeAnimator(const CompositeAnimator&);
		CompositeAnimator& operator=(const CompositeAnimator&);

		friend class SerializationAccess;
	};

	class Animator : public Component
	{
	public:
		Animator(const char* animationSetFile = "") : _animationSetFile(animationSetFile), _isPaused(false), _useAnimator2(false), _isDisableCrossFade(false) {}
		
		const Animation& getAnimation(Hash animationId) const { return _animationSet->get(animationId); }
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile.c_str()); }

		static const Hash TYPE;
	private:
		static const float CROSS_FADE_DURATION;

		std::string _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeList _sceneNodes;
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