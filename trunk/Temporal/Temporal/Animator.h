#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Timer.h"
#include "EntitySystem.h"
#include "Hash.h"
#include "Animation.h"
#include "SceneNode.h"
#include <memory>

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
	};

	class SingleAnimator
	{
	public:
		SingleAnimator() : _animation(0), _isRewind(false) {}

		void reset(const Animation* animation, bool isRewind) { _animation = animation; _isRewind = isRewind; }
		const Animation* getAnimation() const { return _animation; }
		Timer& getTimer() { return _timer; }
		bool animate(const SceneNode& sceneNode, SRT& srt);
		bool isEnded() const;

	private:
		const Animation* _animation;
		bool _isRewind;
		Timer _timer;
	};

	class Animator : public Component
	{
	public:
		Animator(const char* animationSetFile = "") : _animationSetFile(animationSetFile), _isPaused(false), _crossFade(false) {}
		
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile.c_str()); }

		static const Hash TYPE;
	private:
		static const float FPS;
		static const float CROSS_FADE_DURATION;

		std::string _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeCollection _sceneNodes;
		bool _isPaused;

		SingleAnimator _currentAnimator;
		SingleAnimator _previousAnimator;
		bool _crossFade;

		friend class SerializationAccess;

		void update();
		void reset(AnimationParams& animationParams);
		float frameToTime(float frame) { return frame / FPS; }
		float timeToFrame(float time) { return time * FPS; }
	};

	/*typedef std::vector<SingleAnimator*> SingleAnimatorCollection;
	typedef SingleAnimatorCollection::const_iterator SingleAnimatorIterator;

	class CompositeAnimator
	{
	public:
		SRT animate(Hash sceneNodeId);
	private:
		SingleAnimatorCollection _singleAnimators;
	};*/
}
#endif