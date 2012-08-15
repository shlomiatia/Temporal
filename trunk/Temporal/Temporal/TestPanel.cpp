#include "TestPanel.h"
#include "NumericPair.h"
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
#include "SceneNodeSample.h"
#include "Renderer.h"
#include "LineRenderer.h"
#include "Navigator.h"
#include "Sentry.h"
#include "Patrol.h"
#include "Camera.h"
#include "Laser.h"
#include "MessageUtils.h"
#include "Lighting.h"
#include "Particles.h"
#include "Fixture.h"
#include "CollisionFilter.h"
#include "PhysicsEnums.h"
#include <sstream>

namespace Temporal
{
	static const Size ENTITY_SIZE(20.0f, 80.0f);
	static const Size EDGE_SENSOR_SIZE(ENTITY_SIZE.getWidth() + 20.0f, 20.0f);

	void addJumpSensor(Entity& entity)
	{
		// Jump sensor
		float jumpSensorBackOffset = ENTITY_SIZE.getWidth() / 2.0f - 1.0f;
		float maxJumpDistance = getMaxJumpDistance(ANGLE_45_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy());
		float jumpSensorWidth = maxJumpDistance / 2.0f + jumpSensorBackOffset; 
		float jumpSensorHeight = getMaxJumpHeight(ANGLE_90_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy());
		float sensorOffsetX = jumpSensorWidth / 2.0f - jumpSensorBackOffset;
		float sensorOffsetY =  (ENTITY_SIZE.getHeight() + jumpSensorHeight) / 2.0f;
		Vector sensorOffset(sensorOffsetX, sensorOffsetY);
		Size sensorSize(jumpSensorWidth, jumpSensorHeight);
		Fixture* fixture = new Fixture((const Transform&)*entity.get(ComponentType::TRANSFORM), (const CollisionFilter&)*entity.get(ComponentType::COLLISION_FILTER), new AABB(sensorOffset, sensorSize));
		Sensor* sensor(new Sensor(fixture, new LedgeDetector(Hash("SNS_JUMP") ,-ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS)));
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
		Fixture* fixture = new Fixture((const Transform&)*entity.get(ComponentType::TRANSFORM), (const CollisionFilter&)*entity.get(ComponentType::COLLISION_FILTER), new AABB(sensorOffset, sensorSize));
		Sensor* sensor = new Sensor(fixture, new LedgeDetector(Hash("SNS_HANG"), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS));
		entity.add(sensor);
	}

	void addBackEdgeSensor(Entity& entity)
	{
		// Back Edge Sensor
		float sensorOffsetX = -(EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Fixture* fixture = new Fixture((const Transform&)*entity.get(ComponentType::TRANSFORM), (const CollisionFilter&)*entity.get(ComponentType::COLLISION_FILTER), new AABB(sensorOffset, EDGE_SENSOR_SIZE));
		Sensor* sensor = new Sensor(fixture, new LedgeDetector(Hash("SNS_BACK_EDGE"), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS));
		entity.add(sensor);
	}

	void addFrontEdgeSensor(Entity& entity)
	{
		// Front edge sensor
		float sensorOffsetX = (EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Fixture* fixture = new Fixture((const Transform&)*entity.get(ComponentType::TRANSFORM), (const CollisionFilter&)*entity.get(ComponentType::COLLISION_FILTER), new AABB(sensorOffset, EDGE_SENSOR_SIZE));
		Sensor* sensor = new Sensor(fixture, new LedgeDetector(Hash("SNS_FRONT_EDGE"), -ANGLE_135_IN_RADIANS - ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS));
		entity.add(sensor);
	}

	void addSensors(Entity& entity)
	{
		addJumpSensor(entity);
		addHangSensor(entity);
		addBackEdgeSensor(entity);
		addFrontEdgeSensor(entity);
	}

	SceneNode* createDefaultSceneGraph()
	{
		return new SceneNode(Hash("SCN_ROOT"));
	}

	void addAnimation(AnimationCollection* animations, const Hash& animationID, const Hash& sceneNodeID, const Hash& spriteGroupID, float duration = 0.0f, const Vector& translation = Vector::Zero, float rotation = 0.0f )
	{
		Animation* animation = (Animation*)(*animations)[animationID];
		if(animation == NULL)
		{
			animation = new Animation();
			(*animations)[animationID] = animation;
		}
		animation->add(sceneNodeID, spriteGroupID, duration, translation, rotation);
	}

	void createTemporalEcho(Entity* entity, AnimationCollection* animations = NULL)
	{
		const Component* transform = entity->get(ComponentType::TRANSFORM);
		const Component* drawPosition = entity->get(ComponentType::DRAW_POSITION);
		const Component* renderer = entity->get(ComponentType::RENDERER);
		
		Entity* echoEntity = new Entity();
		if(transform != NULL)
			echoEntity->add(transform->clone());
		if(drawPosition != NULL)
			echoEntity->add(drawPosition->clone());
		if(renderer != NULL)
		{
			Component* rendererClone = renderer->clone();
			if(animations != NULL)
			{
				Animator* animator = new Animator(*animations, ((Renderer*)rendererClone)->getRoot());
				echoEntity->add(animator);
			}
			echoEntity->add(rendererClone);
		}

		TemporalEcho* temporalEcho = new TemporalEcho(echoEntity);
		entity->add(temporalEcho);
	}

	void createPlayer(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		Transform* transform = new Transform(Point(512.0f, 768.0f), Side::LEFT);
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::PLAYER);
		DrawPosition* drawPosition = new DrawPosition(Point(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		InputController* controller = new InputController();
		Fixture* info = new Fixture(*transform, *collisionFilter, new AABB(Point::Zero, ENTITY_SIZE));
		DynamicBody* dynamicBody = new DynamicBody(info);
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::PRESENT);
		LightGem* lightGem = new LightGem();
		const Texture* texture = Texture::load("bubble.png");
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

	void createSentry(SpriteSheet* spritesheet)
	{
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		Transform* transform = new Transform(Point(100.0f, 550.0f), Side::RIGHT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Sentry* sentry = new Sentry();
		Sight* sight = new Sight(ANGLE_0_IN_RADIANS, ANGLE_60_IN_RADIANS, *collisionFilter);
		SceneNode* root = createDefaultSceneGraph();
		root->setSpriteGroupID(Hash("POP_ANM_STAND"));
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::PAST);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(drawPosition);
		entity->add(sentry);
		entity->add(sight);
		entity->add(renderer);
		//createTemporalEcho(entity);
		//entity->add(temporalPeriod);
		EntitiesManager::get().add(Hash("ENT_SENTRY"), entity);
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
		root->setSpriteGroupID(animationID);
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

		Transform* transform = new Transform(Point(383.0f, 383.0f), Side::LEFT);
		Camera* camera = new Camera();
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		Sight* sight = new Sight(-ANGLE_30_IN_RADIANS, ANGLE_30_IN_RADIANS, *collisionFilter);
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::FUTURE);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(camera);
		entity->add(sight);
		entity->add(animator);
		entity->add(renderer);
		//createTemporalEcho(entity);
		//entity->add(temporalPeriod);
		EntitiesManager::get().add(Hash("ENT_CAMERA"), entity);
	}

	void createPatrol(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		Transform* transform = new Transform(Point(512.0f, 768.0f), Side::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Point(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Patrol* patrol = new Patrol();
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		Fixture* info = new Fixture(*transform, *collisionFilter, new AABB(Point::Zero, ENTITY_SIZE));
		DynamicBody* dynamicBody = new DynamicBody(info);
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);
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
		//createTemporalEcho(entity, animations);
		EntitiesManager::get().add(Hash("ENT_PATROL"), entity);
	}

	void createChaser(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		Transform* transform = new Transform(Point(512.0f, 768.0f), Side::LEFT);
		CollisionFilter* collisionFilter = new CollisionFilter(FilterType::CHARACTER);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Navigator* navigator = new Navigator();
		Fixture* info = new Fixture(*transform, *collisionFilter, new AABB(Point::Zero, ENTITY_SIZE));
		DynamicBody* dynamicBody = new DynamicBody(info);
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC, root);
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
		//createTemporalEcho(entity);

		EntitiesManager::get().add(Hash("ENT_CHASER"), entity);
	}

	static int platformID = 0;
	void createPlatform(Shape* shape, SpriteSheet* spritesheet, int filter = FilterType::OBSTACLE) 
	{
		Transform* transform = new Transform(shape->getCenter());
		shape->setCenter(Point::Zero);
		CollisionFilter* collisionFilter = new CollisionFilter(filter);
		Fixture* fixture = new Fixture(*transform, *collisionFilter, shape);
		StaticBody* staticBody = new StaticBody(fixture);
		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(collisionFilter);
		entity->add(staticBody);
		std::ostringstream animationID;
		animationID << "ENT_PLATFORM" << platformID++;
		EntitiesManager::get().add(Hash(animationID.str().c_str()), entity);
		
	}

	void createPlatforms()
	{
		SpriteSheet* spritesheet = NULL;
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

	void createBackground(const Point& position)
	{
		const Texture* texture = Texture::load("bg.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture);
		SpriteGroup* spriteGroup = new SpriteGroup();
		const Hash& spriteGroupID = Hash("ANM_DEFAULT");
		spritesheet->add(spriteGroupID, spriteGroup);
		Size size = texture->getSize();
		spriteGroup->add(new Sprite(AABB(size / 2.0f, size), Vector::Zero));
		SceneNode* root = createDefaultSceneGraph();
		root->setSpriteGroupID(spriteGroupID);
		Transform* transform = new Transform(position);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::BACKGROUND, root);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(renderer);
		std::ostringstream animationID;
		animationID << "ENT_BACKGROUND" << platformID++;
		EntitiesManager::get().add(Hash(animationID.str().c_str()), entity);
	}

	void createLaser()
	{
		Transform* transform = new Transform(Point(100.0f, 100.0f));
		Laser* laser = new Laser(Hash("ENT_PLATFORM23"));
		const Texture* texture = Texture::load("laser.png");
		LineRenderer* renderer = new LineRenderer(VisualLayer::NPC, *texture);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::FUTURE);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(laser);
		entity->add(renderer);
		//entity->add(temporalPeriod);

		//createTemporalEcho(entity);
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

		Transform* transform = new Transform(Point(300.0f, 300.0f), Side::RIGHT);
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);

		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(animator);
		entity->add(renderer);
		//createTemporalEcho(entity, animations);
		entity->handleMessage(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animationID, true, true)));
		EntitiesManager::get().add(Hash("ENT_SKELETON"), entity);
	}

	void createLight(const Point& point)
	{
		Transform* transform = new Transform(point);
		Light* light = new Light(Color(1.0f, 1.0f, 1.0f), 300.0f);
		Entity* entity = new Entity();
		entity->add(transform);
		entity->add(light);
		std::ostringstream lightID;
		lightID << "ENT_LIGHT" << platformID++;
		EntitiesManager::get().add(Hash(lightID.str().c_str()), entity);
	}

	void TestPanel::draw() const 
	{
		BasePanel::draw();
	}

	void TestPanel::createEntities()
	{
		const Texture* texture = Texture::load("pop.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Side::RIGHT);
		SpriteGroup* spriteGroup;
		AnimationCollection* animations = new AnimationCollection();
		Hash animationID = Hash::INVALID;
		Hash sceneNodeID = Hash("SCN_ROOT");

		#pragma region Player Spritesheet
		
		addAnimation(animations, animationID, sceneNodeID, animationID, 0.0f);
		// POP_ANM_JUMP_FORWARD
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_FORWARD");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(541.5, 232.5, 60, 58), Vector(4, -29)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 0.0f);
		// POP_ANM_STAND
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_STAND");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(16.5, 44.5, 14, 78), Vector(-1, -39)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 0.0f);
		// POP_ANM_TURN
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_TURN");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(46.5, 44.5, 14, 78), Vector(-1, -39)));
		spriteGroup->add(new Sprite(AABB(73.5, 44.5, 16, 78), Vector(1, -39)));
		spriteGroup->add(new Sprite(AABB(108.5, 44.5, 22, 78), Vector(4, -39)));
		spriteGroup->add(new Sprite(AABB(142.5, 44.5, 22, 78), Vector(2, -39)));
		spriteGroup->add(new Sprite(AABB(176.5, 44.5, 22, 78), Vector(1, -39)));
		spriteGroup->add(new Sprite(AABB(213.5, 44.5, 24, 78), Vector(4, -39)));
		spriteGroup->add(new Sprite(AABB(248.5, 44.5, 26, 78), Vector(1, -39)));
		spriteGroup->add(new Sprite(AABB(285.5, 44.5, 20, 78), Vector(-3, -39)));
		spriteGroup->add(new Sprite(AABB(320.5, 44.5, 22, 78), Vector(-1, -39)));
		spriteGroup->add(new Sprite(AABB(356.5, 44.5, 18, 78), Vector(-1, -39)));
		spriteGroup->add(new Sprite(AABB(387.5, 44.5, 12, 78), Vector(0, -39)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 363.0f);
		// POP_ANM_FALL
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_FALL");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(425.5, 46.5, 24, 90), Vector(4, -45)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 0.0f);
		// POP_ANM_WALK
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_WALK");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(15.5, 136.5, 20, 78), Vector(0, -39)));
		spriteGroup->add(new Sprite(AABB(52.5, 137.5, 30, 76), Vector(1, -38)));
		spriteGroup->add(new Sprite(AABB(107.5, 137.5, 44, 76), Vector(0, -38)));
		spriteGroup->add(new Sprite(AABB(161.5, 137.5, 40, 76), Vector(1, -38)));
		spriteGroup->add(new Sprite(AABB(212.5, 136.5, 34, 78), Vector(8, -39)));
		spriteGroup->add(new Sprite(AABB(260.5, 136.5, 22, 78), Vector(5, -39)));
		spriteGroup->add(new Sprite(AABB(302.5, 136.5, 18, 78), Vector(1, -39)));
		spriteGroup->add(new Sprite(AABB(344.5, 137.5, 34, 76), Vector(-2, -38)));
		spriteGroup->add(new Sprite(AABB(394.5, 137.5, 42, 76), Vector(-2, -38)));
		spriteGroup->add(new Sprite(AABB(445.5, 138.5, 40, 74), Vector(0, -37)));
		spriteGroup->add(new Sprite(AABB(493.5, 137.5, 32, 76), Vector(2, -38)));
		spriteGroup->add(new Sprite(AABB(536.5, 137.5, 22, 76), Vector(2, -38)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 396.0f);
		// POP_ANM_JUMP_FORWARD_START
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_FORWARD_START");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(11.5, 227.5, 16, 76), Vector(2, -38)));
		spriteGroup->add(new Sprite(AABB(49.5, 230.5, 20, 70), Vector(-6, -35)));
		spriteGroup->add(new Sprite(AABB(89.5, 232.5, 24, 66), Vector(-5, -33)));
		spriteGroup->add(new Sprite(AABB(129.5, 234.5, 32, 62), Vector(0, -31)));
		spriteGroup->add(new Sprite(AABB(177.5, 236.5, 32, 58), Vector(-8, -29)));
		spriteGroup->add(new Sprite(AABB(225.5, 235.5, 32, 60), Vector(-8, -30)));
		spriteGroup->add(new Sprite(AABB(277.5, 234.5, 44, 62), Vector(-15, -31)));
		spriteGroup->add(new Sprite(AABB(338.5, 234.5, 54, 66), Vector(-16, -33)));
		spriteGroup->add(new Sprite(AABB(402.5, 232.5, 54, 70), Vector(-22, -35)));
		spriteGroup->add(new Sprite(AABB(467.5, 231.5, 56, 64), Vector(-20, -32)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 330.0f);
		// POP_ANM_JUMP_UP_START
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_UP_START");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(20.5, 333.5, 22, 76), Vector(0, -38)));
		spriteGroup->add(new Sprite(AABB(57.5, 333.5, 24, 76), Vector(-2, -38)));
		spriteGroup->add(new Sprite(AABB(91.5, 335.5, 28, 72), Vector(-1, -36)));
		spriteGroup->add(new Sprite(AABB(131.5, 339.5, 32, 64), Vector(-4, -32)));
		spriteGroup->add(new Sprite(AABB(172.5, 342.5, 30, 58), Vector(3, -29)));
		spriteGroup->add(new Sprite(AABB(208.5, 344.5, 22, 54), Vector(4, -27)));
		spriteGroup->add(new Sprite(AABB(246.5, 346.5, 26, 50), Vector(4, -25)));
		spriteGroup->add(new Sprite(AABB(286.5, 348.5, 30, 46), Vector(3, -23)));
		spriteGroup->add(new Sprite(AABB(330.5, 344.5, 30, 54), Vector(4, -27)));
		spriteGroup->add(new Sprite(AABB(375.5, 349.5, 36, 44), Vector(7, -22)));
		spriteGroup->add(new Sprite(AABB(420.5, 347.5, 26, 48), Vector(2, -24)));
		spriteGroup->add(new Sprite(AABB(461.5, 342.5, 32, 58), Vector(-4, -29)));
		spriteGroup->add(new Sprite(AABB(502.5, 338.5, 30, 66), Vector(-3, -33)));
		spriteGroup->add(new Sprite(AABB(540.5, 328.5, 18, 86), Vector(2, -43)));
		spriteGroup->add(new Sprite(AABB(577.5, 326.5, 20, 90), Vector(0, -45)));
		spriteGroup->add(new Sprite(AABB(609.5, 326.5, 24, 90), Vector(-2, -45)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 528.0f);
		// POP_ANM_JUMP_UP
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_UP");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(643.5, 326.5, 24, 90), Vector(-1, -45)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 0.0f);
		// POP_ANM_CLIMB
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_CLIMB");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(19.5, 440.5, 16, 74), Vector(-4, -37)));
		spriteGroup->add(new Sprite(AABB(54.5, 443.5, 22, 68), Vector(-5, -34)));
		spriteGroup->add(new Sprite(AABB(88.5, 447.5, 26, 60), Vector(-1, -30)));
		spriteGroup->add(new Sprite(AABB(128.5, 449.5, 26, 56), Vector(-4, -28)));
		spriteGroup->add(new Sprite(AABB(169.5, 450.5, 28, 54), Vector(-2, -27)));
		spriteGroup->add(new Sprite(AABB(212.5, 451.5, 34, 52), Vector(0, -26)));
		spriteGroup->add(new Sprite(AABB(259.5, 455.5, 40, 48), Vector(0, -24)));
		spriteGroup->add(new Sprite(AABB(311.5, 457.5, 36, 52), Vector(1, -15)));
		spriteGroup->add(new Sprite(AABB(358.5, 457.5, 34, 52), Vector(-1, -10)));
		spriteGroup->add(new Sprite(AABB(404.5, 454.5, 34, 54), Vector(1, -1)));
		spriteGroup->add(new Sprite(AABB(450.5, 458.5, 30, 54), Vector(2, -1)));
		spriteGroup->add(new Sprite(AABB(496.5, 457.5, 30, 60), Vector(3, 7)));
		spriteGroup->add(new Sprite(AABB(541.5, 454.5, 32, 62), Vector(8, 4)));
		spriteGroup->add(new Sprite(AABB(586.5, 453.5, 38, 64), Vector(8, 6)));
		spriteGroup->add(new Sprite(AABB(635.5, 455.5, 36, 64), Vector(7, 10)));
		spriteGroup->add(new Sprite(AABB(681.5, 449.5, 36, 68), Vector(11, 12)));
		spriteGroup->add(new Sprite(AABB(724.5, 448.5, 34, 70), Vector(10, 14)));
		spriteGroup->add(new Sprite(AABB(768.5, 449.5, 26, 72), Vector(6, 17)));
		spriteGroup->add(new Sprite(AABB(807.5, 449.5, 24, 72), Vector(5, 20)));
		spriteGroup->add(new Sprite(AABB(843.5, 449.5, 24, 72), Vector(4, 22)));
		spriteGroup->add(new Sprite(AABB(878.5, 447.5, 22, 72), Vector(7, 30)));
		spriteGroup->add(new Sprite(AABB(911.5, 445.5, 20, 76), Vector(6, 37)));
		spriteGroup->add(new Sprite(AABB(947.5, 441.5, 20, 80), Vector(10, 39)));
		spriteGroup->add(new Sprite(AABB(983.5, 439.5, 20, 92), Vector(9, 45)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 792.0f);
		// POP_ANM_HANG
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_HANG");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(1018.5, 438.5, 14, 94), Vector(1, 46)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 0.0f);
		// POP_ANM_DROP
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_DROP");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(26.5, 544.5, 26, 70), Vector(6, 34)));
		spriteGroup->add(new Sprite(AABB(78.5, 546.5, 30, 78), Vector(7, 38)));
		spriteGroup->add(new Sprite(AABB(127.5, 546.5, 24, 94), Vector(3, 46)));
		spriteGroup->add(new Sprite(AABB(166.5, 547.5, 18, 92), Vector(-1, 45)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 132.0f);
		// POP_ANM_JUMP_FORWARD_END
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_FORWARD_END");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(614.5, 236.5, 58, 62), Vector(23, -31)));
		spriteGroup->add(new Sprite(AABB(679.5, 238.5, 48, 58), Vector(20, -29)));
		spriteGroup->add(new Sprite(AABB(736.5, 239.5, 30, 56), Vector(11, -28)));
		spriteGroup->add(new Sprite(AABB(779.5, 241.5, 28, 52), Vector(10, -26)));
		spriteGroup->add(new Sprite(AABB(823.5, 242.5, 32, 50), Vector(7, -25)));
		spriteGroup->add(new Sprite(AABB(867.5, 238.5, 28, 58), Vector(0, -29)));
		spriteGroup->add(new Sprite(AABB(906.5, 234.5, 22, 66), Vector(-2, -33)));
		spriteGroup->add(new Sprite(AABB(943.5, 230.5, 16, 74), Vector(-1, -37)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 264.0f);

		#pragma endregion

		//createSkeleton();
		createPlayer(spritesheet, animations);
		createLaser();
		//createSentry(spritesheet);
		//createCamera();
		createPatrol(spritesheet, animations);
		//createChaser(spritesheet, animations);
		createPlatforms();
		createBackground(Point(512.0f, 384.0f));
		createBackground(Point(1536.0f, 384.0f));
		//createLight(Point(500.0f, 300.0f));
		//createLight(Point(1500.0f, 300.0f));
	}
}
