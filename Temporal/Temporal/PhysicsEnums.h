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
		};
	}
}

#endif