#pragma once

namespace Temporal
{
	namespace ComponentType
	{
		enum Enum
		{
			INPUT_CONTROLLER,
			POSITION,
			ORIENTATION,
			DYNAMIC_BODY,
			STATIC_BODY,
			STATE_MACHINE,
			ANIMATOR,
			RENDERER,

			OTHER
		};
	}
	namespace MessageID
	{
		enum Enum
		{
			UPDATE,
			DRAW,
			DEBUG_DRAW,

			GET_POSITION,
			SET_POSITION,
			GET_ORIENTATION,
			FLIP_ORIENTATION,
			GET_DRAW_POSITION,
			GET_BOUNDS,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			
			GET_SIZE,
			GET_SENSOR_SIZE,
			GET_GRAVITY,
			SET_GRAVITY_ENABLED,
			GET_FORCE,
			SET_FORCE,
			RAY_CAST,
			BODY_COLLISION,
			SENSOR_COLLISION,

			ENTER_STATE,
			EXIT_STATE,

			GET_SPRITE_GROUP,
			GET_SPRITE,
			SET_SPRITE_GROUP_ID,
			SET_SPRITE_ID,
			RESET_ANIMATION,
			ANIMATION_ENDED
		};
	}

	namespace AnimationID
	{
		enum Enum
		{
			STAND,
			TURN,
			DROP,
			FALL_START,
			FALL,
			JUMP_UP_START,
			JUMP_UP,
			HANG,
			CLIMBE,
			JUMP_FORWARD_START,
			JUMP_FORWARD,
			JUMP_FORWARD_END,
			WALK,
			HANG_FORWARD,
			HANG_BACKWARD,
		};
	}

	namespace SensorID
	{
		enum Enum
		{
			JUMP,
			HANG,
			BACK_EDGE,
			FRONT_EDGE
		};
	}

	namespace EntityStateID
	{
		enum Enum
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

	namespace EntityStateGravityResponse
	{
		enum Enum
		{
			DISABLE_GRAVITY,
			KEEP_STATE,
			FALL
		};
	}

	namespace VisualLayer
	{
		enum Enum
		{
			BACKGROUND = 0,
			STATIC = 1,
			PC = 2,
			COVER = 3,
			NPC = 4
		};

		static const int FARTHEST = BACKGROUND;
		static const int NEAREST = NPC;
	}
}