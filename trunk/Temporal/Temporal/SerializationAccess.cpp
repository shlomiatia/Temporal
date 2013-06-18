#include "SerializationAccess.h"
#include "Serialization.h"
#include "MovingPlatform.h"
#include "Transform.h"
#include "DrawPosition.h"
#include "Animator.h"
#include "Renderer.h"
#include "Particles.h"
#include "Lighting.h"
#include "CollisionFilter.h"
#include "Sight.h"
#include "StaticBody.h"
#include "DynamicBody.h"
#include "Sensor.h"
#include "ActionController.h"
#include "Laser.h"
#include "Patrol.h"
#include "Sentry.h"
#include "SecurityCamera.h"
#include "Navigator.h"
#include "TemporalEcho.h"
#include "TemporalPeriod.h"
#include "SceneNode.h"
#include "InputController.h"
#include "Shapes.h"
#include "Button.h"
#include "Door.h"
#include "Text.h"

namespace Temporal
{
	void SerializationAccess::serialize(const char* key, Component*& component, BaseBinarySerializer& serializer)
	{
		switch(component->getType())
		{
		case(ComponentType::TRANSFORM):
			serialize(key, *(Transform*&)component, serializer);
			break;
		case(ComponentType::DRAW_POSITION):
			serialize(key, *(DrawPosition*&)component, serializer);
			break;
		case(ComponentType::ANIMATOR):
			serialize(key, *(Animator*&)component, serializer);
			break;
		case(ComponentType::ACTION_CONTROLLER):
			serialize(key, *(ActionController*&)component, serializer);
			break;
		case(ComponentType::DYNAMIC_BODY):
			serialize(key, *(DynamicBody*&)component, serializer);
			break;
		case(ComponentType::LASER):
			serialize(key, *(Laser*&)component, serializer);
			break;
		case(ComponentType::SENTRY):
		case(ComponentType::PATROL):
		case(ComponentType::SECURITY_CAMERA):
			serialize(key, *(StateMachineComponent*&)component, serializer);
			break;
		}
	}

	void SerializationAccess::serialize(const char* key, Component*& component, XmlDeserializer& serializer)
	{
		if(strcmp(key, "transform") == 0)
			serialize(key, (Transform*&)component, serializer);
		else if(strcmp(key, "draw-position") == 0)
			serialize(key, (DrawPosition*&)component, serializer);
		else if(strcmp(key, "animator") == 0)
			serialize(key, (Animator*&)component, serializer);
		else if(strcmp(key, "renderer") == 0)
			serialize(key, (Renderer*&)component, serializer);
		else if(strcmp(key, "light") == 0)
			serialize(key, (Light*&)component, serializer);
		else if(strcmp(key, "particle-emitter") == 0)
			serialize(key, (ParticleEmitter*&)component, serializer);
		else if(strcmp(key, "text") == 0)
			serialize(key, (Text*&)component, serializer);
		else if(strcmp(key, "input-controller") == 0)
			component  = new InputController();
		else if(strcmp(key, "moving-platform") == 0)
			serialize(key, (MovingPlatform*&)component, serializer);
		else if(strcmp(key, "action-controller") == 0)
			component = new ActionController();
		else if(strcmp(key, "collision-filter") == 0)
			serialize(key, (CollisionFilter*&)component, serializer);
		else if(strcmp(key, "sight") == 0)
			serialize(key, (Sight*&)component, serializer);
		else if(strcmp(key, "sensor") == 0)
			serialize(key, (Sensor*&)component, serializer);
		else if(strcmp(key, "dynamic-body") == 0)
			serialize(key, (DynamicBody*&)component, serializer);
		else if(strcmp(key, "static-body") == 0)
			serialize(key, (StaticBody*&)component, serializer);
		else if(strcmp(key, "sentry") == 0)
			component = new Sentry();
		else if(strcmp(key, "security-camera") == 0)
			component = new SecurityCamera();
		else if(strcmp(key, "patrol") == 0)
			component = new Patrol();
		else if(strcmp(key, "navigator") == 0)
			component = new Navigator();
		else if(strcmp(key, "door") == 0)
			component = new Door();
		else if(strcmp(key, "button") == 0)
			serialize(key, (Button*&)component, serializer);
		else if(strcmp(key, "laser") == 0)
			serialize(key, (Laser*&)component, serializer);
		else if(strcmp(key, "temporal-echo") == 0)
			component = new TemporalEcho();
		else if(strcmp(key, "temporal-period") == 0)
			serialize(key, (TemporalPeriod*&)component, serializer);
		else if(strcmp(key, "player-period") == 0)
			serialize(key, (PlayerPeriod*&)component, serializer);
		else
			abort();
	}

	void SerializationAccess::serialize(const char* key, Fixture& fixture, BaseBinarySerializer& serializer)
	{
	}

	void SerializationAccess::serialize(const char* key, Fixture& fixture, XmlDeserializer& serializer)
	{
		serializer.serialize("yabp", fixture._localShape);
	}
}