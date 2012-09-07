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
			COLLISION_FILTER = 16,
			DYNAMIC_BODY = 32,
			STATIC_BODY = 64,
			SENSOR = 128,
			SIGHT = 256,
			ACTION_CONTROLLER = 512,
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
			ENTITY_PRE_INIT, // All components exist but uninitialized
			ENTITY_INIT, // Initialize component
			ENTITY_CREATED, // Initialize component based on other components
			ENTITY_DESTROYED,

			LEVEL_CREATED, // Initialize component based on other entities
			LEVEL_DESTROYED,

			UPDATE,
			DRAW,
			DRAW_DEBUG,
			DRAW_LIGHTS,

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
			SET_COLLISION_GROUP,
			GET_COLLISION_GROUP,

			STATE_ENTERED,
			STATE_EXITED,

			SET_COLOR,
			SET_ALPHA,
			RESET_ANIMATION,
			ANIMATION_ENDED,

			SET_NAVIGATION_DESTINATION,
			STOP_NAVIGATION,

			SET_PERIOD,
			MERGE_TO_TEMPORAL_ECHOES,

			SET_LIT,
			IS_LIT
		};
	}
}
#endif