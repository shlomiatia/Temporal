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
#include "LineRenderer.h"
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
#include <sstream>
#include "tinyxml2.h"

namespace Temporal
{
	static const Size ENTITY_SIZE(20.0f, 80.0f);
	static const Size EDGE_SENSOR_SIZE(ENTITY_SIZE.getWidth() + 20.0f, 20.0f);

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

	

	void TestLevel::update(float framePeriodInMillis)
	{
		Input::get().update();
		
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		if(Input::get().isQ())
		{
			//const AABB& bounds = *static_cast<AABB*>(EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			//EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));		
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
	/*

	void createLaser()
	{
		Transform* transform = new Transform(Vector::Zero);
		Laser* laser = new Laser(Hash("ENT_PLATFORM23"));
		const Texture* texture = Texture::load("laser.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture);
		SpriteGroup* spriteGroup = new SpriteGroup();
		Hash spriteGroupID = Hash("ANM_DEFAULT");
		spritesheet->add(spriteGroupID, spriteGroup);
		Size size = texture->getSize();
		spriteGroup->add(new Sprite(AABB(size.toVector() / 2.0f, size), Vector::Zero));
		SceneNode* root = new SceneNode(Hash("SCN_ROOT"), false, true);
		SceneNode* laserNode = new SceneNode(Hash("SCN_LASER"));
		root->setSpriteGroupID(spriteGroupID);
		LineRenderer* renderer = new LineRenderer(LayerType::NPC, *texture);
		//Renderer* renderer = new Renderer(*spritesheet, LayerType::NPC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::FUTURE);
		CollisionFilter* collisionFilter = new CollisionFilter(CollisionCategory::CHARACTER);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(laser);
		entity->add(renderer);
		//entity->add(temporalPeriod);
		createTemporalEcho(entity);

		EntitiesManager::get().add(Hash("ENT_LASER"), entity);
	}

	void createSkeleton()
	{
		const Texture* texture = Texture::load("skeleton.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Side::LEFT);
		SpriteGroup* spriteGroup;
		AnimationCollection* animations = new AnimationCollection();
		Hash animationID = Hash("ANM_WALK");
		SceneNode* root = new SceneNode(Hash("SCN_ROOT"), false, true);

		#pragma region SpriteGroup
		// SCN_RIGHT_LEG
		Hash SCN_RIGHT_LEG_ID = Hash("SCN_RIGHT_LEG");
		SceneNode* SCN_RIGHT_LEG = new SceneNode(Hash(SCN_RIGHT_LEG_ID), true);
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_RIGHT_LEG_ID, spriteGroup);
		SCN_RIGHT_LEG->setSpriteGroupID(SCN_RIGHT_LEG_ID);
		spriteGroup->add(new Sprite(AABB(20.5, 31.5, 41, 45), Vector(6, 9)));
		// SCN_LEFT_LEG
		Hash SCN_LEFT_LEG_ID = Hash("SCN_LEFT_LEG");
		SceneNode* SCN_LEFT_LEG = new SceneNode(Hash(SCN_LEFT_LEG_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_LEFT_LEG_ID, spriteGroup);
		SCN_LEFT_LEG->setSpriteGroupID(SCN_LEFT_LEG_ID);
		spriteGroup->add(new Sprite(AABB(69, 31.5, 36, 55), Vector(-3, 14)));
		// SCN_LEFT_FOOT
		Hash SCN_LEFT_FOOT_ID = Hash("SCN_LEFT_FOOT");
		SceneNode* SCN_LEFT_FOOT = new SceneNode(Hash(SCN_LEFT_FOOT_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_LEFT_FOOT_ID, spriteGroup);
		SCN_LEFT_FOOT->setSpriteGroupID(SCN_LEFT_FOOT_ID);
		spriteGroup->add(new Sprite(AABB(162.5, 33.5, 37, 53), Vector(-8, 20)));
		// SCN_RIGHT_FOOT
		Hash SCN_RIGHT_FOOT_ID = Hash("SCN_RIGHT_FOOT");
		SceneNode* SCN_RIGHT_FOOT = new SceneNode(Hash(SCN_RIGHT_FOOT_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_RIGHT_FOOT_ID, spriteGroup);
		SCN_RIGHT_FOOT->setSpriteGroupID(SCN_RIGHT_FOOT_ID);
		spriteGroup->add(new Sprite(AABB(114.5, 33.5, 37, 53), Vector(-8, 20)));
		// SCN_BODY
		Hash SCN_BODY_ID = Hash("SCN_BODY");
		SceneNode* SCN_BODY = new SceneNode(Hash(SCN_BODY_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_BODY_ID, spriteGroup);
		SCN_BODY->setSpriteGroupID(SCN_BODY_ID);
		spriteGroup->add(new Sprite(AABB(27.5, 100.5, 55, 75), Vector(5, -2)));
		// SCN_RIGHT_PALM
		Hash SCN_RIGHT_PALM_ID = Hash("SCN_RIGHT_PALM");
		SceneNode* SCN_RIGHT_PALM = new SceneNode(Hash(SCN_RIGHT_PALM_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_RIGHT_PALM_ID, spriteGroup);
		SCN_RIGHT_PALM->setSpriteGroupID(SCN_RIGHT_PALM_ID);
		spriteGroup->add(new Sprite(AABB(93, 103, 56, 58), Vector(21, 5)));
		// SCN_RIGHT_HAND
		Hash SCN_RIGHT_HAND_ID = Hash("SCN_RIGHT_HAND");
		SceneNode* SCN_RIGHT_HAND = new SceneNode(Hash(SCN_RIGHT_HAND_ID), true);
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_RIGHT_HAND_ID, spriteGroup);
		SCN_RIGHT_HAND->setSpriteGroupID(SCN_RIGHT_HAND_ID);
		spriteGroup->add(new Sprite(AABB(206.5, 102, 27, 42), Vector(4, 14)));
		// SCN_LEFT_HAND
		Hash SCN_LEFT_HAND_ID = Hash("SCN_LEFT_HAND");
		SceneNode* SCN_LEFT_HAND = new SceneNode(Hash(SCN_LEFT_HAND_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_LEFT_HAND_ID, spriteGroup);
		SCN_LEFT_HAND->setSpriteGroupID(SCN_LEFT_HAND_ID);
		spriteGroup->add(new Sprite(AABB(153.5, 98, 35, 46), Vector(-11, 15)));
		// SCN_LEFT_PALM
		Hash SCN_LEFT_PALM_ID = Hash("SCN_LEFT_PALM");
		SceneNode* SCN_LEFT_PALM = new SceneNode(Hash(SCN_LEFT_PALM_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_LEFT_PALM_ID, spriteGroup);
		SCN_LEFT_PALM->setSpriteGroupID(SCN_LEFT_PALM_ID);
		spriteGroup->add(new Sprite(AABB(107.5, 197.5, 59, 59), Vector(22, 23)));
		// SCN_HEAD
		Hash SCN_HEAD_ID = Hash("SCN_HEAD");
		SceneNode* SCN_HEAD = new SceneNode(Hash(SCN_HEAD_ID));
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_HEAD_ID, spriteGroup);
		SCN_HEAD->setSpriteGroupID(SCN_HEAD_ID);
		spriteGroup->add(new Sprite(AABB(38.5, 195, 73, 62), Vector(24, -11)));
		// SCN_SHADOW
		Hash SCN_SHADOW_ID = Hash("SCN_SHADOW");
		SceneNode* SCN_SHADOW = new SceneNode(Hash(SCN_SHADOW_ID), true);
		spriteGroup = new SpriteGroup();
		spritesheet->add(SCN_SHADOW_ID, spriteGroup);
		SCN_SHADOW->setSpriteGroupID(SCN_SHADOW_ID);
		spriteGroup->add(new Sprite(AABB(198, 212, 94, 30), Vector(-2, 2)));

		#pragma endregion

		root->add(SCN_BODY);
		SCN_BODY->add(SCN_SHADOW);
		SCN_BODY->add(SCN_RIGHT_HAND);
		SCN_RIGHT_HAND->add(SCN_RIGHT_PALM);
		SCN_BODY->add(SCN_RIGHT_LEG);
		SCN_RIGHT_LEG->add(SCN_RIGHT_FOOT);
		SCN_BODY->add(SCN_HEAD);
		SCN_BODY->add(SCN_LEFT_LEG);
		SCN_LEFT_LEG->add(SCN_LEFT_FOOT);
		SCN_BODY->add(SCN_LEFT_HAND);
		SCN_LEFT_HAND->add(SCN_LEFT_PALM);
		
		addAnimation(animations, animationID, SCN_BODY_ID, SCN_BODY_ID);
		addAnimation(animations, animationID, SCN_HEAD_ID, SCN_HEAD_ID, 0.0f, Vector(-9,31));
		addAnimation(animations, animationID, SCN_LEFT_HAND_ID, SCN_LEFT_HAND_ID, 500.0f, Vector(1,22));
		addAnimation(animations, animationID, SCN_LEFT_HAND_ID, SCN_LEFT_HAND_ID, 500.0f, Vector(1,22), -90.0f);
		addAnimation(animations, animationID, SCN_LEFT_PALM_ID, SCN_LEFT_PALM_ID, 0.0f, Vector(19,-38));
		addAnimation(animations, animationID, SCN_RIGHT_HAND_ID, SCN_RIGHT_HAND_ID, 250.0f, Vector(-28,28), -45.0f);
		addAnimation(animations, animationID, SCN_RIGHT_HAND_ID, SCN_RIGHT_HAND_ID, 500.0f, Vector(-28,28), -90.0f);
		addAnimation(animations, animationID, SCN_RIGHT_HAND_ID, SCN_RIGHT_HAND_ID, 250.0f, Vector(-28,28), 0.0f);
		addAnimation(animations, animationID, SCN_RIGHT_PALM_ID, SCN_RIGHT_PALM_ID, 0.0f, Vector(-10,-26));
		addAnimation(animations, animationID, SCN_LEFT_LEG_ID, SCN_LEFT_LEG_ID, 0.0f, Vector(6,-29));
		addAnimation(animations, animationID, SCN_LEFT_FOOT_ID, SCN_LEFT_FOOT_ID, 0.0f, Vector(0,-38));
		addAnimation(animations, animationID, SCN_RIGHT_LEG_ID, SCN_RIGHT_LEG_ID, 0.0f, Vector(-13,-39));
		addAnimation(animations, animationID, SCN_RIGHT_FOOT_ID, SCN_RIGHT_FOOT_ID, 0.0f, Vector(-18,-29));
		addAnimation(animations, animationID, SCN_SHADOW_ID, SCN_SHADOW_ID, 0.0f, Vector(-5,-101));

		Transform* transform = new Transform(Vector(300.0f, 300.0f), Side::RIGHT);
		Animator* animator = new Animator(*animations);
		Renderer* renderer = new Renderer(*spritesheet, LayerType::NPC, root);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(animator);
		entity->add(renderer);
		//createTemporalEcho(entity);
		entity->handleMessage(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animationID, true, true)));
		EntitiesManager::get().add(Hash("ENT_SKELETON"), entity);
	}
*/

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
					Renderer* renderer = new Renderer();
					renderer->serialize(componentDeserializer);
					entity->add(renderer);
				}
				else if(strcmp(componentElement->Name(), "light") == 0)
				{
					Light* light = new Light();
					light->serialize(componentDeserializer);
					entity->add(light);
				}
				else if(strcmp(componentElement->Name(), "input-controller") == 0)
				{
					InputController* inputController = new InputController();
					entity->add(inputController);
				}
				else if(strcmp(componentElement->Name(), "senry") == 0)
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
