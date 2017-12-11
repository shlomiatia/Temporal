#ifndef IDS_H
#define IDS_H

#include "Hash.h"

namespace Temporal
{
	namespace SensorIds
	{
		static const Hash LEDGE_SENSOR_ID = Hash("SNS_LEDGE");
		static const Hash ACTIVATE_SENSOR_ID = Hash("SNS_ACTIVATE");
		static const Hash TAKEDOWN_SENSOR_ID = Hash("SNS_TAKEDOWN");
	}

	namespace AnimationIds
	{
		static const Hash STAND_ANIMATION = Hash("POP_ANM_STAND");
		static const Hash TURN_ANIMATION = Hash("POP_ANM_TURN");
		static const Hash FALL_ANIMATION = Hash("POP_ANM_FALL");
		static const Hash HANG_ANIMATION = Hash("POP_ANM_HANG");
		static const Hash DESCEND_ANIMATION = Hash("POP_ANM_DESCEND");
		static const Hash WALK_ANIMATION = Hash("POP_ANM_WALK");
		static const Hash TAKEDOWN_ANIMATION = Hash("POP_ANM_TAKEDOWN");
		static const Hash DIE_ANIMATION = Hash("POP_ANM_DIE");
		static const Hash DEAD_ANIMATION = Hash("POP_ANM_DEAD");
		static const Hash AIM_DOWN_ANIMATION = Hash("POP_ANM_AIM_DOWN");
		static const Hash AIM_UP_ANIMATION = Hash("POP_ANM_AIM_UP");
		static const Hash FIRE_DOWN_ANIMATION = Hash("POP_ANM_FIRE_DOWN");
		static const Hash FIRE_UP_ANIMATION = Hash("POP_ANM_FIRE_UP");
		static const Hash INVESTIGATE_ANIMATION = Hash("POP_ANM_INVESTIGATE");
		static const Hash DRAG_ANIMATION = Hash("POP_ANM_DRAG");
	}

	namespace ActionControllerStateIds
	{
		static const Hash STAND_STATE = Hash("ACT_STT_STAND");
		static const Hash FALL_STATE = Hash("ACT_STT_FALL");
		static const Hash WALK_STATE = Hash("ACT_STT_WALK");
		static const Hash TURN_STATE = Hash("ACT_STT_TURN");
		static const Hash JUMP_STATE = Hash("ACT_STT_JUMP");
		static const Hash JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
		static const Hash HANG_STATE = Hash("ACT_STT_HANG");
		static const Hash CLIMB_STATE = Hash("ACT_STT_CLIMB");
		static const Hash DESCEND_STATE = Hash("ACT_STT_DESCEND");
		static const Hash TAKEDOWN_STATE = Hash("ACT_STT_TAKEDOWN");
		static const Hash DIE_STATE = Hash("ACT_STT_DIE");
		static const Hash DEAD_STATE = Hash("ACT_STT_DEAD");
		static const Hash AIM_STATE = Hash("ACT_STT_AIM");
		static const Hash FIRE_STATE = Hash("ACT_STT_FIRE");
		static const Hash DRAG_STAND_STATE = Hash("ACT_STT_DRAG_STAND");
		static const Hash DRAG_FORWARD_STATE = Hash("ACT_STT_DRAG_FORWARD");
		static const Hash DRAG_BACKWARDS_STATE = Hash("ACT_STT_DRAG_BACKWARDS");
		static const Hash INVESTIGATE_STATE = Hash("ACT_STT_INVESTIGATE");
	}

	namespace ComponentsIds
	{
		static const Hash VISIBILITY_GEM = Hash("visibility-gem");
		static const Hash SENSOR = Hash("sensor");
		static const Hash TRANSFORM = Hash("transform");
		static const Hash TEMPORAL_PERIOD = Hash("temporal-period");
		static const Hash PLAYER_PERIOD = Hash("player-period");
		static const Hash TEMPORAL_ECHO = Hash("temporal-echo");
		static const Hash TEMPORAL_ECHO_MANAGER = Hash("temporal-echo-manager");
		static const Hash STATIC_BODY = Hash("static-body");
		static const Hash COLLISION_FILTER = Hash("collision-filter");
		static const Hash SIGHT = Hash("sight");
		static const Hash RENDERER = Hash("renderer");
		static const Hash PARTICLE_EMITTER = Hash("particle-emitter");
		static const Hash LIGHT = Hash("light");
		static const Hash PATROL = Hash("patrol");
		static const Hash NAVIGATOR = Hash("navigator");
		static const Hash CONTROL = Hash("control");
		static const Hash CAMERA_CONTROL("camera-control");
		static const Hash INPUT_CONTROLLER = Hash("input-controller");
		static const Hash ANIMATOR = Hash("animator");
		static const Hash ACTION_CONTROLLER = Hash("action-controller");
		static const Hash DYNAMIC_BODY = Hash("dynamic-body");
		static const Hash ENTITY_SAVER_LOADER = Hash("entity-saver-loader");
		static const Hash GAME_SAVER_LOADER = Hash("game-saver-loader");
		static const Hash GAME_STATE_EDITOR = Hash("game-state-editor");
		static const Hash GAME_STATE_EDITOR_PREVIEW = Hash("game-state-editor-preview");
		static const Hash ANIMATION_EDITOR = Hash("animation-editor");
		static const Hash LASER = Hash("laser");
		static const Hash DEBUG_MANAGER = Hash("debug-manager");
		static const Hash BUTTON = Hash("button");
		static const Hash ACTIVATION_SENSOR = Hash("activation-sensor");
		static const Hash DOOR = Hash("door");
		static const Hash SECURITY_CAMERA = Hash("security-camera");
		static const Hash EDITABLE = Hash("editable");
	}
}

#endif
	