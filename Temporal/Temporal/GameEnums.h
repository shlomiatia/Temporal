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
			TRANSFORM = 4,
			DRAW_POSITION = 8,
			DYNAMIC_BODY = 16,
			STATIC_BODY = 32,
			SENSOR = 64,
			SIGHT = 128,
			ACTION_CONTROLLER = 256,
			ANIMATOR = 1024,
			RENDERER = 2048,
			TEMPORAL_ECHO = 4096,
			TEMPORAL_PERIOD = 8192,

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
			GET_SHAPE,
			GET_DRAW_POSITION,
			SET_DRAW_POSITION_OVERRIDE,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			
			GET_GROUND_VECTOR,
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

			SET_LIT,
			IS_LIT
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

		static const int FARTHEST = BACKGROUND;
		static const int NEAREST = NPC;
	}
}
#endif