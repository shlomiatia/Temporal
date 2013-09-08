#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H

namespace Temporal
{
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

			SAVE,
			LOAD,

			UPDATE,
			DRAW,
			DRAW_DEBUG,
			DRAW_LIGHTS,

			GET_POSITION,
			SET_POSITION,
			GET_ORIENTATION,
			FLIP_ORIENTATION,
			GET_SHAPE,
			GET_VELOCITY,
			GET_DRAW_POSITION,
			SET_DRAW_POSITION_OVERRIDE,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			ACTION_ACTIVATE,
			
			GET_GROUND,
			SET_BODY_ENABLED,
			SET_GRAVITY_ENABLED,
			SET_TIME_BASED_IMPULSE,
			SET_ABSOLUTE_IMPULSE,
			SENSOR_START,
			SENSOR_SENSE,
			SENSOR_END,
			BODY_COLLISION,
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

			SENSOR_HANG_UP,
			SENSOR_HANG_FRONT,
			SENSOR_DESCEND,
			SENSOR_FRONG_EDGE,

			SET_LIT,
			IS_LIT,

			ACTIVATE,

			SET_TEXT
		};
	}
}
#endif