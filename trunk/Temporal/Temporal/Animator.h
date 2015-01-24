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

	class Animator : public Component
	{
	public:
		Animator(const char* animationSetFile = "", Hash animationId = Hash::INVALID) :
			_animationSetFile(animationSetFile), _animationId(animationId), _memoryAnimationId(animationId), _previousAnimationId(Hash::INVALID), _isPaused(false), _isRewined(false), _previousIsRewined(false) {}
		
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile.c_str(), _animationId); }

		static const Hash TYPE;
	private:
		static const float FPS;
		static const float CROSS_FADE_DURATION;

		std::string _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeCollection _sceneNodes;
		Timer _timer;
		Hash _animationId;
		bool _isRewined;
		bool _isPaused;

		Hash _memoryAnimationId;

		Timer _previousTimer;
		Hash _previousAnimationId;
		bool _previousIsRewined;

		friend class SerializationAccess;

		void update();
		void reset(AnimationParams& animationParams);
		float frameToTime(float frame) { return frame / FPS; }
		float timeToFrame(float time) { return time * FPS; }
	};
}
#endif