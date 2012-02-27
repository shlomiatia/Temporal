#pragma once
#include <Temporal\Base\Base.h>
#include <Temporal\Game\Component.h>
#include <Temporal\Game\MessageParams.h>
#include "SpriteGroup.h"

namespace Temporal
{
	class Animator : public Component
	{
	public:
		Animator(float framePeriod) : FRAME_PERIOD(framePeriod), _rewind(false), _repeat(false), _frameID(0), _time(0.0f) {}
		
		virtual ComponentType::Enum getType(void) const { return ComponentType::ANIMATOR; }

		virtual void handleMessage(Message& message);
		void update(float framePeriodInMillis);

	private:
		const float FRAME_PERIOD;

		bool _rewind;
		bool _repeat;
		int _frameID;
		float _time;

		void reset(const ResetAnimationParams& resetAnimationParams);
		const SpriteGroup& getSpriteGroup(void) const;

		Animator(const Animator&);
		Animator& operator=(const Animator&);
	};
}