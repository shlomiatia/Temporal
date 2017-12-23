#ifndef PHYSICSENUMS_H
#define PHYSICSENUMS_H

namespace Temporal
{
	namespace CollisionCategory
	{
		enum Enum
		{
			NONE = 0,
			STATIC_OBSTACLE = 1,
			DYNAMIC_OBSTACLE = 2,
			OBSTACLE = 3,
			CHARACTER = 4,
			PLAYER = 8,
			BUTTON = 16,
			PATROL_CONTROL = 32,
			MOVING_PLATFORM_CONTROL = 64,
			DRAGGABLE = 128,
			DEAD = 256,
			SECURITY_CAMERA_TARGET = 512,
			COVER = 1024,
		};
	}
}

#endif