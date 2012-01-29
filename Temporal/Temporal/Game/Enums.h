#pragma once

namespace Temporal
{
	namespace EntityStateID
	{
		enum Type
		{
			STAND,
			FALL,
			WALK,
			TURN,
			PREPARE_TO_JUMP,
			JUMP_START_45,
			JUMP_START_60,
			JUMP_START_75,
			JUMP_START_90,
			JUMP_START_105,
			JUMP_UP,
			JUMP_FORWARD,
			JUMP_FORWARD_END,
			HANGING,
			HANG,
			DROP,
			CLIMBE,
			PREPARE_TO_DESCEND,
			DESCEND
		};
	}

	namespace VisualLayer
	{
		enum Type
		{
			BACKGROUND = 0,
			STATIC = 1,
			DYNAMIC = 2
		};

		static const int FARTHEST = BACKGROUND;
		static const int NEAREST = DYNAMIC;
	}
}