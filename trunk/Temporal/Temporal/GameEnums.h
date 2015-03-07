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
			DRAW_BORDERS,
			DRAW_LIGHTS,

			MOUSE_DOWN,
			MOUSE_MOVE,
			MOUSE_UP,

			KEY_DOWN,
			KEY_UP,

			GAMEPAD_BUTTON_DOWN,
			GAMEPAD_BUTTON_UP,

			GET_POSITION,
			SET_POSITION,
			TRANSLATE_POSITION,
			GET_ORIENTATION,
			FLIP_ORIENTATION,
			GET_SHAPE,
			GET_VELOCITY,

			ACTION_UP_START,
			ACTION_UP_CONTINUE,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			ACTION_ACTIVATE,
			ACTION_TAKEDOWN,
			ACTION_HOLSTER,
			ACTION_AIM,
			ACTION_FIRE,
			
			GET_GROUND,
			SET_GRAVITY_ENABLED,
			SET_IMPULSE,
			SENSOR_START,
			SENSOR_SENSE,
			SENSOR_END,
			BODY_COLLISION,
			LINE_OF_SIGHT,
			SET_COLLISION_GROUP,
			GET_COLLISION_GROUP,
			// For doors
			SET_BODY_ENABLED,

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

			// For buttons/doors
			ACTIVATE,
			DIE
		};
	}
}
#endif