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
			DRAW_TEXT,
			DRAW_LIGHTS,

			MOUSE_DOWN,
			MOUSE_MOVE,
			MOUSE_UP,

			KEY_DOWN,
			KEY_UP,

			GET_POSITION,
			SET_POSITION,
			SET_ROTATION,
			GET_ORIENTATION,
			FLIP_ORIENTATION,
			GET_SHAPE,
			GET_VELOCITY,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			ACTION_STOP,
			ACTION_ACTIVATE,
			
			GET_GROUND,
			SET_BODY_ENABLED,
			SET_GRAVITY_ENABLED,
			SET_IMPULSE,
			SENSOR_START,
			SENSOR_SENSE,
			SENSOR_END,
			BODY_COLLISION,
			LINE_OF_SIGHT,
			SET_COLLISION_GROUP,
			GET_COLLISION_GROUP,

			STATE_ENTERED,
			STATE_EXITED,

			GET_ROOT_SCENE_NODE,
			SET_COLOR,
			SET_ALPHA,
			RESET_ANIMATION,
			TOGGLE_ANIMATION,
			SET_ANIMATION_FRAME,
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

			ACTIVATE
		};
	}
}
#endif