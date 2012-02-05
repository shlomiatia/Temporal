#pragma once

namespace Temporal
{
	namespace ComponentType
	{
		enum Type
		{
			INPUT_CONTROLLER,
			TRANSFORM,
			DYNAMIC_BODY,
			STATIC_BODY,
			STATE_MACHINE,
			ANIMATOR,
			RENDERER

		};
	}
	namespace MessageID
	{
		enum Type
		{
			UPDATE,
			DRAW,

			GET_POSITION,
			SET_POSITION,
			GET_ORIENTATION,
			FLIP_ORIENTATION,

			ACTION_UP,
			ACTION_DOWN,
			ACTION_FORWARD,
			ACTION_BACKWARD,
			
			GET_SIZE,
			SET_GRAVITY,
			GET_FORCE,
			SET_FORCE,
			BODY_COLLISION,
			SENSOR_COLLISION,
			
			ENTER_STATE,
			EXIT_STATE,

			GET_SPRITE_GROUP,
			GET_SPRITE,
			SET_SPRITE_GROUP_ID,
			SET_SPRITE_ID,
			RESET_ANIMATION,
			ANIMATION_ENDED
		};
	}

	namespace SensorID
	{
		enum Type
		{
			JUMP,
			HANG,
			BACK_EDGE,
			FRONT_EDGE
		};
	}

	namespace EntityStateID
	{
		enum Type
		{
			STAND,
			FALL,
			WALK,
			TURN,
			PREPARE_TO_JUMP,
			JUMP_START_45,
			JUMP_START_60,
			JUMP_START_75,
			JUMP_START_90,
			JUMP_START_105,
			JUMP_UP,
			JUMP_FORWARD,
			JUMP_FORWARD_END,
			HANGING,
			HANG,
			DROP,
			CLIMBE,
			PREPARE_TO_DESCEND,
			DESCEND
		};
	}

	namespace EntityStateGravityResponse
	{
		enum Type
		{
			DISABLE_GRAVITY,
			KEEP_STATE,
			FALL
		};
	}

	namespace VisualLayer
	{
		enum Type
		{
			BACKGROUND = 0,
			STATIC = 1,
			DYNAMIC = 2
		};

		static const int FARTHEST = BACKGROUND;
		static const int NEAREST = DYNAMIC;
	}
}