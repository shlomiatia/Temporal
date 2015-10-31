#ifndef PHYSICSENUMS_H
#define PHYSICSENUMS_H

namespace Temporal
{
	namespace CollisionCategory
	{
		enum Enum
		{
			NONE = 0,
			OBSTACLE = 1,
			COVER = 2,
			CHARACTER = 4,
			PLAYER = 8,
			BUTTON = 16,
			PATROL_CONTROL = 32,
			MOVING_PLATFORM_CONTROL = 64,
			DRAGGABLE = 128,
			DEAD = 256,
			SECURITY_CAMERA_TARGET = 512
		};
	}
}

#endif