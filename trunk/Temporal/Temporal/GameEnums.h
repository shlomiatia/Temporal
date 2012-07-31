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
			TEMPORAL_ECHO = 8192,
			TEMPORAL_PERIOD = 16384,

			OTHER = 1048576,

			ALL = 2097151
		};
	}
	inline ComponentType::Enum operator|(ComponentType::Enum a, ComponentType::Enum b) { return static_cast<ComponentType::Enum>(static_cast<int>(a) | static_cast<int>(b)); }
	inline ComponentType::Enum operator&(ComponentType::Enum a, ComponentType::Enum b) { return static_cast<ComponentType::Enum>(static_cast<int>(a) & static_cast<int>(b)); }

	namespace MessageID
	{
		enum Enum
		{
			ENTITY_CREATED,
			ENTITY_DESTROYED,

			LEVEL_CREATED,
			LEVEL_DESTROYED,

			UPDATE,
			DRAW,
			DEBUG_DRAW,

			SERIALIZE,
			DESERIALIZE,

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

			SET_COLOR,
			RESET_ANIMATION,
			ANIMATION_ENDED,
			SET_TARGET,

			SET_NAVIGATION_DESTINATION,
			STOP_NAVIGATION,

			GET_PERIOD,
			SET_PERIOD,
			MERGE_TO_TEMPORAL_ECHOES,
		};
	}

	namespace VisualLayer
	{
		enum Enum
		{
			LIGHT,
			BACKGROUND,
			STATIC,
			PC,
			COVER,
			NPC
		};

		static const int FARTHEST = LIGHT;
		static const int NEAREST = NPC;
	}
}
#endif