#pragma once

#include <Temporal\Game\Enums.h>

namespace Temporal
{
	class ResetAnimationParams
	{
	public:
		ResetAnimationParams(int animationID, bool rewind = false, bool repeat = false) : _animationID(animationID), _rewind(rewind), _repeat(repeat) {}

		int getAnimationID(void) const { return _animationID; }
		bool getRewind(void) const { return _rewind; }
		bool getRepeat(void) const { return _repeat; }

	private:
		const int _animationID;
		const bool _rewind;
		const bool _repeat;

		ResetAnimationParams(const ResetAnimationParams&);
		ResetAnimationParams& operator=(const ResetAnimationParams&);
	};
}