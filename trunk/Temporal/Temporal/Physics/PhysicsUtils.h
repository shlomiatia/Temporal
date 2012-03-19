#pragma once

#include <Temporal\Base\Enums.h>

namespace Temporal
{
	class Rect;

	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Rect& boundsB);
}