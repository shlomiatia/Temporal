#include "TestLevel.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "EntitySystem.h"
#include "Transform.h"
#include "DrawPosition.h"
#include "ActionController.h"
#include "MovementUtils.h"
#include "TemporalEcho.h"
#include "TemporalPeriod.h"
#include "StaticBody.h"
#include "DynamicBody.h"
#include "Grid.h"
#include "Sensor.h"
#include "Sight.h"
#include "InputController.h"
#include "SpriteSheet.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Graphics.h"
#include "Animator.h"
#include "Animation.h"
#include "Renderer.h"
#include "Navigator.h"
#include "Sentry.h"
#include "Patrol.h"
#include "SecurityCamera.h"
#include "Laser.h"
#include "MessageUtils.h"
#include "Lighting.h"
#include "Particles.h"
#include "Fixture.h"
#include "CollisionFilter.h"
#include "PhysicsEnums.h"
#include "Input.h"
#include "Camera.h"
#include "Layer.h"
#include "Serialization.h"
#include "ResourceManager.h"
#include "tinyxml2.h"

namespace Temporal
{
	void TestLevel::init()
	{
		Size resolution = Size(1280.0f, 720.0f);
		Size levelSize = Size(2048.0f, 720.0f);
		float relativeHeight = 720.0f;

		Grid::get().init(levelSize, 128.0f);
		Graphics::get().init(resolution, relativeHeight);
		ResourceManager::get().init();

		LayersManager::get().add(new Camera(levelSize));
		LayersManager::get().add(new SpriteLayer());
		//LayersManager::get().add(new LightLayer(Color(0.1f, 0.1f, 0.1f)));
		LayersManager::get().add(new DebugLayer());

		createEntities();

		NavigationGraph::get().init();
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_CREATED));
	}

	void TestLevel::update(float framePeriod)
	{
		Input::get().update();
		
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		if(Input::get().isQ())
		{
			const AABB& bounds = *static_cast<AABB*>(EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			//EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}

		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));		
	}

	void TestLevel::draw() const
	{
		LayersManager::get().draw();
	}

	void TestLevel::dispose()
	{
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_DESTROYED));
		NavigationGraph::get().dispose();
		EntitiesManager::get().dispose();
		LayersManager::get().dispose();
		ResourceManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
		HashToString::get().dispose();
	}

	SceneNode* deserializeSceneNode(tinyxml2::XMLElement* element)
	{
		SceneNode* sceneNode = new SceneNode();
		XmlDeserializer deserializer(element);
		sceneNode->serialize(deserializer);
		for(tinyxml2::XMLElement* child = element->FirstChildElement("scene-node"); child != NULL; child = child->NextSiblingElement())
		{
			sceneNode->add(deserializeSceneNode(child));
		}
		return sceneNode;
	}

	void TestLevel::createEntities()
	{
		tinyxml2::XMLDocument document;
		document.LoadFile("entities.xml");
		for(tinyxml2::XMLElement* entityElement = document.FirstChildElement("entity"); entityElement != NULL; entityElement = entityElement->NextSiblingElement())
		{
			XmlDeserializer entityDeserializer(entityElement);
			Entity* entity = new Entity();
			entity->serialize(entityDeserializer);
			for(tinyxml2::XMLElement* componentElement = entityElement->FirstChildElement(); componentElement != NULL; componentElement = componentElement->NextSiblingElement())
			{
				XmlDeserializer componentDeserializer(componentElement);
				if(strcmp(componentElement->Name(), "transform") == 0)
				{
					Transform* transform = new Transform();
					transform->serialize(componentDeserializer);
					entity->add(transform);
				}
				else if(strcmp(componentElement->Name(), "collision-filter") == 0)
				{
					CollisionFilter* collisionFilter = new CollisionFilter();
					collisionFilter->serialize(componentDeserializer);
					entity->add(collisionFilter);
				}
				else if(strcmp(componentElement->Name(), "draw-position") == 0)
				{
					DrawPosition* drawPosition = new DrawPosition();
					drawPosition->serialize(componentDeserializer);
					entity->add(drawPosition);
				}
				else if(strcmp(componentElement->Name(), "sight") == 0)
				{
					Sight* sight = new Sight();
					sight->serialize(componentDeserializer);
					entity->add(sight);
				}
				else if(strcmp(componentElement->Name(), "animator") == 0)
				{
					Animator* animator = new Animator();
					animator->serialize(componentDeserializer);
					entity->add(animator);
				}
				else if(strcmp(componentElement->Name(), "renderer") == 0)
				{
					SceneNode* root = deserializeSceneNode(componentElement->FirstChildElement("scene-node"));
					Renderer* renderer = new Renderer(root);
					renderer->serialize(componentDeserializer);
					entity->add(renderer);
				}
				else if(strcmp(componentElement->Name(), "light") == 0)
				{
					Light* light = new Light();
					light->serialize(componentDeserializer);
					entity->add(light);
				}
				else if(strcmp(componentElement->Name(), "particle-emitter") == 0)
				{
					ParticleEmitter* particleEmitter = new ParticleEmitter();
					particleEmitter->serialize(componentDeserializer);
					entity->add(particleEmitter);
				}
				else if(strcmp(componentElement->Name(), "input-controller") == 0)
				{
					InputController* inputController = new InputController();
					entity->add(inputController);
				}
				else if(strcmp(componentElement->Name(), "sentry") == 0)
				{
					Sentry* sentry = new Sentry();
					entity->add(sentry);
				}
				else if(strcmp(componentElement->Name(), "security-camera") == 0)
				{
					SecurityCamera* sentry = new SecurityCamera();
					entity->add(sentry);
				}
				else if(strcmp(componentElement->Name(), "navigator") == 0)
				{
					Navigator* navigator = new Navigator();
					entity->add(navigator);
				}
				else if(strcmp(componentElement->Name(), "laser") == 0)
				{
					Laser* laser = new Laser();
					laser->serialize(componentDeserializer);
					entity->add(laser);
				}
				else if(strcmp(componentElement->Name(), "action-controller") == 0)
				{
					ActionController* actionController = new ActionController();
					entity->add(actionController);
				}
				else if(strcmp(componentElement->Name(), "patrol") == 0)
				{
					Patrol* patrol = new Patrol();
					entity->add(patrol);
				}
				else if(strcmp(componentElement->Name(), "temporal-echo") == 0)
				{
					TemporalEcho* temporalEcho = new TemporalEcho();
					entity->add(temporalEcho);
				}
				else if(strcmp(componentElement->Name(), "temporal-period") == 0)
				{
					TemporalPeriod* temporalPeriod = new TemporalPeriod();
					temporalPeriod->serialize(componentDeserializer);
					entity->add(temporalPeriod);
				}
				else if(strcmp(componentElement->Name(), "player-period") == 0)
				{
					PlayerPeriod* playerPeriod = new PlayerPeriod();
					playerPeriod->serialize(componentDeserializer);
					entity->add(playerPeriod);
				}
				else if(strcmp(componentElement->Name(), "sensor") == 0)
				{
					tinyxml2::XMLElement* shapeElement = componentElement->FirstChildElement("aabb");
					XmlDeserializer shapeDeserializer(shapeElement);
					AABB* aabb = new AABB();
					aabb->serialize(shapeDeserializer);
					Fixture* fixture = new Fixture(aabb);
					tinyxml2::XMLElement* ledgeDetectorElement = componentElement->FirstChildElement("ledge-detector");
					XmlDeserializer ledgeDetectorDeserializer(ledgeDetectorElement);
					LedgeDetector* ledgeDetector = new LedgeDetector();
					ledgeDetector->serialize(ledgeDetectorDeserializer);
					Sensor* sensor = new Sensor(fixture, ledgeDetector);
					sensor->serialize(componentDeserializer);
					entity->add(sensor);
				}
				else if(strcmp(componentElement->Name(), "dynamic-body") == 0)
				{
					tinyxml2::XMLElement* shapeElement = componentElement->FirstChildElement();
					XmlDeserializer shapeDeserializer(shapeElement);
					AABB* aabb = new AABB();
					aabb->serialize(shapeDeserializer);
					Fixture* fixture = new Fixture(aabb);
					DynamicBody* dynamicBody = new DynamicBody(fixture);
					entity->add(dynamicBody);
				}
				else if(strcmp(componentElement->Name(), "static-body") == 0)
				{
					tinyxml2::XMLElement* shapeElement = componentElement->FirstChildElement();
					XmlDeserializer shapeDeserializer(shapeElement);
					Segment* segment = new Segment();
					segment->serialize(shapeDeserializer);
					Fixture* fixture = new Fixture(segment);
					StaticBody* staticBody = new StaticBody(fixture);
					entity->add(staticBody);
				}
			}
			EntitiesManager::get().add(entity);
		}
	}
}
