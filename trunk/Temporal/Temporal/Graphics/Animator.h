#pragma once

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class ResetAnimationParams;
	class SpriteGroup;

	class Animator : public Component
	{
	public:
		Animator(float framePeriod) : FRAME_PERIOD(framePeriod), _rewind(false), _repeat(false), _frameID(0) {}
		
		virtual ComponentType::Enum getType(void) const { return ComponentType::ANIMATOR; }

		virtual void handleMessage(Message& message);
		void update(float framePeriodInMillis);

	private:
		const float FRAME_PERIOD;

		bool _rewind;
		bool _repeat;
		int _frameID;
		Timer _timer;

		void reset(const ResetAnimationParams& resetAnimationParams);
		const SpriteGroup& getSpriteGroup(void) const;
	};
}