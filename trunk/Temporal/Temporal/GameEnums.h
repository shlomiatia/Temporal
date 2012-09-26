#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H

namespace Temporal
{
	namespace ComponentType
	{
		enum Enum
		{
			TRANSFORM = 1,

			INPUT_CONTROLLER = 2,

			DRAW_POSITION = 4,
			ANIMATOR = 8,
			RENDERER = 16,
			LIGHT = 32,
			LIGHT_GEM = 64,
			PARTICLE_EMITTER = 128,

			COLLISION_FILTER = 256,
			DYNAMIC_BODY = 512,
			STATIC_BODY = 1024,
			SENSOR = 2048,
			SIGHT = 4096,

			ACTION_CONTROLLER = 8192,
			
			TEMPORAL_ECHO = 16384,
			TEMPORAL_PERIOD = 32768,
			PLAYER_PERIOD = 65536,

			SENTRY = 131072,
			SECURITY_CAMERA = 262144,
			PATROL = 524288,
			LASER = 1048576,
			NAVIGATOR = 2097152,

			ALL = 4194303
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
			ENTITY_POST_INIT, // Initialize component based on other components
			ENTITY_DISPOSED,

			LEVEL_INIT, // Initialize component based on other entities
			LEVEL_DISPOSED,

			UPDATE,
			DRAW,
			DRAW_DEBUG,
			DRAW_LIGHTS,

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
			
			GET_GROUND,
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