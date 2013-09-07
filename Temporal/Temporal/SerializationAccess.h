#ifndef SERIALIZATIONACCESS_H
#define SERIALIZATIONACCESS_H

namespace Temporal
{
	class Settings;
	class XmlDeserializer;
	class BaseSerializer;
	class Component;
	class Vector;
	class AABB;
	class Segment;
	class YABP;
	class Fixture;
	class Color;
	class Sprite;
	class SpriteGroup;
	class SpriteSheet;
	class Sample;
	class SceneNode;
	class SampleSet;
	class Animation;
	class AnimationSet;
	class Grid;
	class LayersManager;
	class GameState;
	class EntitiesManager;
	class Entity;
	class StateMachineComponent;
	class Transform;
	class DrawPosition;
	class Animator;
	class Renderer;
	class Light;
	class ParticleEmitter;
	class Text;
	class CollisionFilter;
	class Sight;
	class Sensor;
	class DynamicBody;
	class StaticBody;
	class Laser;
	class TemporalPeriod;
	class PlayerPeriod;
	class ActionController;
	class MovingPlatform;
	class Button;
	class Sentry;
	class Patrol;
	class SecurityCamera;
	class InputController;
	class Navigator;
	class Door;
	class TemporalEcho;
	class EntitySaverLoader;

	class SerializationAccess
	{
	public:
		// Factory methods
		static void serialize(const char* key, Component*& component, BaseSerializer& serializer);
		static void serialize(const char* key, Component*& component, XmlDeserializer& serializer);

		template<class T, class S>
		static void serialize(const char* key, T*& value, S& serializer)
		{
			SerializationAccess::serialize(key, *value, serializer);
		}

		template<class T>
		static void serialize(const char* key, T*& value, XmlDeserializer& serializer)
		{
			value = new T();
			SerializationAccess::serialize(key, *value, serializer);
		}

		// Physics objects
		template<class T>
		static void serialize(const char* key, Fixture& fixture, T& serializer) 
		{
			// xml
			serializer.serialize("yabp", fixture._localShape);
		}

		template<class T>
		static void serialize(const char* key, Vector& vector, T& serializer)
		{
			serializer.serialize("x", vector._x);
			serializer.serialize("y", vector._y);
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
		static void serialize(const char* key, YABP& yabp, T& serializer)
		{
			serializer.serialize("center", yabp._center);
			serializer.serialize("sloped-radius", yabp._slopedRadius);
			serializer.serialize("y-radius", yabp._yRadius);
		}

		// Graphics objects
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
			serializer.serialize("texture", spritesheet._textureFile);
			serializer.serialize("orientation", (int&)spritesheet._orientation);
			serializer.serialize("sprite-group", spritesheet._spriteGroups);
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
		static void serialize(const char* key, SceneNode& sceneNode, T& serializer)
		{
			serializer.serialize("id", sceneNode._id);
			serializer.serialize("behind-parent", sceneNode._drawBehindParent);
			serializer.serialize("transform", sceneNode._transformOnly);
			serializer.serialize("sprite-group", sceneNode._spriteGroupId);
			serializer.serialize("scene-node", sceneNode._children);
		}
		
		template<class T>
		static void serialize(const char* key, SampleSet& sampleSet, T& serializer)
		{
			serializer.serialize("scene-node-id", sampleSet._sceneNodeId);
			serializer.serialize("sample", sampleSet._samples);
		}
		
		template<class T>
		static void serialize(const char* key, Animation& animation, T& serializer)
		{
			serializer.serialize("id", animation._id);
			serializer.serialize("repeat", animation._repeat);
			serializer.serialize("rewind", animation._rewind);
			serializer.serialize("sample-set", animation._sampleSets);
		}
		
		template<class T>
		static void serialize(const char* key, AnimationSet& animationSet, T& serializer)
		{
			serializer.serialize("animation", animationSet._animations);
		}

		// Entity objects
		template<class T>
		static void serialize(const char* key, Settings& settings, T& serializer)
		{
			serializer.serialize("resolution", settings._resolution);
			serializer.serialize("view-y", settings._viewY);
			serializer.serialize("fullscreen", settings._fullscreen);
		}

		template<class T>
		static void serialize(const char* key, GameState& gameState, T& serializer)
		{
			serializer.serialize("grid", *gameState._grid);
			serializer.serialize("entities-manager", *gameState._entitiesManager);
			serializer.serialize("layers-manager", *gameState._layersManager);
		}

		template<class T>
		static void serialize(const char* key, Grid& grid, T& serializer)
		{
			serializer.serialize("tile-size", grid._tileSize);
			serializer.serialize("world-size", grid._worldSize);
		}

		template<class T>
		static void serialize(const char* key, EntitiesManager& entitiesManager, T& serializer)
		{
			serializer.serialize("entity", entitiesManager._entities);
		}

		template<class T>
		static void serialize(const char* key, LayersManager& layersManager, T& serializer)
		{
			serializer.serialize("camera", layersManager._camera);
			serializer.serialize("ambient-color", layersManager._ambientColor);
		}

		template<class T>
		static void serialize(const char* key, Entity& entity, T& serializer)
		{
			serializer.serialize("id", entity._id);
			serializer.serialize("component", entity._components);
		}
		
		// TODO:
		template<class T>
		static void serialize(const char* key, StateMachineComponent& stateMachineComponent, T& serializer)
		{
			serializer.serialize("state", stateMachineComponent._currentStateID);
			if(serializer.type() == SerializationDirection::DESERIALIZATION)
				stateMachineComponent.setState(stateMachineComponent._currentStateID);
			serializer.serialize("timer", stateMachineComponent._timer);
			serializer.serialize("permanent-flag", stateMachineComponent._permanentFlag);
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
			serializer.serialize("offset", drawPosition._offset); // xml
			serializer.serialize("override", drawPosition._override); // memory
		}
		
		template<class T>
		static void serialize(const char* key, Animator& animator, T& serializer)
		{
			serializer.serialize("animation-set", animator._animationSetFile); // xml
			serializer.serialize("animation", animator._animationId); 
			serializer.serialize("timer", animator._timer); // memory
		}
		
		template<class T>
		static void serialize(const char* key, Renderer& renderer, T& serializer)
		{
			serializer.serialize("texture", renderer._textureFile);
			serializer.serialize("sprite-sheet", renderer._spriteSheetFile);
			serializer.serialize("layer", (int&)renderer._layer);
			serializer.serialize("color", renderer._color);
			serializer.serialize("scene-node", renderer._root);
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
			serializer.serialize("texture", particleEmitter._textureFile);
			serializer.serialize("sprite-sheet", particleEmitter._spritesheetFile);
			serializer.serialize("birth-radius", particleEmitter._birthRadius);
			serializer.serialize("velocity", particleEmitter._velocity);
			serializer.serializeRadians("center", particleEmitter._directionCenter);
			serializer.serializeRadians("size", particleEmitter._directionSize);
		}
		
		template<class T>
		static void serialize(const char* key, Text& text, T& serializer)
		{
			serializer.serialize("font-family", text._fontFamily);
			serializer.serialize("font-size", text._fontSize);
			serializer.serialize("text", text._text);
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
			serializer.serialize("id", sensor._id);
			serializer.serialize("category-mask", sensor._categoryMask);
			serializer.serialize("fixture", sensor._fixture);
		}
		
		template<class T>
		static void serialize(const char* key, DynamicBody& dynamicBody, T& serializer)
		{
			serializer.serialize("fixture", dynamicBody._fixture); // xml
			serializer.serialize("gravity-enabled", dynamicBody._gravityEnabled); // memory
			serializer.serialize("velocity", dynamicBody._velocity); // memory
		}
		
		template<class T>
		static void serialize(const char* key, StaticBody& staticBody, T& serializer)
		{
			serializer.serialize("fixture", staticBody._fixture);
		}
		
		template<class T>
		static void serialize(const char* key, Laser& laser, T& serializer)
		{
			serializer.serialize("platform", laser._platformID); // xml
			serializer.serialize("direction", laser._isPositiveDirection); // memory
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
		
		template<class T>
		static void serialize(const char* key, ActionController& actionController, T& serializer)
		{
			serializer.serialize("jump-type", (int&)actionController.getJumpHelper()._type);
			serialize(key, (StateMachineComponent&)actionController, serializer);
		}
		
		template<class T>
		static void serialize(const char* key, MovingPlatform& component, T& serializer)
		{
			serializer.serialize("point1", component._point1);
			serializer.serialize("point2", component._point2);
		}
		
		template<class T>
		static void serialize(const char* key, Button& button, T& serializer)
		{
			serializer.serialize("target", button._target);
		}

		template<class T>
		static void serialize(const char* key, Sentry& component, T& serializer)
		{
			serialize(key, (StateMachineComponent&)component, serializer);
		}

		template<class T>
		static void serialize(const char* key, SecurityCamera& component, T& serializer)
		{
			serialize(key, (StateMachineComponent&)component, serializer);
		}

		template<class T>
		static void serialize(const char* key, Patrol& component, T& serializer)
		{
			serialize(key, (StateMachineComponent&)component, serializer);
		}

		template<class T>
		static void serialize(const char* key, InputController& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, Navigator& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, Door& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, TemporalEcho& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, EntitySaverLoader& component, T& serializer) {}
	};
}

#endif