#include "AnimationUtils.h"

namespace Temporal
{
	float AnimationUtils::transition(float interpolation, float startValue, float endValue)
	{
		float inverseInterpolation = 1 - interpolation;
		return startValue * inverseInterpolation + endValue * interpolation;

		// TODO: Check if it's usefull. If so, interpolation between aim/fire should be linear
		/*return startValue * (powf(inverseInterpolation, 3.0f) + 3 * powf(inverseInterpolation, 2.0f) * interpolation) +
		endValue * (powf(interpolation, 3.0f) + 3 * powf(interpolation, 2.0f) * inverseInterpolation);*/
	}

	Vector AnimationUtils::transition(float interpolation, const Vector& startValue, const Vector& endValue)
	{
		Vector vector(
			transition(interpolation, startValue.getX(), endValue.getX()),
			transition(interpolation, startValue.getY(), endValue.getY()));
		return vector;
	}

	Color AnimationUtils::transition(float interpolation, const Color& startValue, const Color& endValue)
	{
		Color color(
			transition(interpolation, startValue.getR(), endValue.getR()),
			transition(interpolation, startValue.getG(), endValue.getG()),
			transition(interpolation, startValue.getB(), endValue.getB()),
			transition(interpolation, startValue.getA(), endValue.getA()));
		return color;
				
	}
}