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

		Graphics::get().init(resolution, relativeHeight);

		LayersManager::get().add(new Camera(levelSize));
		LayersManager::get().add(new SpriteLayer());
		//LayersManager::get().add(new LightLayer(Color(0.1f, 0.1f, 0.1f)));
		LayersManager::get().add(new DebugLayer());

		Grid::get().init(levelSize, 128.0f);
		ResourceManager::get().init();

		createEntities();

		NavigationGraph::get().init();
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_CREATED));
		/*Period::Enum period = Period::PRESENT;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_CURRENT_PERIOD, &period));*/
	}

	

	void TestLevel::update(float framePeriodInMillis)
	{
		Input::get().update();
		
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		/*if(Input::get().isQ())
		{
			//const AABB& bounds = *static_cast<AABB*>(EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			//EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			ChangePeriod(Period::PAST);
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));

		}
		if(Input::get().isW())
		{
			ChangePeriod(Period::PRESENT);
		}
		if(Input::get().isE())
		{
			ChangePeriod(Period::FUTURE);
		}*/
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));		
	}

	void TestLevel::draw() const
	{
		LayersManager::get().draw();
		
	}

	void TestLevel::dispose()
	{
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_DESTROYED));
		ResourceManager::get().dispose();
		HashToString::get().dispose();
		EntitiesManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
		NavigationGraph::get().dispose();
		LayersManager::get().dispose();
	}
	/*void ChangePeriod(Period::Enum period)
	{
		EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::SET_PERIOD, &period));
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_CURRENT_PERIOD, &period));
	}
	SceneNode* createDefaultSceneGraph()
	{
		return new SceneNode(Hash("SCN_ROOT"));
	}

	void addAnimation(AnimationCollection* animations, Hash animationID, Hash sceneNodeID, Hash spriteGroupID, float duration = 0.0f, const Vector& translation = Vector::Zero, float rotation = 0.0f )
	{
		Animation* animation = (Animation*)(*animations)[animationID];
		if(animation == NULL)
		{
			animation = new Animation();
			(*animations)[animationID] = animation;
		}
		animation->add(sceneNodeID, spriteGroupID, duration, translation, rotation);
	}

	void addJumpSensor(Entity& entity)
	{
		// Jump sensor
		float jumpSensorBackOffset = ENTITY_SIZE.getWidth() / 2.0f - 1.0f;
		float maxJumpDistance = getMaxJumpDistance(ANGLE_45_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getY());
		float jumpSensorWidth = maxJumpDistance / 2.0f + jumpSensorBackOffset; 
		float jumpSensorHeight = getMaxJumpHeight(ANGLE_90_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getY());
		float sensorOffsetX = jumpSensorWidth / 2.0f - jumpSensorBackOffset;
		float sensorOffsetY =  (ENTITY_SIZE.getHeight() + jumpSensorHeight) / 2.0f;
		Vector sensorOffset(sensorOffsetX, sensorOffsetY);
		Size sensorSize(jumpSensorWidth, jumpSensorHeight);
		Fixture* fixture = new Fixture(new AABB(sensorOffset, sensorSize));
		Sensor* sensor(new Sensor(fixture, new LedgeDetector(Hash("SNS_JUMP") ,-ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS), FilterType::OBSTACLE));
		entity.add(sensor);
	}

	void addHangSensor(Entity& entity)
	{
		// Hang Sensor
		float hangSensorBackOffset = ENTITY_SIZE.getWidth() - 1.0f;
		const float HANG_SENSOR_SIZE = 20.0f;
		Size sensorSize = Size(hangSensorBackOffset + HANG_SENSOR_SIZE, HANG_SENSOR_SIZE);
		float sensorOffsetX = sensorSize.getWidth() / 2.0f - (hangSensorBackOffset / 2.0f);
		float sensorOffsetY = (ENTITY_SIZE.getHeight() + sensorSize.getHeight()) / 2.0f;
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Fixture* fixture = new Fixture(new AABB(sensorOffset, sensorSize));
		Sensor* sensor = new Sensor(fixture, new LedgeDetector(Hash("SNS_HANG"), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS), FilterType::OBSTACLE);
		entity.add(sensor);
	}

	void addBackEdgeSensor(Entity& entity)
	{
		// Back Edge Sensor
		float sensorOffsetX = -(EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Fixture* fixture = new Fixture(new AABB(sensorOffset, EDGE_SENSOR_SIZE));
		Sensor* sensor = new Sensor(fixture, new LedgeDetector(Hash("SNS_BACK_EDGE"), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS), FilterType::OBSTACLE);
		entity.add(sensor);
	}

	void addFrontEdgeSensor(Entity& entity)
	{
		// Front edge sensor
		float sensorOffsetX = (EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Fixture* fixture = new Fixture(new AABB(sensorOffset, EDGE_SENSOR_SIZE));
		Sensor* sensor = new Sensor(fixture, new LedgeDetector(Hash("SNS_FRONT_EDGE"), -ANGLE_135_IN_RADIANS - ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS), FilterType::OBSTACLE);
		entity.add(sensor);
	}

	void addSensors(Entity& entity)
	{
		addJumpSensor(entity);
		addHangSensor(entity);
		addBackEdgeSensor(entity);
		addFrontEdgeSensor(entity);
	}
	
	void createTemporalEcho(Entity* entity)
	{
		TemporalEcho* temporalEcho = new TemporalEcho();
		entity->add(temporalEcho);
	}

	void createPlayer(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		const Texture* texture = Texture::load("bubble.png");
		SceneNode* root = createDefaultSceneGraph();

		Transform* transform = new Transform(Vector(512.0f, 768.0f), Side::LEFT);
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::PLAYER);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -ENTITY_SIZE.getHeight() / 2.0f));
		InputController* controller = new InputController();
		Fixture* info = new Fixture(new AABB(Vector::Zero, ENTITY_SIZE));
		DynamicBody* dynamicBody = new DynamicBody(info);
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(*animations);
		Renderer* renderer = new Renderer(*spritesheet, LayerType::PC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::PRESENT);
		LightGem* lightGem = new LightGem();
		ParticleEmitter* particleEmitter = new ParticleEmitter(texture, 3000.0f, 2);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(drawPosition);
		entity->add(controller);
		entity->add(dynamicBody);
		addSensors(*entity);
		entity->add(actionController);
		entity->add(animator);
		entity->add(renderer);
		//entity->add(particleEmitter);
		//entity->add(lightGem);
		//entity->add(temporalPeriod);
		EntitiesManager::get().add(Hash("ENT_PLAYER"), entity);
	}

	void createCamera()
	{
		const Texture* texture = Texture::load("camera.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Side::LEFT);
		SpriteGroup* spriteGroup;
		AnimationCollection* animations = new AnimationCollection();
		Hash animationID = Hash::INVALID;
		Hash sceneNodeID = Hash("SCN_ROOT");
		SceneNode* root = new SceneNode(sceneNodeID);
		
		#pragma region Camera spriteGroup
		// Search - 0
		spriteGroup = new SpriteGroup();
		animationID = Hash("CAM_ANM_SEARCH");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(19, 19, 24, 32), Vector(4, 16)));
		addAnimation(animations, animationID, sceneNodeID, animationID);
		// See - 1
		spriteGroup = new SpriteGroup();
		animationID = Hash("CAM_ANM_SEE");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(19, 59, 24, 32), Vector(4, 16)));
		addAnimation(animations, animationID, sceneNodeID, animationID);
		// Turn - 2
		spriteGroup = new SpriteGroup();
		animationID = Hash("CAM_ANM_TURN");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(50.5, 19.5, 17, 33), Vector(2, 16)));
		spriteGroup->add(new Sprite(AABB(76, 19.5, 12, 33), Vector(1, 16)));
		spriteGroup->add(new Sprite(AABB(98, 19.5, 12, 33), Vector(0, 16)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 200.0f);
		#pragma endregion

		Transform* transform = new Transform(Vector(383.0f, 383.0f), Side::LEFT);
		SecurityCamera* camera = new SecurityCamera();
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		Sight* sight = new Sight(-ANGLE_30_IN_RADIANS, ANGLE_30_IN_RADIANS, *collisionFilter);
		Animator* animator = new Animator(*animations);
		Renderer* renderer = new Renderer(*spritesheet, LayerType::NPC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::FUTURE);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(camera);
		entity->add(sight);
		entity->add(animator);
		entity->add(renderer);
		//entity->add(temporalPeriod);
		createTemporalEcho(entity);
		EntitiesManager::get().add(Hash("ENT_CAMERA"), entity);
	}

	void createPatrol(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		SceneNode* root = createDefaultSceneGraph();
		Fixture* info = new Fixture(new AABB(Vector::Zero, ENTITY_SIZE));

		Transform* transform = new Transform(Vector(512.0f, 768.0f), Side::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -ENTITY_SIZE.getHeight() / 2.0f));
		Patrol* patrol = new Patrol();
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		DynamicBody* dynamicBody = new DynamicBody(info);
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(*animations);
		Renderer* renderer = new Renderer(*spritesheet, LayerType::NPC, root);
		Sight* sight = new Sight(ANGLE_0_IN_RADIANS, ANGLE_60_IN_RADIANS, *collisionFilter);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::PRESENT);
		Light* light = new Light(Color::White, 500.0f, ANGLE_0_IN_RADIANS, ANGLE_30_IN_RADIANS);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(drawPosition);
		entity->add(patrol);
		entity->add(dynamicBody);
		entity->add(sight);
		addFrontEdgeSensor(*entity);
		entity->add(actionController);
		entity->add(animator);
		entity->add(renderer);
		//entity->add(light);
		//entity->add(temporalPeriod);
		createTemporalEcho(entity);
		EntitiesManager::get().add(Hash("ENT_PATROL"), entity);
	}

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
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(laser);
		entity->add(renderer);
		//entity->add(temporalPeriod);
		createTemporalEcho(entity);

		EntitiesManager::get().add(Hash("ENT_LASER"), entity);
	}

	void createChaser(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		Transform* transform = new Transform(Vector(512.0f, 768.0f), Side::LEFT);
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -ENTITY_SIZE.getHeight() / 2.0f));
		Navigator* navigator = new Navigator();
		Fixture* info = new Fixture(new AABB(Vector::Zero, ENTITY_SIZE));
		DynamicBody* dynamicBody = new DynamicBody(info);
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations);
		Renderer* renderer = new Renderer(*spritesheet, LayerType::PC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::PAST);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(drawPosition);
		entity->add(navigator);
		entity->add(dynamicBody);
		addSensors(*entity);
		entity->add(actionController);
		entity->add(animator);
		entity->add(renderer);
		//entity->add(temporalPeriod);
		createTemporalEcho(entity);

		EntitiesManager::get().add(Hash("ENT_CHASER"), entity);
	}

	void createPlatforms()
	{
		#pragma region Platforms

		// Edges
		createPlatform(new Segment(0.0f, 0.0f, 0.0f, 767.0f), spritesheet);
		createPlatform(new Segment(0.0f, 0.0f, 2047.0f, 0.0f), spritesheet);
		createPlatform(new Segment(2047.0f, 0.0f, 2047.0f, 767.0f), spritesheet);

		// Screen Splitter
		createPlatform(new Segment(1023.0f, 0.0f, 1023.0f, 256.0f), spritesheet);
		createPlatform(new Segment(1023.0f, 384.0f, 1023.0f, 767.0f), spritesheet);
		
		// Right lower platform
		createPlatform(new Segment(723.0f, 0.0f, 723.0f, 128.0f), spritesheet);
		createPlatform(new Segment(723.0f, 128.0f, 1023.0f, 128.0f), spritesheet);

		// Right upper platform
		createPlatform(new Segment(896.0f, 128.0f, 896.0f, 256.0f), spritesheet);
		createPlatform(new Segment(896.0f, 256.0f, 1023.0f, 256.0f), spritesheet);

		// Right floater
		createPlatform(new Segment(640.0f, 384.0f, 1023.0f, 384.0f), spritesheet);

		// Right balcony
		createPlatform(new Segment(767.0f, 512.0f, 1023.0f, 512.0f), spritesheet);
		createPlatform(new Segment(896.0f, 384.0f, 896.0f, 512.0f), spritesheet);

		// Right unreachable
		createPlatform(new Segment(896.0f, 640.0f, 1023.0f, 640.0f), spritesheet);
		createPlatform(new Segment(896.0f, 640.0f, 896.0f, 767.0f), spritesheet);
		
		// Left lower platform
		createPlatform(new Segment(300, 0.0f, 300.0f, 128.0f), spritesheet);
		createPlatform(new Segment(0.0f, 128.0f, 300, 128.0f), spritesheet);

		// Left upper platform
		createPlatform(new Segment(128.0f, 128.0f, 128.0f, 256.0f), spritesheet);
		createPlatform(new Segment(0.0f, 256.0f, 128.0f, 256.0f), spritesheet);

		// Left floater
		createPlatform(new Segment(0.0f, 384.0f, 384.0f, 384.0f), spritesheet);

		// Left balcony
		createPlatform(new Segment(0.0f, 512.0f, 256.0f, 512.0f), spritesheet);
		createPlatform(new Segment(128, 384.0f, 128.0f, 512.0f), spritesheet);

		// Left unrechable
		createPlatform(new Segment(0.0f, 640.0f, 128.0f, 640.0f), spritesheet);
		createPlatform(new Segment(128.0f, 640.0f, 128.0f, 767.0f), spritesheet);
		
		// Middle floaters
		createPlatform(new Segment(384.0f, 128.0f, 640.0f, 128.0f), spritesheet);
		createPlatform(new Segment(384.0f, 256.0f, 640.0f, 256.0f), spritesheet);

		// Cover
		createPlatform(new AABB(AABBLB(640.0f, 384.0f, 256.0f, 64.0f)), spritesheet, FilterType::COVER);

		// 30
		createPlatform(new Segment(1088.0f, 0.0f, 1216.0f, 64.0f), spritesheet);
		createPlatform(new Segment(1216.0f, 64.0f, 1280.0f, 64.0f), spritesheet);
		createPlatform(new Segment(1280.0f, 64.0f, 1408.0f, 0.0f), spritesheet);

		// 45
		createPlatform(new Segment(1472.0f, 0.0f, 1536.0f, 64.0f), spritesheet);
		createPlatform(new Segment(1536.0f, 64.0f, 1600.0f, 64.0f), spritesheet);
		createPlatform(new Segment(1600.0f, 64.0f, 1664.0f, 0.0f), spritesheet);

		// 60
		createPlatform(new Segment(1728.0f, 0.0f, 1792.0f, 128.0f), spritesheet);
		createPlatform(new Segment(1792.0f, 128.0f, 1856.0f, 128.0f), spritesheet);
		createPlatform(new Segment(1856.0f, 128.0f, 1920.0f, 0.0f), spritesheet);
		
		// Torches
		createPlatform(new Segment(1024.0f, 64.0f, 1088.0f, 128.0f), spritesheet);
		createPlatform(new Segment(1024.0f, 128.0f, 1088.0f, 128.0f), spritesheet);
		
		createPlatform(new Segment(1983.0f, 128.0f, 2047.0f, 64.0f), spritesheet);
		createPlatform(new Segment(1983.0f, 128.0f, 2047.0f, 128.0f), spritesheet);

		// Platforms
		createPlatform(new Segment(1088.0f, 256.0f, 1792.0f, 256.0f), spritesheet);
		createPlatform(new Segment(1856.0f, 256.0f, 2047.0f, 256.0f), spritesheet);

		// 30
		createPlatform(new Segment(1088.0f, 384.0f, 1216.0f, 448.0f), spritesheet);
		createPlatform(new Segment(1216.0f, 448.0f, 1344.0f, 384.0f), spritesheet);

		// 45
		createPlatform(new Segment(1408.0f, 384.0f, 1472.0f, 448.0f), spritesheet);
		createPlatform(new Segment(1472.0f, 448.0f, 1536.0f, 384.0f), spritesheet);

		// 60
		createPlatform(new Segment(1600.0f, 384.0f, 1664.0f, 512.0f), spritesheet);
		createPlatform(new Segment(1664.0f, 512.0f, 1728.0f, 384.0f), spritesheet);

		// Platforms
		createPlatform(new Segment(1855.0f, 384.0f, 2047.0f, 384.0f), spritesheet);
		createPlatform(new Segment(1088.0f, 512.0f, 1983.0f, 512.0f), spritesheet);

		// V
		createPlatform(new Segment(1472.0f, 640.0f, 1536.0f, 512.0f), spritesheet);
		createPlatform(new Segment(1536.0f, 512.0f, 1600.0f, 640.0f), spritesheet);
		createPlatform(new Segment(1472.0f, 640.0f, 1600.0f, 640.0f), spritesheet);

		#pragma endregion
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

	void createLight(const Vector& point)
	{
		Transform* transform = new Transform(point);
		Light* light = new Light(Color(1.0f, 1.0f, 1.0f), 300.0f);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(light);

		std::ostringstream lightID;
		lightID << "ENT_LIGHT" << sequence++;
		EntitiesManager::get().add(Hash(lightID.str().c_str()), entity);
	}*/

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
				else if(strcmp(componentElement->Name(), "static-body") == 0)
				{
					tinyxml2::XMLElement* shapeElement = componentElement->FirstChildElement();
					XmlDeserializer shapeDeserializer(shapeElement);

					if(strcmp(shapeElement->Name(), "segment") == 0)
					{
						Segment* segment = new Segment();
						segment->serialize(shapeDeserializer);
						Fixture* fixture = new Fixture(segment);
						StaticBody* staticBody = new StaticBody(fixture);
						entity->add(staticBody);
					}
				}
			}
			EntitiesManager::get().add(entity);
		}
		
		/*
		#pragma endregion

		createPlayer(spritesheet, animations);
		
		createSentry(spritesheet);
		createCamera();
		createPatrol(spritesheet, animations);
		createChaser(spritesheet, animations);
		createLaser();
		createPlatforms();
		createBackground(Vector(512.0f, 384.0f));
		createBackground(Vector(1536.0f, 384.0f));
		createSkeleton();
		createLight(Vector(700.0f, 300.0f));
		createLight(Vector(1500.0f, 300.0f));*/
	}
}
