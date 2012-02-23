#pragma once

#include <Temporal/Game/Enums.h>

namespace Temporal
{
	class ResetAnimationParams
	{
	public:
		ResetAnimationParams(AnimationID::Enum animationID, bool rewind = false, bool repeat = false) : _animationID(animationID), _rewind(rewind), _repeat(repeat) {}

		AnimationID::Enum getAnimationID(void) const { return _animationID; }
		bool getRewind(void) const { return _rewind; }
		bool getRepeat(void) const { return _repeat; }

	private:
		const AnimationID::Enum _animationID;
		const bool _rewind;
		const bool _repeat;
	};

	class RayCastParams
	{
	public:
		RayCastParams(Vector target) : _target(target), _result(false) {}

		const Vector& getTarget(void) const { return _target; }
		bool getResult(void) const { return _result; }
		void setResult(bool result) { _result = result; }
	private:
		Vector _target;
		bool _result;
		
	};
}