#include "SerializationAccess.h"
#include "Serialization.h"
#include "MovingPlatform.h"
#include "Renderer.h"
#include "Particles.h"
//#include "Lighting.h"
#include "CollisionFilter.h"
#include "Sight.h"
#include "StaticBody.h"
#include "Sensor.h"
#include "Patrol.h"
#include "Sentry.h"
#include "SecurityCamera.h"
//#include "Navigator.h"
#include "TemporalEcho.h"
#include "TemporalPeriod.h"
#include "SceneNode.h"
#include "InputController.h"
#include "Shapes.h"
#include "Button.h"
#include "Door.h"
#include "SaverLoader.h"
#include "Transform.h"
#include "Animator.h"
#include "ActionController.h"
#include "DynamicBody.h"
#include "Laser.h"
#include "SecurityCamera.h"
#include "Patrol.h"
#include "Sentry.h"

namespace Temporal
{
	void SerializationAccess::serialize(const char* key, Component*& component, XmlDeserializer& serializer)
	{
		if(strcmp(key, "transform") == 0)
			serialize(key, (Transform*&)component, serializer);
		else if(strcmp(key, "animator") == 0)
			serialize(key, (Animator*&)component, serializer);
		else if(strcmp(key, "renderer") == 0)
			serialize(key, (Renderer*&)component, serializer);
		/*else if(strcmp(key, "light") == 0)
			serialize(key, (Light*&)component, serializer);*/
		else if(strcmp(key, "particle-emitter") == 0)
			serialize(key, (ParticleEmitter*&)component, serializer);
		else if(strcmp(key, "input-controller") == 0)
			serialize(key, (InputController*&)component, serializer);
		else if(strcmp(key, "moving-platform") == 0)
			serialize(key, (MovingPlatform*&)component, serializer);
		else if(strcmp(key, "action-controller") == 0)
			serialize(key, (ActionController*&)component, serializer);
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
			serialize(key, (Sentry*&)component, serializer);
		else if(strcmp(key, "security-camera") == 0)
			serialize(key, (SecurityCamera*&)component, serializer);
		else if(strcmp(key, "patrol") == 0)
			serialize(key, (Patrol*&)component, serializer);
		/*else if(strcmp(key, "navigator") == 0)
			serialize(key, (Navigator*&)component, serializer);*/
		else if(strcmp(key, "door") == 0)
			serialize(key, (Door*&)component, serializer);
		else if(strcmp(key, "button") == 0)
			serialize(key, (Button*&)component, serializer);
		else if(strcmp(key, "laser") == 0)
			serialize(key, (Laser*&)component, serializer);
		else if(strcmp(key, "temporal-echo") == 0)
			serialize(key, (TemporalEcho*&)component, serializer);
		else if(strcmp(key, "temporal-period") == 0)
			serialize(key, (TemporalPeriod*&)component, serializer);
		else if(strcmp(key, "player-period") == 0)
			serialize(key, (PlayerPeriod*&)component, serializer);
		else if(strcmp(key, "saver-loader") == 0)
			serialize(key, (EntitySaverLoader*&)component, serializer);
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
		else if(component->getType() == Laser::TYPE)
			serialize(key, *(Laser*&)component, serializer);
		else if(component->getType() == Sentry::TYPE)
			serialize(key, *(Sentry*&)component, serializer);
		else if(component->getType() == Patrol::TYPE)
			serialize(key, *(Patrol*&)component, serializer);
		else if(component->getType() == SecurityCamera::TYPE)
			serialize(key, *(SecurityCamera*&)component, serializer);

	}
}