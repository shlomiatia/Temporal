#ifndef ANIMATION_UTILS_H
#define ANIMATION_UTILS_H

#include "Vector.h"
#include "Color.h"

namespace Temporal
{
	class AnimationUtils
	{
	public:
		static float transition(float interpolation, float startValue, float endValue);
		static Vector transition(float interpolation, const Vector& startValue, const Vector& endValue);
		static Color transition(float interpolation, const Color& startValue, const Color& endValue);

	};
}

#endif