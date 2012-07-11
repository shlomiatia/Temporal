#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Timer.h"
#include "Component.h"
#include "Hash.h"
#include "SceneNodeSample.h"

namespace Temporal
{
	class ResetAnimationParams;

	class Animator : public Component
	{
	public:
		explicit Animator(const AnimationCollection& animations) :
		_animations(animations), _animationId(Hash::INVALID), _rewind(false), _repeat(false) {}
		
		ComponentType::Enum getType(void) const { return ComponentType::ANIMATOR; }

		void handleMessage(Message& message);
		void update(float framePeriodInMillis);

	private:
		const AnimationCollection& _animations;

		Hash _animationId;
		bool _rewind;
		bool _repeat;
		Timer _timer;

		void reset(const ResetAnimationParams& resetAnimationParams);
	};
}
#endif