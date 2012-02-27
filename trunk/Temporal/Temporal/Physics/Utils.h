#pragma  once

#include <Temporal/Base/Base.h>

namespace Temporal
{
	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Rect& boundsB);
	
}