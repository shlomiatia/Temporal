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

			LEVEL_INIT, // Initialize component based on other entities - on level load
			LEVEL_DISPOSED,

			ENTITY_READY, // Initialize component based on other entities - always

			SAVE,
			PRE_LOAD,
			LOAD,
			POST_LOAD,

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
			GET_ORIENTATION,
			FLIP_ORIENTATION,
			GET_SHAPE,
			GET_FIXTURE,
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
			ACTION_TEMPORAL_TRAVEL,
			
			GET_GROUND,
			SET_GROUND,
			SET_GRAVITY_ENABLED,
			SET_BODY_ENABLED,
			IS_BODY_ENABLED,
			SET_IMPULSE,
			SENSOR_START,
			SENSOR_SENSE,
			SENSOR_END,
			BODY_COLLISION,
			LINE_OF_SIGHT,
			SET_COLLISION_GROUP,
			GET_COLLISION_GROUP,
			SET_COLLISION_CATEGORY,
			GET_COLLISION_CATEGORY,

			STATE_ENTERED,
			STATE_EXITED,

			GET_ROOT_SCENE_NODE,
			SET_COLOR,
			SET_ALPHA,
			SET_VISIBILITY,
			RESET_ANIMATION,
			TOGGLE_ANIMATION,
			SET_ANIMATION_FRAME,
			GET_ANIMATION_NORMALIZED_TIME,
			ANIMATION_ENDED,

			START_EMITTER,

			SET_NAVIGATION_DESTINATION,

			TEMPORAL_PERIOD_CHANGED,
			TEMPORAL_ECHOS_MERGED,

			SENSOR_HANG_UP,
			SENSOR_HANG_FRONT,
			SENSOR_DESCEND,
			SENSOR_FRONG_EDGE,

			SET_LIT,
			IS_VISIBLE,
			IS_COVERED,

			ACTIVATE,
			DIE
		};
	}
}
#endif