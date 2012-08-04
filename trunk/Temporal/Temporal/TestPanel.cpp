#include "TestPanel.h"
#include "NumericPair.h"
#include "Shapes.h"
#include "Math.h"
#include "EntitySystem.h"
#include "Position.h"
#include "Orientation.h"
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
#include "MessageParams.h"
#include "Lighting.h"
#include "Particles.h"
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
		Sensor* sensor(new Sensor(Hash("SNS_JUMP"), AABB(sensorOffset, sensorSize), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS));
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
		Sensor* sensor = new Sensor(Hash("SNS_HANG"), AABB(sensorOffset, sensorSize), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS);
		entity.add(sensor);
	}

	void addBackEdgeSensor(Entity& entity)
	{
		// Back Edge Sensor
		float sensorOffsetX = -(EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Sensor* sensor = new Sensor(Hash("SNS_BACK_EDGE"), AABB(sensorOffset, EDGE_SENSOR_SIZE), -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS);
		entity.add(sensor);
	}

	void addFrontEdgeSensor(Entity& entity)
	{
		// Front edge sensor
		float sensorOffsetX = (EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Sensor* sensor = new Sensor(Hash("SNS_FRONT_EDGE"), AABB(sensorOffset, EDGE_SENSOR_SIZE), -ANGLE_135_IN_RADIANS - ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS);
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
		Animation* animation = const_cast<Animation*>((*animations)[animationID]);
		if(animation == NULL)
		{
			animation = new Animation();
			(*animations)[animationID] = animation;
		}
		animation->add(sceneNodeID, spriteGroupID, duration, translation, rotation);
	}

	void createTemporalEcho(Entity* entity, AnimationCollection* animations = NULL)
	{
		const Component* position = entity->get(ComponentType::POSITION);
		const Component* orientation = entity->get(ComponentType::ORIENTATION);
		const Component* drawPosition = entity->get(ComponentType::DRAW_POSITION);
		const Component* renderer = entity->get(ComponentType::RENDERER);
		
		Entity* echoEntity = new Entity();
		if(position != NULL)
			echoEntity->add(position->clone());
		if(orientation != NULL)
			echoEntity->add(orientation->clone());
		if(drawPosition != NULL)
			echoEntity->add(drawPosition->clone());
		if(renderer != NULL)
		{
			Component* rendererClone = renderer->clone();
			if(animations != NULL)
			{
				Animator* animator = new Animator(*animations, static_cast<Renderer*>(rendererClone)->getRoot());
				echoEntity->add(animator);
			}
			echoEntity->add(rendererClone);
		}

		TemporalEcho* temporalEcho = new TemporalEcho(echoEntity);
		entity->add(temporalEcho);
	}

	void createPlayer(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		Position* position = new Position(Point(512.0f, 768.0f));
		Orientation* orientation = new Orientation(Side::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Point(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		InputController* controller = new InputController();
		DynamicBody* dynamicBody = new DynamicBody(Size(ENTITY_SIZE.getWidth(), ENTITY_SIZE.getHeight()));
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::Present, true);
		LightGem* lightGem = new LightGem();
		const Texture* texture = Texture::load("bubble.png");
		ParticleEmitter* particleEmitter = new ParticleEmitter(texture, 3000.0f, 2);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
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
		Position* position = new Position(Point(100.0f, 550.0f));
		Orientation* orientation = new Orientation(Side::RIGHT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Sentry* sentry = new Sentry();
		Sight* sight = new Sight(ANGLE_0_IN_RADIANS, ANGLE_60_IN_RADIANS);
		SceneNode* root = createDefaultSceneGraph();
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::Past);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
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

		Position* position = new Position(Point(383.0f, 383.0f));
		Orientation* orientation = new Orientation(Side::LEFT);
		Camera* camera = new Camera();
		Sight* sight = new Sight(-ANGLE_30_IN_RADIANS, ANGLE_30_IN_RADIANS);
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::Present);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
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
		Position* position = new Position(Point(512.0f, 768.0f));
		Orientation* orientation = new Orientation(Side::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Point(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Patrol* patrol = new Patrol();
		DynamicBody* dynamicBody = new DynamicBody(Size(ENTITY_SIZE.getWidth(), ENTITY_SIZE.getHeight()));
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);
		Sight* sight = new Sight(ANGLE_0_IN_RADIANS, ANGLE_60_IN_RADIANS);
		Light* light = new Light(Color::White, 500.0f, ANGLE_0_IN_RADIANS, ANGLE_30_IN_RADIANS); 
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::Past);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(drawPosition);
		entity->add(patrol);
		entity->add(dynamicBody);
		entity->add(sight);
		addFrontEdgeSensor(*entity);
		entity->add(actionController);
		entity->add(light);
		entity->add(animator);
		entity->add(renderer);
		//entity->add(temporalPeriod);
		//createTemporalEcho(entity, animations);
		EntitiesManager::get().add(Hash("ENT_PATROL"), entity);
	}

	void createChaser(SpriteSheet* spritesheet, AnimationCollection* animations)
	{
		Position* position = new Position(Point(512.0f, 768.0f));
		Orientation* orientation = new Orientation(Side::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Navigator* navigator = new Navigator();
		DynamicBody* dynamicBody = new DynamicBody(Size(ENTITY_SIZE.getWidth(), ENTITY_SIZE.getHeight()));
		ActionController* actionController = new ActionController();
		SceneNode* root = createDefaultSceneGraph();
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC, root);
		TemporalPeriod* temporalPeriod = new TemporalPeriod(Period::Present);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
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
	void createPlatform(Shape* shape, SpriteSheet* spritesheet, bool cover = false) 
	{
		//Position* position = new Position(Point(shape->getCenterX(), shape->getCenterY()));
		StaticBody* staticBody = new StaticBody(shape, cover);

		Entity* entity = new Entity();
		//entity->add(position);
		entity->add(staticBody);
		Grid::get().add(staticBody);
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
		createPlatform(new AABB(AABBLB(640.0f, 384.0f, 256.0f, 64.0f)), spritesheet, true);

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

	void createBackground()
	{
		const Texture* texture = Texture::load("bg.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Side::NONE);
		SpriteGroup* spriteGroup = new SpriteGroup();
		const Hash& spriteGroupID = Hash("ANM_DEFAULT");
		spritesheet->add(spriteGroupID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(texture->getSize() / 2.0f, texture->getSize()), Vector::Zero));
		SceneNode* root = createDefaultSceneGraph();
		root->setSpriteGroupID(spriteGroupID);
		Position* position = new Position(texture->getSize() / 2.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::BACKGROUND, root);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(renderer);
		EntitiesManager::get().add(Hash("ENT_BACKGROUND"), entity);
	}

	void createLaser()
	{
		Position* position = new Position(Point(100.0f, 100.0f));
		Laser* laser = new Laser(Hash("ENT_PLATFORM23"));
		LineRenderer* renderer = new LineRenderer(VisualLayer::NPC);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(laser);
		entity->add(renderer);

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

		Position* position = new Position(Point(300.0f, 300.0f));
		Orientation* orientation = new Orientation(Side::RIGHT);
		Animator* animator = new Animator(*animations, *root);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC, root);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(animator);
		entity->add(renderer);
		//createTemporalEcho(entity, animations);
		entity->handleMessage(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animationID, true, true)));
		EntitiesManager::get().add(Hash("ENT_SKELETON"), entity);
	}

	void createLight(const Point& point)
	{
		Position* position = new Position(point);
		Light* light = new Light(Color(1.0f, 1.0f, 1.0f), 300.0f);
		Entity* entity = new Entity();
		entity->add(position);
		entity->add(light);
		std::ostringstream lightID;
		lightID << "ENT_LIGHT" << platformID++;
		EntitiesManager::get().add(Hash(lightID.str().c_str()), entity);
	}

	void TestPanel::createEntities()
	{
		const Texture* texture = Texture::load("pop.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Side::LEFT);
		SpriteGroup* spriteGroup;
		AnimationCollection* animations = new AnimationCollection();
		Hash animationID = Hash::INVALID;
		Hash sceneNodeID = Hash("SCN_ROOT");

		#pragma region Player Spritesheet
		// Stand - 0
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_STAND");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(26.5, 48, 21, 84), Vector(6, -42)));
		addAnimation(animations, animationID, sceneNodeID, animationID);
		// Turn - 1
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_TURN");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(60.5, 50, 23, 80), Vector(7, -40)));
		spriteGroup->add(new Sprite(AABB(95.5, 50, 25, 80), Vector(8, -40)));
		spriteGroup->add(new Sprite(AABB(134.5, 48, 29, 84), Vector(4, -42)));
		spriteGroup->add(new Sprite(AABB(177, 49.5, 38, 81), Vector(5, -41)));
		spriteGroup->add(new Sprite(AABB(224.5, 50, 43, 80), Vector(-4, -40)));
		spriteGroup->add(new Sprite(AABB(276, 52, 36, 76), Vector(-10, -38)));
		spriteGroup->add(new Sprite(AABB(316.5, 51, 31, 78), Vector(-8, -39)));
		spriteGroup->add(new Sprite(AABB(359, 51.5, 28, 79), Vector(-7, -40)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 530.0f);
		// Drop - 2
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_DROP");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(418.5, 58.5, 23, 109), Vector(-3, 54)));
		spriteGroup->add(new Sprite(AABB(462.5, 60, 21, 96), Vector(0, 48)));
		spriteGroup->add(new Sprite(AABB(509, 65, 22, 86), Vector(3, 43)));
		spriteGroup->add(new Sprite(AABB(546.5, 69.5, 25, 77), Vector(7, 38)));
		spriteGroup->add(new Sprite(AABB(584.5, 67.5, 25, 81), Vector(9, 40)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 330.0f);
		// FallStart - 3
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_FALL_START");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(667.5, 71, 71, 76), Vector(3, -38)));
		spriteGroup->add(new Sprite(AABB(737, 68.5, 62, 75), Vector(3, -38)));
		spriteGroup->add(new Sprite(AABB(805.5, 67, 51, 72), Vector(10, -36)));
		spriteGroup->add(new Sprite(AABB(866, 62, 54, 80), Vector(7, -40)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 260.0f);
		// Fall - 4
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_FALL");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(927, 63, 34, 78), Vector(-2, -39)));
		addAnimation(animations, animationID, sceneNodeID, animationID);
		// JumpUpStart - 5
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_UP_START");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(30, 193.5, 22, 83), Vector(2, -42)));
		spriteGroup->add(new Sprite(AABB(72.5, 193, 23, 84), Vector(2, -42)));
		spriteGroup->add(new Sprite(AABB(114, 194, 22, 82), Vector(2, -41)));
		spriteGroup->add(new Sprite(AABB(158, 195, 28, 80), Vector(-1, -40)));
		spriteGroup->add(new Sprite(AABB(202, 195, 36, 80), Vector(-5, -40)));
		spriteGroup->add(new Sprite(AABB(245.5, 194.5, 43, 81), Vector(-4, -41)));
		spriteGroup->add(new Sprite(AABB(296, 194.5, 48, 81), Vector(-7, -41)));
		spriteGroup->add(new Sprite(AABB(349.5, 196, 43, 78), Vector(1, -39)));
		spriteGroup->add(new Sprite(AABB(396, 198.5, 32, 73), Vector(7, -37)));
		spriteGroup->add(new Sprite(AABB(438, 198.5, 32, 73), Vector(7, -37)));
		spriteGroup->add(new Sprite(AABB(486, 197.5, 42, 71), Vector(13, -36)));
		spriteGroup->add(new Sprite(AABB(536, 195.5, 34, 75), Vector(12, -38)));
		spriteGroup->add(new Sprite(AABB(582, 187.5, 32, 95), Vector(11, -48)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 860.0f);
		// JumpUp - 6
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_UP");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(630, 189.5, 32, 103), Vector(10, -52)));
		addAnimation(animations, animationID, sceneNodeID, animationID);
		// Hang - 7
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_HANG");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(717.5, 190.5, 23, 111), Vector(-1, 55)));
		addAnimation(animations, animationID, sceneNodeID, animationID);
		// Climb - 8
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_CLIMB");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(24.5, 342, 21, 106), Vector(-7, 49)));
		spriteGroup->add(new Sprite(AABB(67, 348, 24, 88), Vector(-9, 40)));
		spriteGroup->add(new Sprite(AABB(109, 348.5, 26, 89), Vector(-5, 28)));
		spriteGroup->add(new Sprite(AABB(150.5, 346.5, 33, 91), Vector(-4, 20)));
		spriteGroup->add(new Sprite(AABB(200.5, 348.5, 45, 81), Vector(5, 13)));
		spriteGroup->add(new Sprite(AABB(258, 349, 48, 70), Vector(7, 10)));
		spriteGroup->add(new Sprite(AABB(319, 349.5, 46, 59), Vector(7, 0)));
		spriteGroup->add(new Sprite(AABB(385.5, 346, 61, 46), Vector(16, -9)));
		spriteGroup->add(new Sprite(AABB(459.5, 347.5, 59, 41), Vector(2, -10)));
		spriteGroup->add(new Sprite(AABB(528, 346.5, 62, 51), Vector(5, -14)));
		spriteGroup->add(new Sprite(AABB(600, 344.5, 56, 45), Vector(14, -23)));
		spriteGroup->add(new Sprite(AABB(660, 344.5, 52, 47), Vector(18, -24)));
		spriteGroup->add(new Sprite(AABB(727.5, 338.5, 53, 57), Vector(11, -29)));
		spriteGroup->add(new Sprite(AABB(787, 332.5, 52, 63), Vector(17, -32)));
		spriteGroup->add(new Sprite(AABB(843, 330, 50, 68), Vector(20, -34)));
		spriteGroup->add(new Sprite(AABB(896, 324, 30, 78), Vector(4, -39)));
		spriteGroup->add(new Sprite(AABB(937, 324.5, 22, 79), Vector(3, -40)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 1120.0f);
		// JumpForwardStart - 9
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_FORWARD_START");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(20.5, 502, 25, 84), Vector(7, -42)));
		spriteGroup->add(new Sprite(AABB(59.5, 503, 27, 82), Vector(8, -41)));
		spriteGroup->add(new Sprite(AABB(106, 503, 42, 80), Vector(6, -40)));
		spriteGroup->add(new Sprite(AABB(159.5, 508, 47, 68), Vector(12, -34)));
		spriteGroup->add(new Sprite(AABB(225, 509, 56, 64), Vector(15, -32)));
		spriteGroup->add(new Sprite(AABB(298.5, 507.5, 55, 65), Vector(20, -33)));
		spriteGroup->add(new Sprite(AABB(369, 505, 54, 68), Vector(22, -34)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 460.0f);
		// JumpForward - 10
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_FORWARD");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(449.5, 503, 79, 70), Vector(2, -35)));
		spriteGroup->add(new Sprite(AABB(557.5, 501, 99, 70), Vector(1, -35)));
		spriteGroup->add(new Sprite(AABB(678.5, 507.5, 103, 61), Vector(-3, -31)));
		spriteGroup->add(new Sprite(AABB(788, 506.5, 84, 55), Vector(-2, -28)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 260.0f);
		// JumpForwardEnd - 11
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_JUMP_FORWARD_END");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(52, 620.5, 58, 55), Vector(-8, -28)));
		spriteGroup->add(new Sprite(AABB(114.5, 621.5, 41, 51), Vector(-4, -26)));
		spriteGroup->add(new Sprite(AABB(189.5, 618, 55, 60), Vector(3, -30)));
		spriteGroup->add(new Sprite(AABB(248, 613, 44, 70), Vector(-8, -35)));
		spriteGroup->add(new Sprite(AABB(306.5, 609, 35, 80), Vector(-2, -40)));
		spriteGroup->add(new Sprite(AABB(352, 608, 30, 82), Vector(0, -41)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 400.0f);
		// Walk - 12
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_WALK");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(34, 735.5, 24, 81), Vector(8, -41)));
		spriteGroup->add(new Sprite(AABB(76, 735, 22, 80), Vector(6, -40)));
		spriteGroup->add(new Sprite(AABB(117.5, 736.5, 27, 77), Vector(6, -39)));
		spriteGroup->add(new Sprite(AABB(164.5, 734.5, 41, 77), Vector(0, -39)));
		spriteGroup->add(new Sprite(AABB(226, 734.5, 54, 75), Vector(5, -38)));
		spriteGroup->add(new Sprite(AABB(290, 734, 58, 76), Vector(5, -38)));
		spriteGroup->add(new Sprite(AABB(358.5, 734, 57, 78), Vector(3, -39)));
		spriteGroup->add(new Sprite(AABB(424.5, 735, 53, 76), Vector(0, -38)));
		spriteGroup->add(new Sprite(AABB(487, 734, 50, 78), Vector(-6, -39)));
		spriteGroup->add(new Sprite(AABB(543.5, 733, 41, 80), Vector(-3, -40)));
		spriteGroup->add(new Sprite(AABB(593, 732.5, 26, 81), Vector(0, -41)));
		spriteGroup->add(new Sprite(AABB(634.5, 732, 21, 82), Vector(5, -41)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 790.0f);
		// HangingForward - 13
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_SWING_FORWARD");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(32.5, 859.5, 29, 111), Vector(1, 55)));
		spriteGroup->add(new Sprite(AABB(91.5, 859.5, 33, 109), Vector(6, 54)));
		spriteGroup->add(new Sprite(AABB(152.5, 863.5, 41, 107), Vector(11, 53)));
		spriteGroup->add(new Sprite(AABB(206, 861.5, 40, 103), Vector(9, 51)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 260.0f);
		// HangingBackwards - 14
		spriteGroup = new SpriteGroup();
		animationID = Hash("POP_ANM_SWING_BACKWARD");
		spritesheet->add(animationID, spriteGroup);
		spriteGroup->add(new Sprite(AABB(252.5, 864.5, 23, 109), Vector(-6, 54)));
		spriteGroup->add(new Sprite(AABB(295.5, 864, 35, 108), Vector(-11, 54)));
		spriteGroup->add(new Sprite(AABB(348.5, 863, 45, 106), Vector(-16, 53)));
		spriteGroup->add(new Sprite(AABB(408, 860.5, 52, 101), Vector(-22, 50)));
		spriteGroup->add(new Sprite(AABB(475.5, 860, 55, 102), Vector(-22, 51)));
		spriteGroup->add(new Sprite(AABB(539, 859.5, 56, 101), Vector(-23, 50)));
		spriteGroup->add(new Sprite(AABB(611, 858.5, 56, 99), Vector(-22, 49)));
		addAnimation(animations, animationID, sceneNodeID, animationID, 460.0f);

#pragma endregion

		createSkeleton();
		createPlayer(spritesheet, animations);
		//createLaser();
		//createSentry(spritesheet);
		//createCamera();
		//createPatrol(spritesheet, animations);
		//createChaser(spritesheet, animations);
		createPlatforms();
		createBackground();
		//createLight(Point(500.0f, 300.0f));
		//createLight(Point(1500.0f, 300.0f));
	}
}
