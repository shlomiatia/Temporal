#pragma once

namespace Temporal
{
	namespace ComponentType
	{
		enum Enum
		{
			INPUT_CONTROLLER,
			AI_CONTROLLER,
			POSITION,
			ORIENTATION,
			DRAW_POSITION,
			DYNAMIC_BODY,
			STATIC_BODY,
			SENSOR,
			ACTION_CONTROLLER,
			ANIMATOR,
			RENDERER,

			OTHER
		};
	}
	namespace MessageID
	{
		enum Enum
		{
			ENTITY_CREATED,

			UPDATE,
			DRAW,
			DEBUG_DRAW,

			GET_POSITION,
			SET_POSITION,
			GET_ORIENTATION,
			FLIP_ORIENTATION,
			GET_BOUNDS,
			GET_DRAW_POSITION,
			SET_DRAW_POSITION_OVERRIDE,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			
			GET_GRAVITY,
			GET_SIZE,
			GET_SENSOR_SIZE,
			SET_GRAVITY_ENABLED,
			SET_FORCE,
			SET_IMPULSE,
			RAY_CAST,
			BODY_COLLISION,
			SENSOR_COLLISION,

			ENTER_STATE,
			EXIT_STATE,

			GET_SPRITE_GROUP,
			GET_SPRITE,
			SET_SPRITE_GROUP_ID,
			SET_SPRITE_ID,
			SET_COLOR,
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
			CLIMB,
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