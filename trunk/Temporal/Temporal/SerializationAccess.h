#ifndef SERIALIZATIONACCESS_H
#define SERIALIZATIONACCESS_H

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

namespace Temporal
{
	class Vector;
	class AABB;
	class Segment;
	class Sprite;
	class SpriteGroup;
	class SpriteSheet;
	class Sample;
	class SampleSet;
	class Animation;
	class AnimationSet;
	class Entity;
	class Component;

	class SerializationAccess
	{
	public:
		template<class T>
		static void serialize(const char* key, T*& value, XmlDeserializer& serializer)
		{
			value = new T();
			serialize(key, *value, serializer);
		}

		template<class T>
		static void serialize(const char* key, AABB& aabb, T& serializer)
		{
			serializer.serialize("center", aabb._center);
			serializer.serialize("radius", aabb._radius);
		}

		template<class T>
		static void serialize(const char* key, Segment& segment, T& serializer)
		{
			serializer.serialize("center", segment._center);
			serializer.serialize("radius", segment._radius);
		}

		template<class T>
		static void serialize(const char* key, Vector& vector, T& serializer)
		{
			serializer.serialize("x", vector._x);
			serializer.serialize("y", vector._y);
		}

		template<class T>
		static void serialize(const char* key, Color& color, T& serializer)
		{
			serializer.serialize("r", color._r);	
			serializer.serialize("g", color._g);
			serializer.serialize("b", color._b);
			serializer.serialize("a", color._a);
		}

		template<class T>
		static void serialize(const char* key, Sprite& sprite, T& serializer)
		{
			serializer.serialize("bounds", sprite._bounds);
			serializer.serialize("offset", sprite._offset);
		}
	
		template<class T>
		static void serialize(const char* key, SpriteGroup& spritegroup, T& serializer)
		{
			serializer.serialize("id", spritegroup._id);
			serializer.serialize("sprite", spritegroup._sprites);
		}
	
		template<class T>
		static void serialize(const char* key, SpriteSheet& spritesheet, T& serializer)
		{
			serializer.serialize("texture", spritesheet._id);
			serializer.serialize("orientation", (int&)spritesheet._orientation);
			serializer.serialize("sprite-group", spritesheet._spriteGroups);
			spritesheet.init();
		}

		template<class T>
		static void serialize(const char* key, Sample& sample, T& serializer)
		{
			serializer.serialize("sprite-group", sample._spriteGroupId);
			serializer.serialize("translation", sample._translation);
			serializer.serialize("rotation", sample._rotation);
			serializer.serialize("duration", sample._duration);
		}

		template<class T>
		static void serialize(const char* key, SampleSet& sampleSet, T& serializer)
		{
			serializer.serialize("scene-node-id", sampleSet._sceneNodeId);
			serializer.serialize("sample", sampleSet._samples);
			sampleSet.init();
		}
		template<class T>
		static void serialize(const char* key, Animation& animation, T& serializer)
		{
			serializer.serialize("id", animation._id);
			serializer.serialize("repeat", animation._repeat);
			serializer.serialize("rewind", animation._rewind);
			serializer.serialize("sample-set", animation._sampleSets);
			animation.init();
		}

		template<class T>
		static void serialize(const char* key, AnimationSet& animationSet, T& serializer)
		{
			serializer.serialize("id", animationSet._id);
			serializer.serialize("animation", animationSet._animations);
		}

		template<class T>
		static void serialize(const char* key, Entity& entity, T& serializer)
		{
			serializer.serialize("id", entity._id);
			serializer.serialize("component", entity._components);
		}

		template<class T>
		static void serialize(const char* key, Transform& transform, T& serializer)
		{
			serializer.serialize("position", transform._position);
			serializer.serialize("orientation", (int&)transform._orientation);
		}

		template<class T>
		static void serialize(const char* key, DrawPosition& drawPosition, T& serializer)
		{
			serializer.serialize("offset", drawPosition._offset);
			serializer.serialize("override", drawPosition._override);
		}

		template<class T>
		static void serialize(const char* key, Animator& animator, T& serializer)
		{
			serializer.serialize("animation-set", animator._animationSetId);
			serializer.serialize("animation", animator._animationId); 
			serializer.serialize("timer", animator._timer);
		}

		template<class T>
		static void serialize(const char* key, SceneNode& sceneNode, T& serializer)
		{
			serializer.serialize("id", sceneNode._id);
			serializer.serialize("behind-parent", sceneNode._drawBehindParent);
			serializer.serialize("transform", sceneNode._transformOnly);
			serializer.serialize("sprite-sheet", sceneNode._spriteSheetId);
			serializer.serialize("sprite-group", sceneNode._spriteGroupId);
			serializer.serialize("scene-node", sceneNode._children);
		}
		template<class T>
		static void serialize(const char* key, Renderer& renderer, T& serializer)
		{
			serializer.serialize("layer", (int&)renderer._layer);
			serializer.serialize("color", renderer._color);
			serializer.serialize("scene-node", renderer._root);
			renderer._root->init();
		}

		template<class T>
		static void serialize(const char* key, Light& light, T& serializer)
		{
			serializer.serialize("color", light._color);
			serializer.serialize("radius", light._radius);
			serializer.serialize("center", light._beamCenter);
			serializer.serialize("size", light._beamSize);
		}

		template<class T>
		static void serialize(const char* key, ParticleEmitter& particleEmitter, T& serializer)
		{
			serializer.serialize("lifetime", particleEmitter._lifetime);
			serializer.serialize("birth-threshold", particleEmitter._birthThreshold);
			serializer.serialize("sprite-sheet", particleEmitter._spritesheetId);
			serializer.serialize("birth-radius", particleEmitter._birthRadius);
			serializer.serialize("velocity", particleEmitter._velocity);
			serializer.serializeRadians("center", particleEmitter._directionCenter);
			serializer.serializeRadians("size", particleEmitter._directionSize);
		}

		template<class T>
		static void serialize(const char* key, CollisionFilter& collisionnFilter, T& serializer)
		{
			serializer.serialize("category", collisionnFilter._category);
			serializer.serialize("group", collisionnFilter._group);
		}

		template<class T>
		static void serialize(const char* key, Sight& sight, T& serializer)
		{
			serializer.serializeRadians("center", sight._sightCenter);
			serializer.serializeRadians("size", sight._sightSize);
		}

		template<class T>
		static void serialize(const char* key, Sensor& sensor, T& serializer)
		{
			serializer.serialize("category-mask", sensor._categoryMask);
			serializer.serialize("fixture", sensor._fixture);
			serializer.serialize("contact-listener", sensor._listener);
		}

		template<class T>
		static void serialize(const char* key, DynamicBody& dynamicBody, T& serializer)
		{
			serializer.serialize("fixture", dynamicBody._fixture);
		}

		template<class T>
		static void serialize(const char* key, StaticBody& staticBody, T& serializer)
		{
			serializer.serialize("fixture", staticBody._fixture);
		}

		template<class T>
		static void serialize(const char* key, LedgeDetector& ledgeDetector, T& serializer)
		{
			serializer.serialize("id", ledgeDetector._id);
			serializer.serializeRadians("center", ledgeDetector._rangeCenter);
			serializer.serializeRadians("size", ledgeDetector._rangeSize);
		}

		template<class T>
		static void serialize(const char* key, Laser& laser, T& serializer)
		{
			serializer.serialize("platform", laser._platformID);
		}

		template<class T>
		static void serialize(const char* key, TemporalPeriod& temporalPeriod, T& serializer)
		{
			serializer.serialize("period", (int&)temporalPeriod._period);
		}

		template<class T>
		static void serialize(const char* key, PlayerPeriod& playerPeriod, T& serializer)
		{
			serializer.serialize("period", (int&)playerPeriod._period);
		}

		static void serialize(const char* key, Component*& component, MemorySerialization& serializer)
		{
			if(component->getType() == ComponentType::TRANSFORM)
				serialize(key, *(Transform*&)component, serializer);
			else if(component->getType() == ComponentType::DRAW_POSITION)
				serialize(key, *(DrawPosition*&)component, serializer);
			else if(component->getType() == ComponentType::ANIMATOR)
				serialize(key, *(Animator*&)component, serializer);
		}

		static void serialize(const char* key, Fixture*& fixture, XmlDeserializer& serializer)
		{
			Shape* shape = NULL;
			if(strcmp(key, "aabb") == 0)
				serialize(key, (AABB*&)shape, serializer);
			else if(strcmp(key, "segment") == 0)
				serialize(key, (Segment*&)shape, serializer);
			else
				abort();
			fixture = new Fixture(shape);
		}

		static void serialize(const char* key, ContactListener*& contactListener, XmlDeserializer& serializer)
		{
			if(strcmp(key, "ledge-detector") == 0)
				serialize(key, (LedgeDetector*&)contactListener, serializer);
			else
				abort();
		}

		static void serialize(const char* key, Component*& component, XmlDeserializer& serializer)
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
			else if(strcmp(key, "input-controller") == 0)
				component  = new InputController();
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
	};
}

#endif