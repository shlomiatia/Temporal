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
	class OBB;
	class Fixture;
	class Color;
	class Sprite;
	class SpriteGroup;
	class SpriteSheet;
	class SceneNode;
	class SceneNodeSample;
	class SceneGraphSample;
	class Animation;
	class AnimationSet;
	class Grid;
	class LayersManager;
	class GameState;
	class EntitiesManager;
	class Entity;
	class StateMachineComponent;
	class Transform;
	class Animator;
	class CompositeAnimator;
	class SingleAnimator;
	class Renderer;
	class Light;
	class ParticleEmitter;
	class CollisionFilter;
	class Sight;
	class Sensor;
	class DynamicBody;
	class StaticBody;
	class TemporalPeriod;
	class PlayerPeriod;
	class ActionController;
	class MovingPlatform;
	class Patrol;
	class InputController;
	class Navigator;
	class TemporalEcho;
	class VisibilityGem;
	class CameraControl;
	class Laser;
	class Button;
	class Door;

	// Singletons
	class GameSaverLoader;
	class AnimationEditor;
	class GameStateEditor;
	class DebugManager;

	class SerializationAccess
	{
	public:
		static void getConfig(const char*& key, Component*& value, BaseSerializer& serializer, bool& shouldSerialize);
		static void getConfig(const char*& key, Entity*& value, BaseSerializer& serializer, bool& shouldSerialize);

		template<class T, class S>
		static void getConfig(const char*& key, T*& value, S& serializer, bool& shouldSerialize){}

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
			serializer.serialize("obb", fixture._localShape);
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
		static void serialize(const char* key, OBB& obb, T& serializer)
		{
			serializer.serialize("center", obb._center);
			serializer.serialize("radius", obb._radius);

			// TODO:
			float angle = obb.getAngle();
			serializer.serializeRadians("angle", angle);
			obb.setAngle(angle);
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
		static void serialize(const char* key, SceneNode& sceneNode, T& serializer)
		{
			serializer.serialize("id", sceneNode._id);
			serializer.serialize("behind-parent", sceneNode._drawBehindParent);
			serializer.serialize("transform", sceneNode._transformOnly);
			serializer.serialize("sprite-group", sceneNode._spriteGroupId);
			serializer.serialize("flip", sceneNode._flip);
			serializer.serialize("rotation", sceneNode._rotation);
			serializer.serialize("translation", sceneNode._translation);
			serializer.serialize("scale", sceneNode._scale);
			serializer.serialize("center", sceneNode._center);
			serializer.serialize("radius", sceneNode._radius);
			serializer.serialize("scene-node", sceneNode._children);
		}

		template<class T>
		static void serialize(const char* key, SceneNodeSample& sample, T& serializer)
		{
			serializer.serialize("scene-node-id", sample._sceneNodeId);
			serializer.serialize("sprite-group", sample._spriteGroupId);
			serializer.serialize("translation", sample._translation);
			serializer.serialize("rotation", sample._rotation);
			serializer.serialize("ignore", sample._isIgnore);
		}
		
		template<class T>
		static void serialize(const char* key, SceneGraphSample& sample, T& serializer)
		{
			serializer.serialize("index", sample._index);
			serializer.serialize("scene-node-sample", sample._samples);
		}
		
		template<class T>
		static void serialize(const char* key, Animation& animation, T& serializer)
		{
			serializer.serialize("id", animation._id);
			serializer.serialize("repeat", animation._repeat);
			serializer.serialize("cross-fade", animation._crossFade);
			serializer.serialize("scene-graph-sample", animation._samples);
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
			serializer.serialize("layers-manager", *gameState._layersManager);
			serializer.serialize("grid", *gameState._grid);
			serializer.serialize("entities-manager", *gameState._entitiesManager);
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
			serializer.serialize("camera-follow-player", layersManager._cameraFollowPlayer);
			serializer.serialize("ambient-color", layersManager._ambientColor);
		}

		template<class T>
		static void serialize(const char* key, Entity& entity, T& serializer)
		{
			serializer.serialize("id", entity._id);
			serializer.serialize(0, entity._components);
		}
		
		template<class T>
		static void serialize(const char* key, StateMachineComponent& stateMachineComponent, T& serializer)
		{
			serializer.serialize("state", stateMachineComponent._currentStateID);
			serializer.serialize("timer", stateMachineComponent._timer);
			serializer.serialize("state-flag", stateMachineComponent._stateFlag);
		}
		
		template<class T>
		static void serialize(const char* key, Transform& transform, T& serializer)
		{
			serializer.serialize("position", transform._position);
			serializer.serialize("orientation", (int&)transform._orientation);
		}
		
		template<class T>
		static void serialize(const char* key, Animator& animator, T& serializer)
		{
			serializer.serialize("animation-set", animator._animationSetFile); // xml
			serializer.serialize("disable-cross-fade", animator._isDisableCrossFade); // xml
			serializer.serialize("use-animator2", animator._useAnimator2); // memory
			serializer.serialize("composite-animator1", animator._animator1); // memory
			serializer.serialize("composite-animator2", animator._animator2); // memory
		}

		template<class T>
		static void serialize(const char* key, CompositeAnimator& animator, T& serializer)
		{
			serializer.serialize("single-animator", animator._singleAnimators);
		}

		template<class T>
		static void serialize(const char* key, SingleAnimator& animator, T& serializer)
		{
			serializer.serialize("animation-id", animator._animationId);
			serializer.serialize("is-rewind", animator._isRewind);
			serializer.serialize("weight", animator._weight);
			serializer.serialize("timer", animator._timer);
		}
		
		template<class T>
		static void serialize(const char* key, Renderer& renderer, T& serializer)
		{
			serializer.serialize("texture", renderer._textureFile);
			serializer.serialize("sprite-sheet", renderer._spriteSheetFile);
			serializer.serialize("layer", (int&)renderer._layer);
			serializer.serialize("scene-node", renderer._root);
			serializer.serialize("color", renderer._color);
		}
		
		template<class T>
		static void serialize(const char* key, Light& light, T& serializer)
		{
			serializer.serialize("color", light._color);
			serializer.serialize("radius", light._radius);
			serializer.serialize("center", light._center);
			serializer.serialize("size", light._size);
			serializer.serialize("activated", light._activate);
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
			serializer.serialize("attachment", particleEmitter._attachment);
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
			serializer.serialize("size", sight._sightSize);
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
			serializer.serialize("gravity-enabled", dynamicBody._gravityEnabled); // xml
			serializer.serialize("body-enabled", dynamicBody._bodyEnabled); // memory
			serializer.serialize("velocity", dynamicBody._velocity); // memory
		}
		
		template<class T>
		static void serialize(const char* key, StaticBody& staticBody, T& serializer)
		{
			serializer.serialize("fixture", staticBody._fixture);
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
			serializer.serialize("max-walk-force-per-second", actionController.MAX_WALK_FORCE_PER_SECOND);
			serializer.serialize("hang-descend-movement", actionController._hangDescendMovement);
			serializer.serialize("hang-descnd-original-translation", actionController._hangDescendOriginalTranslation);
			serializer.serialize("hang-descend-start-position", actionController._hangDescendStartPosition);
			serialize(key, (StateMachineComponent&)actionController, serializer);
		}
		
		template<class T>
		static void serialize(const char* key, Patrol& component, T& serializer)
		{
			serialize(key, (StateMachineComponent&)component, serializer);
		}

		template<class T>
		static void serialize(const char* key, Navigator& navigator, T& serializer) 
		{
			serializer.serialize("destination", navigator._destination);
			serialize(key, (StateMachineComponent&)navigator, serializer);
		}

		template<class T>
		static void serialize(const char* key, Button& button, T& serializer)
		{
			serializer.serialize("target", button._target);
		}

		template<class T>
		static void serialize(const char* key, Door& door, T& serializer)
		{
			serializer.serialize("closed", door._closed);
		}

		template<class T>
		static void serialize(const char* key, Laser& laser, T& serializer) 
		{
			serializer.serialize("friendly", laser._friendly);
		}

		template<class T>
		static void serialize(const char* key, MovingPlatform& movingPlatform, T& serializer) 
		{
			serializer.serialize("movement", movingPlatform._movement);
		}

		template<class T>
		static void serialize(const char* key, InputController& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, TemporalEcho& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, VisibilityGem& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, CameraControl& component, T& serializer) {}

		// Singletons

		template<class T>
		static void serialize(const char* key, GameSaverLoader& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, AnimationEditor& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, GameStateEditor& component, T& serializer) {}

		template<class T>
		static void serialize(const char* key, DebugManager& component, T& serializer) {}
	};
}

#endif