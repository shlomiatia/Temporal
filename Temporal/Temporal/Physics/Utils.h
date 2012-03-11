#pragma  once

#include <Temporal\Base\Rect.h>

namespace Temporal
{
	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Rect& boundsB);
}