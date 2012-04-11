#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H

namespace Temporal
{
	namespace ComponentType
	{
		enum Enum
		{
			INPUT_CONTROLLER = 1,
			AI_CONTROLLER = 2,
			POSITION = 4,
			ORIENTATION = 8,
			DRAW_POSITION = 16,
			DYNAMIC_BODY = 32,
			STATIC_BODY = 64,
			SENSOR = 128,
			SIGHT = 256,
			ACTION_CONTROLLER = 1024,
			ANIMATOR = 2048,
			RENDERER = 4096,

			OTHER = 8192,

			ALL = 16383
		};
	}
	inline ComponentType::Enum operator|(ComponentType::Enum a, ComponentType::Enum b) { return (ComponentType::Enum)((int)a | (int)b); }
	inline ComponentType::Enum operator&(ComponentType::Enum a, ComponentType::Enum b) { return (ComponentType::Enum)((int)a & (int)b); }

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
			GET_SHAPE,
			IS_COVER,
			GET_DRAW_POSITION,
			SET_DRAW_POSITION_OVERRIDE,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			
			GET_GRAVITY,
			GET_GROUND_VECTOR,
			GET_SIZE,
			SET_GRAVITY_ENABLED,
			SET_TIME_BASED_IMPULSE,
			SET_ABSOLUTE_IMPULSE,
			BODY_COLLISION,
			SENSOR_COLLISION,
			LINE_OF_SIGHT,

			STATE_ENTERED,
			STATE_EXITED,

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
			BACKGROUND,
			STATIC,
			PC,
			COVER,
			NPC
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
#endif