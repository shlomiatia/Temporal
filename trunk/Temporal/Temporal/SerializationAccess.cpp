#include "SerializationAccess.h"
#include "Serialization.h"
#include "MovingPlatform.h"
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

namespace Temporal
{
	void SerializationAccess::getConfig(const char*& key, Component*& value, BaseSerializer& serializer, bool& shouldSerialize)
	{
		Hash type = value->getType();
		shouldSerialize = type != Editable::TYPE && type != EntitySaverLoader::TYPE;
		key = type.getString();
	}

	void SerializationAccess::getConfig(const char*& key, Entity*& value, BaseSerializer& serializer, bool& shouldSerialize)
	{
		shouldSerialize = value->getId() != GameStateEditor::TYPE && value->getId() != GameSaverLoader::TYPE;
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
		else if(strcmp(key, LightGem::TYPE.getString()) == 0)
			serialize(key, (LightGem*&)component, serializer);
		else if(strcmp(key, ParticleEmitter::TYPE.getString()) == 0)
			serialize(key, (ParticleEmitter*&)component, serializer);
		else if(strcmp(key, InputController::TYPE.getString()) == 0)
			serialize(key, (InputController*&)component, serializer);
		else if(strcmp(key, MovingPlatform::TYPE.getString()) == 0)
			serialize(key, (MovingPlatform*&)component, serializer);
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
		else if(strcmp(key, TemporalPeriod::TYPE.getString()) == 0)
			serialize(key, (TemporalPeriod*&)component, serializer);
		else if(strcmp(key, PlayerPeriod::TYPE.getString()) == 0)
			serialize(key, (PlayerPeriod*&)component, serializer);
		else if (strcmp(key, CameraControl::TYPE.getString()) == 0)
			serialize(key, (CameraControl*&)component, serializer);
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
		/*else if(component->getType() == Light::TYPE)
			serialize(key, *(Light*&)component, serializer);*/
		else if(component->getType() == ParticleEmitter::TYPE)
			serialize(key, *(ParticleEmitter*&)component, serializer);
		else if(component->getType() == InputController::TYPE)
			serialize(key, *(InputController*&)component, serializer);
		else if(component->getType() == MovingPlatform::TYPE)
			serialize(key, *(MovingPlatform*&)component, serializer);
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
		else if(component->getType() == TemporalEcho::TYPE)
			serialize(key, *(TemporalEcho*&)component, serializer);
		else if(component->getType() == TemporalPeriod::TYPE)
			serialize(key, *(TemporalPeriod*&)component, serializer);
		else if(component->getType() == PlayerPeriod::TYPE)
			serialize(key, *(PlayerPeriod*&)component, serializer);
		else if (component->getType() == CameraControl::TYPE)
			serialize(key, *(CameraControl*&)component, serializer);
	}
}