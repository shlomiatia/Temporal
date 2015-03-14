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
		SingleAnimator() : _animation(0), _isRewind(false) {}

		void reset(const Animation* animation = 0, bool isRewind = false) { _animation = animation; _isRewind = isRewind; }
		float getTime() const { return _timer.getElapsedTime(); }
		void setTime(float time) { _timer.reset(time); }
		void update(float time) { _timer.update(time); }
		
		const Animation* getAnimation() const { return _animation; }
		bool animate(const SceneNode& sceneNode, SRT& srt);
		bool isEnded() const;

	private:
		const Animation* _animation;
		bool _isRewind;
		Timer _timer;

		SingleAnimator(const SingleAnimator&);
		SingleAnimator& operator=(const SingleAnimator&);
	};

	typedef std::vector<SingleAnimator*> SingleAnimatorCollection;
	typedef SingleAnimatorCollection::const_iterator SingleAnimatorIterator;

	class CompositeAnimator
	{
	public:
		CompositeAnimator();
		~CompositeAnimator();
		
		float getTime() const { return _singleAnimators[0]->getTime(); }
		bool isEnded() const { return _singleAnimators[0]->isEnded(); }
		const Animation* getAnimation() const { return _singleAnimators[0]->getAnimation(); }

		void reset(const Animation* animation = 0, bool isRewind = false, int layer = 0);
		void setTime(float time);
		void update(float time);
		bool animate(const SceneNode& sceneNode, SRT& srt);
		
		//const Animation* getAnimation() const { return _animation; }

	private:
		SingleAnimatorCollection _singleAnimators;

		CompositeAnimator(const CompositeAnimator&);
		CompositeAnimator& operator=(const CompositeAnimator&);
	};

	class Animator : public Component
	{
	public:
		Animator(const char* animationSetFile = "") : _animationSetFile(animationSetFile), _isPaused(false), _crossFade(false), _useAimator2(false), _isDisableCrossFade(false) {}
		
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

		CompositeAnimator _animator1;
		CompositeAnimator _animator2;
		bool _useAimator2;
		bool _crossFade;

		friend class SerializationAccess;

		CompositeAnimator& getPreviousAnimator() { return _useAimator2 ? _animator1 : _animator2; }
		CompositeAnimator& getCurrentAnimator() { return _useAimator2 ? _animator2 : _animator1; }
		void update();
		void reset(AnimationParams& animationParams);
	};
}
#endif