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
#include "SceneNode.h"
#include "InputController.h"
#include "Shapes.h"
#include "Transform.h"
#include "Animator.h"
#include "ActionController.h"
#include "DynamicBody.h"
#include "Patrol.h"
#include "Editable.h"
#include "SaverLoader.h"
#include "GameStateEditor.h"
#include "Camera.h"
#include "AnimationEditor.h"
#include "Laser.h"
#include "VisibilityGem.h"
#include "DebugLayer.h"
#include "Button.h"
#include "Door.h"
#include "SecurityCamera.h"

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
		if(strcmp(key, Transform::TYPE.getString()) == 0)
			serialize(key, (Transform*&)component, serializer);
		else if(strcmp(key, Animator::TYPE.getString()) == 0)
			serialize(key, (Animator*&)component, serializer);
		else if(strcmp(key, Renderer::TYPE.getString()) == 0)
			serialize(key, (Renderer*&)component, serializer);
		else if(strcmp(key, Light::TYPE.getString()) == 0)
			serialize(key, (Light*&)component, serializer);
		else if(strcmp(key, VisibilityGem::TYPE.getString()) == 0)
			serialize(key, (VisibilityGem*&)component, serializer);
		else if(strcmp(key, ParticleEmitter::TYPE.getString()) == 0)
			serialize(key, (ParticleEmitter*&)component, serializer);
		else if(strcmp(key, InputController::TYPE.getString()) == 0)
			serialize(key, (InputController*&)component, serializer);
		else if(strcmp(key, ActionController::TYPE.getString()) == 0)
			serialize(key, (ActionController*&)component, serializer);
		else if(strcmp(key, CollisionFilter::TYPE.getString()) == 0)
			serialize(key, (CollisionFilter*&)component, serializer);
		else if(strcmp(key, Sight::TYPE.getString()) == 0)
			serialize(key, (Sight*&)component, serializer);
		else if(strcmp(key, Sensor::TYPE.getString()) == 0)
			serialize(key, (Sensor*&)component, serializer);
		else if(strcmp(key, DynamicBody::TYPE.getString()) == 0)
			serialize(key, (DynamicBody*&)component, serializer);
		else if(strcmp(key, StaticBody::TYPE.getString()) == 0)
			serialize(key, (StaticBody*&)component, serializer);
		else if(strcmp(key, Patrol::TYPE.getString()) == 0)
			serialize(key, (Patrol*&)component, serializer);
		else if(strcmp(key, Navigator::TYPE.getString()) == 0)
			serialize(key, (Navigator*&)component, serializer);
		else if(strcmp(key, TemporalEcho::TYPE.getString()) == 0)
			serialize(key, (TemporalEcho*&)component, serializer);
		else if (strcmp(key, TemporalEchoManager::TYPE.getString()) == 0)
			serialize(key, (TemporalEchoManager*&)component, serializer);
		else if(strcmp(key, TemporalPeriod::TYPE.getString()) == 0)
			serialize(key, (TemporalPeriod*&)component, serializer);
		else if(strcmp(key, PlayerPeriod::TYPE.getString()) == 0)
			serialize(key, (PlayerPeriod*&)component, serializer);
		else if (strcmp(key, CameraControl::TYPE.getString()) == 0)
			serialize(key, (CameraControl*&)component, serializer);
		else if (strcmp(key, Laser::TYPE.getString()) == 0)
			serialize(key, (Laser*&)component, serializer);
		else if (strcmp(key, Button::TYPE.getString()) == 0)
			serialize(key, (Button*&)component, serializer);
		else if (strcmp(key, Door::TYPE.getString()) == 0)
			serialize(key, (Door*&)component, serializer);
		else if (strcmp(key, SecurityCamera::TYPE.getString()) == 0)
			serialize(key, (SecurityCamera*&)component, serializer);
		// Singletons
		else if (strcmp(key, GameSaverLoader::TYPE.getString()) == 0)
			serialize(key, (GameSaverLoader*&)component, serializer);
		else if (strcmp(key, AnimationEditor::TYPE.getString()) == 0)
			serialize(key, (AnimationEditor*&)component, serializer);
		else if (strcmp(key, GameStateEditor::TYPE.getString()) == 0)
			serialize(key, (GameStateEditor*&)component, serializer);
		else if (strcmp(key, DebugManager::TYPE.getString()) == 0)
			serialize(key, (DebugManager*&)component, serializer);
		else
			abort();
	}

	void SerializationAccess::serialize(const char* key, Component*& component, BaseSerializer& serializer)
	{
		if(component->getType() == Transform::TYPE)
			serialize(key, *(Transform*&)component, serializer);
		else if(component->getType() == Animator::TYPE)
			serialize(key, *(Animator*&)component, serializer);
		else if(component->getType() == ActionController::TYPE)
			serialize(key, *(ActionController*&)component, serializer);
		else if(component->getType() == DynamicBody::TYPE)
			serialize(key, *(DynamicBody*&)component, serializer);
		else if(component->getType() == Patrol::TYPE)
			serialize(key, *(Patrol*&)component, serializer);
		// XML ONLY
		else if(component->getType() == Renderer::TYPE)
			serialize(key, *(Renderer*&)component, serializer);
		else if(component->getType() == Light::TYPE)
			serialize(key, *(Light*&)component, serializer);
		else if(component->getType() == ParticleEmitter::TYPE)
			serialize(key, *(ParticleEmitter*&)component, serializer);
		else if(component->getType() == InputController::TYPE)
			serialize(key, *(InputController*&)component, serializer);
		else if(component->getType() == CollisionFilter::TYPE)
			serialize(key, *(CollisionFilter*&)component, serializer);
		else if(component->getType() == Sight::TYPE)
			serialize(key, *(Sight*&)component, serializer);
		else if(component->getType() == Sensor::TYPE)
			serialize(key, *(Sensor*&)component, serializer);
		else if(component->getType() == StaticBody::TYPE)
			serialize(key, *(StaticBody*&)component, serializer);
		else if(component->getType() == Navigator::TYPE)
			serialize(key, *(Navigator*&)component, serializer);
		else if (component->getType() == TemporalEchoManager::TYPE)
			serialize(key, *(TemporalEchoManager*&)component, serializer);
		else if(component->getType() == TemporalEcho::TYPE)
			serialize(key, *(TemporalEcho*&)component, serializer);
		else if(component->getType() == TemporalPeriod::TYPE)
			serialize(key, *(TemporalPeriod*&)component, serializer);
		else if(component->getType() == PlayerPeriod::TYPE)
			serialize(key, *(PlayerPeriod*&)component, serializer);
		else if (component->getType() == CameraControl::TYPE)
			serialize(key, *(CameraControl*&)component, serializer);
		else if (component->getType() == Laser::TYPE)
			serialize(key, *(Laser*&)component, serializer);
		else if (component->getType() == Button::TYPE)
			serialize(key, *(Button*&)component, serializer);
		else if (component->getType() == Door::TYPE)
			serialize(key, *(Door*&)component, serializer);
		else if (component->getType() == SecurityCamera::TYPE)
			serialize(key, *(SecurityCamera*&)component, serializer);
		// Singletons
		else if (component->getType() == GameSaverLoader::TYPE)
			serialize(key, *(GameSaverLoader*&)component, serializer);
		else if (component->getType() == AnimationEditor::TYPE)
			serialize(key, *(AnimationEditor*&)component, serializer);
		else if (component->getType() == GameStateEditor::TYPE)
			serialize(key, *(GameStateEditor*&)component, serializer);
		else if (component->getType() == DebugManager::TYPE)
			serialize(key, *(DebugManager*&)component, serializer);
	}
}