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
			SIGHT,
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
			ENTITY_DESTROYED,

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
			SET_GRAVITY_ENABLED,
			SET_FORCE,
			SET_IMPULSE,
			BODY_COLLISION,
			SENSOR_COLLISION,
			LINE_OF_SIGHT,

			ENTER_STATE,
			EXIT_STATE,

			GET_SPRITE_GROUP,
			GET_SPRITE,
			SET_SPRITE_GROUP_ID,
			SET_SPRITE_ID,
			SET_COLOR,
			RESET_ANIMATION,
			ANIMATION_ENDED,

			SET_NAVIGATION_DESTINATION,
			STOP_NAVIGATION,

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

	namespace ActionStateID
	{
		enum Enum
		{
			STAND,
			FALL,
			WALK,
			TURN,
			PREPARE_TO_JUMP,

			JUMP_START,
			JUMP,
			JUMP_END,

			// TODO: Gradual hang & descend PHYSICS
			PREPARE_TO_HANG,
			HANGING,
			HANG,
			DROP,
			CLIMB,
			PREPARE_TO_DESCEND,
			DESCEND
		};
	}
}