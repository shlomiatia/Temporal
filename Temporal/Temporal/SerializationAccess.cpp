#include "SerializationAccess.h"
#include "BinarySerializer.h"
#include "BinaryDeserializer.h"
#include "XmlSerializer.h"
#include "XmlDeserializer.h"
#include "ComponentEditorSerializer.h"

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

#define I_HATE_CPP \
if (compare(key, component, ComponentsIds::TRANSFORM)) \
	serialize(key, (Transform*&)component, serializer); \
else if (compare(key, component, ComponentsIds::ANIMATOR)) \
	serialize(key, (Animator*&)component, serializer); \
else if (compare(key, component, ComponentsIds::RENDERER)) \
	serialize(key, (Renderer*&)component, serializer); \
else if (compare(key, component, ComponentsIds::LIGHT)) \
	serialize(key, (Light*&)component, serializer); \
else if (compare(key, component, ComponentsIds::PARTICLE_EMITTER)) \
	serialize(key, (ParticleEmitter*&)component, serializer); \
else if (compare(key, component, ComponentsIds::INPUT_CONTROLLER)) \
	serialize(key, (InputController*&)component, serializer); \
else if (compare(key, component, ComponentsIds::ACTION_CONTROLLER)) \
	serialize(key, (ActionController*&)component, serializer); \
else if (compare(key, component, ComponentsIds::COLLISION_FILTER)) \
	serialize(key, (CollisionFilter*&)component, serializer); \
else if (compare(key, component, ComponentsIds::SIGHT)) \
	serialize(key, (Sight*&)component, serializer); \
else if (compare(key, component, ComponentsIds::SENSOR)) \
	serialize(key, (Sensor*&)component, serializer); \
else if (compare(key, component, ComponentsIds::DYNAMIC_BODY)) \
	serialize(key, (DynamicBody*&)component, serializer); \
else if (compare(key, component, ComponentsIds::STATIC_BODY)) \
	serialize(key, (StaticBody*&)component, serializer); \
else if (compare(key, component, ComponentsIds::PATROL)) \
	serialize(key, (Patrol*&)component, serializer); \
else if (compare(key, component, ComponentsIds::NAVIGATOR)) \
	serialize(key, (Navigator*&)component, serializer); \
else if (compare(key, component, ComponentsIds::TEMPORAL_ECHO)) \
	serialize(key, (TemporalEcho*&)component, serializer); \
else if (compare(key, component, ComponentsIds::TEMPORAL_ECHO_MANAGER)) \
	serialize(key, (TemporalEchoManager*&)component, serializer); \
else if (compare(key, component, ComponentsIds::TEMPORAL_PERIOD)) \
	serialize(key, (TemporalPeriod*&)component, serializer); \
else if (compare(key, component, ComponentsIds::PLAYER_PERIOD)) \
	serialize(key, (PlayerPeriod*&)component, serializer); \
else if (compare(key, component, ComponentsIds::LASER)) \
	serialize(key, (Laser*&)component, serializer); \
else if (compare(key, component, ComponentsIds::BUTTON)) \
	serialize(key, (Button*&)component, serializer); \
else if (compare(key, component, ComponentsIds::ACTIVATION_SENSOR)) \
	serialize(key, (ActivationSensor*&)component, serializer); \
else if (compare(key, component, ComponentsIds::DOOR)) \
	serialize(key, (Door*&)component, serializer); \
else if (compare(key, component, ComponentsIds::SECURITY_CAMERA)) \
	serialize(key, (SecurityCamera*&)component, serializer); \
else if (compare(key, component, ComponentsIds::VISIBILITY_GEM)) \
	serialize(key, (VisibilityGem*&)component, serializer); \
else if (compare(key, component, ComponentsIds::GAME_SAVER_LOADER)) \
	serialize(key, (GameSaverLoader*&)component, serializer); \
else if (compare(key, component, ComponentsIds::ANIMATION_EDITOR)) \
	serialize(key, (AnimationEditor*&)component, serializer); \
else if (compare(key, component, ComponentsIds::GAME_STATE_EDITOR)) \
	serialize(key, (GameStateEditor*&)component, serializer); \
else if (compare(key, component, ComponentsIds::DEBUG_MANAGER)) \
	serialize(key, (DebugManager*&)component, serializer); \
else \
	abort(); \

namespace Temporal
{
	bool compare(const char* key, Component*& component, Hash componentId)
	{
		return component ?
			component->getType() == componentId :
			strcmp(key, componentId.getString()) == 0;
	}

	bool SerializationAccess::shouldSerialize(Component*& value)
	{
		bool shouldSerialize = !value->isBypassSave();
		return shouldSerialize;
	}

	bool SerializationAccess::shouldSerialize(Entity*& value)
	{
		bool shouldSerialize = !value->isBypassSave();
		return shouldSerialize;
	}

	void SerializationAccess::modifyKey(const char*& key, Component*& value)
	{
		Hash type = value->getType();
		key = type.getString();
	}

	void SerializationAccess::serialize(const char* key, Component*& component, XmlSerializer& serializer)
	{
		I_HATE_CPP
	}

	void SerializationAccess::serialize(const char* key, Component*& component, XmlDeserializer& serializer)
	{
		I_HATE_CPP
	}

	void SerializationAccess::serialize(const char* key, Component*& component, BinarySerializer& serializer)
	{
		I_HATE_CPP
	}

	void SerializationAccess::serialize(const char* key, Component*& component, BinaryDeserializer& serializer)
	{
		I_HATE_CPP
	}

	void SerializationAccess::serialize(const char* key, Component*& component, ComponentEditorSerializer& serializer)
	{
		I_HATE_CPP
	}
}