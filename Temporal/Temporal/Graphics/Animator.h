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
		Animator(void) : _rewind(false), _repeat(false), _frameID(0), _update(0) {}
		
		virtual ComponentType::Type getType(void) const { return ComponentType::ANIMATOR; }

		virtual void handleMessage(Message& message);
		void update(void);
	private:
		// TODO: Use time instad
		static const int UPDATES_PER_FRAME = 4;

		bool _rewind;
		bool _repeat;
		int _frameID;
		int _update;

		void reset(const ResetAnimationParams& resetAnimationParams);
		const SpriteGroup& getSpriteGroup(void) const;

		Animator(const Animator&);
		Animator& operator=(const Animator&);
	};
}