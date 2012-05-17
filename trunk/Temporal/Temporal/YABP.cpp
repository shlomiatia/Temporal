#include "YABP.h"
#include <assert.h>

namespace Temporal
{
	YABP::YABP(const Point& center, const Vector& slopedRadius, float yRadius)
		: _center(center), _slopedRadius(slopedRadius), _yRadius(yRadius)
	{
		assert(getSlopedRadiusVx() >= 0.0f);
		assert(getYRadius() >= 0.0f);
	}

}