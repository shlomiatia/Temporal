#ifndef PHYSICSUTILS_H
#define PHYSICSUTILS_H

#include <Temporal\Base\BaseEnums.h>

namespace Temporal
{
	class Rect;

	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Rect& boundsB);
}
#endif