#include "SerializationAccess.h"
#include "Serialization.h"
#include "Renderer.h"
#include "Particles.h"
#include "Lighting.h"
#include "CollisionFilter.h"
#include "Sight.h"
#include "StaticBody.h"
#include "Sensor.h"
#include "Patrol.h"
#include "Navigator.h"
#include "TemporalEcho.h"
#include "TemporalPeriod.h"
#include "InputController.h"
#include "Transform.h"
#include "Animator.h"
#include "ActionController.h"
#include "DynamicBody.h"
#include "SaverLoader.h"
#include "GameStateEditor.h"
#include "Camera.h"
#include "AnimationEditor.h"
#include "Laser.h"
#include "VisibilityGem.h"
#include "DebugLayer.h"
#include "Button.h"
#include "ActivationSensor.h"
#include "Door.h"
#include "SecurityCamera.h"
#include "PlayerPeriod.h"

namespace Temporal
{
	void SerializationAccess::getConfig(const char*& key, Component*& value, BaseSerializer& serializer, bool& shouldSerialize)
	{
		shouldSerialize = !value->isBypassSave();
		Hash type = value->getType();
		key = type.getString();
	}

	void SerializationAccess::getConfig(const char*& key, Entity*& value, BaseSerializer& serializer, bool& shouldSerialize)
	{
		shouldSerialize = !value->isBypassSave();
	}

	void SerializationAccess::serialize(const char* key, Component*& component, XmlDeserializer& serializer)
	{
		if(strcmp(key, ComponentsIds::TRANSFORM.getString()) == 0)
			serialize(key, (Transform*&)component, serializer);
		else if(strcmp(key, ComponentsIds::ANIMATOR.getString()) == 0)
			serialize(key, (Animator*&)component, serializer);
		else if(strcmp(key, ComponentsIds::RENDERER.getString()) == 0)
			serialize(key, (Renderer*&)component, serializer);
		else if(strcmp(key, ComponentsIds::LIGHT.getString()) == 0)
			serialize(key, (Light*&)component, serializer);
		else if(strcmp(key, ComponentsIds::VISIBILITY_GEM.getString()) == 0)
			serialize(key, (VisibilityGem*&)component, serializer);
		else if(strcmp(key, ComponentsIds::PARTICLE_EMITTER.getString()) == 0)
			serialize(key, (ParticleEmitter*&)component, serializer);
		else if(strcmp(key, ComponentsIds::INPUT_CONTROLLER.getString()) == 0)
			serialize(key, (InputController*&)component, serializer);
		else if(strcmp(key, ComponentsIds::ACTION_CONTROLLER.getString()) == 0)
			serialize(key, (ActionController*&)component, serializer);
		else if(strcmp(key, ComponentsIds::COLLISION_FILTER.getString()) == 0)
			serialize(key, (CollisionFilter*&)component, serializer);
		else if(strcmp(key, ComponentsIds::SIGHT.getString()) == 0)
			serialize(key, (Sight*&)component, serializer);
		else if(strcmp(key, ComponentsIds::SENSOR.getString()) == 0)
			serialize(key, (Sensor*&)component, serializer);
		else if(strcmp(key, ComponentsIds::DYNAMIC_BODY.getString()) == 0)
			serialize(key, (DynamicBody*&)component, serializer);
		else if(strcmp(key, ComponentsIds::STATIC_BODY.getString()) == 0)
			serialize(key, (StaticBody*&)component, serializer);
		else if(strcmp(key, ComponentsIds::PATROL.getString()) == 0)
			serialize(key, (Patrol*&)component, serializer);
		else if(strcmp(key, ComponentsIds::NAVIGATOR.getString()) == 0)
			serialize(key, (Navigator*&)component, serializer);
		else if(strcmp(key, ComponentsIds::TEMPORAL_ECHO.getString()) == 0)
			serialize(key, (TemporalEcho*&)component, serializer);
		else if (strcmp(key, ComponentsIds::TEMPORAL_ECHO_MANAGER.getString()) == 0)
			serialize(key, (TemporalEchoManager*&)component, serializer);
		else if(strcmp(key, ComponentsIds::TEMPORAL_PERIOD.getString()) == 0)
			serialize(key, (TemporalPeriod*&)component, serializer);
		else if(strcmp(key, ComponentsIds::PLAYER_PERIOD.getString()) == 0)
			serialize(key, (PlayerPeriod*&)component, serializer);
		else if (strcmp(key, ComponentsIds::CAMERA_CONTROL.getString()) == 0)
			serialize(key, (CameraControl*&)component, serializer);
		else if (strcmp(key, ComponentsIds::LASER.getString()) == 0)
			serialize(key, (Laser*&)component, serializer);
		else if (strcmp(key, ComponentsIds::BUTTON.getString()) == 0)
			serialize(key, (Button*&)component, serializer);
		else if (strcmp(key, ComponentsIds::ACTIVATION_SENSOR.getString()) == 0)
			serialize(key, (ActivationSensor*&)component, serializer);
		else if (strcmp(key, ComponentsIds::DOOR.getString()) == 0)
			serialize(key, (Door*&)component, serializer);
		else if (strcmp(key, ComponentsIds::SECURITY_CAMERA.getString()) == 0)
			serialize(key, (SecurityCamera*&)component, serializer);
		// Singletons
		else if (strcmp(key, ComponentsIds::GAME_SAVER_LOADER.getString()) == 0)
			serialize(key, (GameSaverLoader*&)component, serializer);
		else if (strcmp(key, ComponentsIds::ANIMATION_EDITOR.getString()) == 0)
			serialize(key, (AnimationEditor*&)component, serializer);
		else if (strcmp(key, ComponentsIds::GAME_STATE_EDITOR.getString()) == 0)
			serialize(key, (GameStateEditor*&)component, serializer);
		else if (strcmp(key, ComponentsIds::DEBUG_MANAGER.getString()) == 0)
			serialize(key, (DebugManager*&)component, serializer);
		else
			abort();
	}

	void SerializationAccess::serialize(const char* key, Component*& component, BaseSerializer& serializer)
	{
		if(component->getType() == ComponentsIds::TRANSFORM)
			serialize(key, *(Transform*&)component, serializer);
		else if(component->getType() == ComponentsIds::ANIMATOR)
			serialize(key, *(Animator*&)component, serializer);
		else if(component->getType() == ComponentsIds::ACTION_CONTROLLER)
			serialize(key, *(ActionController*&)component, serializer);
		else if(component->getType() == ComponentsIds::DYNAMIC_BODY)
			serialize(key, *(DynamicBody*&)component, serializer);
		else if(component->getType() == ComponentsIds::PATROL)
			serialize(key, *(Patrol*&)component, serializer);
		else if(component->getType() == ComponentsIds::RENDERER)
			serialize(key, *(Renderer*&)component, serializer);
		else if(component->getType() == ComponentsIds::LIGHT)
			serialize(key, *(Light*&)component, serializer);
		else if(component->getType() == ComponentsIds::PARTICLE_EMITTER)
			serialize(key, *(ParticleEmitter*&)component, serializer);
		else if(component->getType() == ComponentsIds::INPUT_CONTROLLER)
			serialize(key, *(InputController*&)component, serializer);
		else if(component->getType() == ComponentsIds::COLLISION_FILTER)
			serialize(key, *(CollisionFilter*&)component, serializer);
		else if(component->getType() == ComponentsIds::SIGHT)
			serialize(key, *(Sight*&)component, serializer);
		else if(component->getType() == ComponentsIds::SENSOR)
			serialize(key, *(Sensor*&)component, serializer);
		else if(component->getType() == ComponentsIds::STATIC_BODY)
			serialize(key, *(StaticBody*&)component, serializer);
		else if(component->getType() == ComponentsIds::NAVIGATOR)
			serialize(key, *(Navigator*&)component, serializer);
		else if (component->getType() == ComponentsIds::TEMPORAL_ECHO_MANAGER)
			serialize(key, *(TemporalEchoManager*&)component, serializer);
		else if(component->getType() == ComponentsIds::TEMPORAL_ECHO)
			serialize(key, *(TemporalEcho*&)component, serializer);
		else if(component->getType() == ComponentsIds::TEMPORAL_PERIOD)
			serialize(key, *(TemporalPeriod*&)component, serializer);
		else if(component->getType() == ComponentsIds::PLAYER_PERIOD)
			serialize(key, *(PlayerPeriod*&)component, serializer);
		else if (component->getType() == ComponentsIds::CAMERA_CONTROL)
			serialize(key, *(CameraControl*&)component, serializer);
		else if (component->getType() == ComponentsIds::LASER)
			serialize(key, *(Laser*&)component, serializer);
		else if (component->getType() == ComponentsIds::BUTTON)
			serialize(key, *(Button*&)component, serializer);
		else if (component->getType() == ComponentsIds::ACTIVATION_SENSOR)
			serialize(key, *(ActivationSensor*&)component, serializer);
		else if (component->getType() == ComponentsIds::DOOR)
			serialize(key, *(Door*&)component, serializer);
		else if (component->getType() == ComponentsIds::SECURITY_CAMERA)
			serialize(key, *(SecurityCamera*&)component, serializer);
		// Singletons
		else if (component->getType() == ComponentsIds::GAME_SAVER_LOADER)
			serialize(key, *(GameSaverLoader*&)component, serializer);
		else if (component->getType() == ComponentsIds::ANIMATION_EDITOR)
			serialize(key, *(AnimationEditor*&)component, serializer);
		else if (component->getType() == ComponentsIds::GAME_STATE_EDITOR)
			serialize(key, *(GameStateEditor*&)component, serializer);
		else if (component->getType() == ComponentsIds::DEBUG_MANAGER)
			serialize(key, *(DebugManager*&)component, serializer);
	}
}