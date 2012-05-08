#include "TestPanel.h"

#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Base\Math.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\Component.h>
#include <Temporal\Game\Entity.h>
#include <Temporal\Game\Position.h>
#include <Temporal\Game\EntityOrientation.h>
#include <Temporal\Game\DrawPosition.h>
#include <Temporal\Game\ActionController.h>
#include <Temporal\Game\EntitiesManager.h>
#include <Temporal\Game\MovementUtils.h>
#include <Temporal\Physics\StaticBody.h>
#include <Temporal\Physics\DynamicBody.h>
#include <Temporal\Physics\Grid.h>
#include <Temporal\Physics\Sensor.h>
#include <Temporal\Physics\Sight.h>
#include <Temporal\Input\InputController.h>
#include <Temporal\Graphics\SpriteSheet.h>
#include <Temporal\Graphics\Texture.h>
#include <Temporal\Graphics\Graphics.h>
#include <Temporal\Graphics\Animator.h>
#include <Temporal\Graphics\Renderer.h>
#include <Temporal\AI\Navigator.h>
#include <Temporal\AI\Sentry.h>
#include <Temporal\AI\Patrol.h>
#include <Temporal\AI\Camera.h>
#include <sstream>

namespace Temporal
{
	#pragma region Helper Methods
	class AreaRenderer : public Component
	{
	public: 
		AreaRenderer(const SpriteSheet& spritesheet, VisualLayer::Enum layer, int spriteGroupID = 0, int spriteID = 0) : _spritesheet(spritesheet), _layer(layer), _spriteGroupID(spriteGroupID), _spriteID(spriteID) {};

		ComponentType::Enum getType(void) const { return ComponentType::OTHER; }

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::DRAW)
			{
				VisualLayer::Enum layer = *(VisualLayer::Enum*)message.getParam();
				if(_layer == layer)
				{
					Shape* bounds = (Shape*)sendMessageToOwner(Message(MessageID::GET_SHAPE));
					const Sprite& sprite = _spritesheet.get(_spriteGroupID).get(_spriteID);
					const Texture& texture = _spritesheet.getTexture();
					float textureWidth = texture.getSize().getWidth();
					float textureHeight = texture.getSize().getWidth();
					for(float x = bounds->getLeft(); x <= bounds->getRight(); x += textureWidth)
					{
						if(bounds->getWidth() < textureWidth)
							x = bounds->getCenterX();
						for(float y = bounds->getBottom(); y <= bounds->getTop(); y += textureHeight)
						{
							if(bounds->getHeight() < textureHeight)
								y = bounds->getCenterY();
							Graphics::get().draw(_spritesheet.getTexture(), sprite.getBounds(), Point(x, y), false);
						}
					}
				}
			}
		}

		const SpriteSheet& _spritesheet;

		int _spriteGroupID;
		int _spriteID;
		VisualLayer::Enum _layer;
	};

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
		Sensor* sensor(new Sensor(Hash("SI_JUMP"), sensorOffset, sensorSize, -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS));
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
		Sensor* sensor = new Sensor(Hash("SI_HANG"), sensorOffset, sensorSize, -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS);
		entity.add(sensor);
	}

	void addBackEdgeSensor(Entity& entity)
	{
		// Back Edge Sensor
		float sensorOffsetX = -(EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Sensor* sensor = new Sensor(Hash("SI_BACK_EDGE"), sensorOffset, EDGE_SENSOR_SIZE, -ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS);
		entity.add(sensor);
	}

	void addFrontEdgeSensor(Entity& entity)
	{
		// Front edge sensor
		float sensorOffsetX = (EDGE_SENSOR_SIZE.getWidth() - ENTITY_SIZE.getWidth()) / 2.0f;
		float sensorOffsetY = -(ENTITY_SIZE.getHeight() / 2.0f) -(EDGE_SENSOR_SIZE.getHeight() /2.0f);
		Vector sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		Sensor* sensor = new Sensor(Hash("SI_FRONT_EDGE"), sensorOffset, EDGE_SENSOR_SIZE, -ANGLE_135_IN_RADIANS - ANGLE_45_IN_RADIANS / 2.0f, ANGLE_135_IN_RADIANS);
		entity.add(sensor);
	}

	void addSensors(Entity& entity)
	{
		addJumpSensor(entity);
		addHangSensor(entity);
		addBackEdgeSensor(entity);
		addFrontEdgeSensor(entity);
	}

	#pragma endregion
	#pragma region Creation Methods

	void createPlayer(SpriteSheet* spritesheet)
	{
		Position* position = new Position(Point(512.0f, 768.0f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Point(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		InputController* controller = new InputController();
		DynamicBody* dynamicBody = new DynamicBody(Size(ENTITY_SIZE.getWidth(), ENTITY_SIZE.getHeight()));
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(66.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC);

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
		EntitiesManager::get().add(Hash("EI_PLAYER"), entity);
	}

	void createChaser(SpriteSheet* spritesheet)
	{
		Position* position = new Position(Point(512.0f, 768.0f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Navigator* navigator = new Navigator();
		DynamicBody* dynamicBody = new DynamicBody(Size(ENTITY_SIZE.getWidth(), ENTITY_SIZE.getHeight()));
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(66.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC);

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
		EntitiesManager::get().add(Hash("EI_CHASER"), entity);
	}

	void createSentry(SpriteSheet* spritesheet)
	{
		Position* position = new Position(Point(100.0f, 550.0f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::RIGHT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Sentry* sentry = new Sentry();
		Sight* sight = new Sight(ANGLE_0_IN_RADIANS, ANGLE_60_IN_RADIANS, Hash("EI_PLAYER"));
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(drawPosition);
		entity->add(sentry);
		entity->add(sight);
		entity->add(renderer);
		EntitiesManager::get().add(Hash("EI_SENTRY"), entity);
	}

	void createPatrol(SpriteSheet* spritesheet)
	{
		Position* position = new Position(Point(512.0f, 768.0f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Point(0.0f, -(ENTITY_SIZE.getHeight() - 1.0f) / 2.0f));
		Patrol* patrol = new Patrol();
		DynamicBody* dynamicBody = new DynamicBody(Size(ENTITY_SIZE.getWidth(), ENTITY_SIZE.getHeight()));
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(66.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC);
		Sight* sight = new Sight(ANGLE_0_IN_RADIANS, ANGLE_60_IN_RADIANS, Hash("EI_PLAYER"));

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(drawPosition);
		entity->add(patrol);
		entity->add(dynamicBody);
		entity->add(sight);
		addFrontEdgeSensor(*entity);
		entity->add(actionController);
		entity->add(animator);
		entity->add(renderer);
		EntitiesManager::get().add(Hash("EI_PATROL"), entity);
	}

	void createCamera()
	{
		Position* position = new Position(Point(383.0f, 383.0f));
		const Texture* texture = Texture::load("c:\\stuff\\camera.png");
		Camera* camera = new Camera();
		Sight* sight = new Sight(-ANGLE_30_IN_RADIANS, ANGLE_30_IN_RADIANS, Hash("EI_PLAYER"));
		SpriteSheet* spritesheet = new SpriteSheet(texture, Orientation::LEFT);
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		SpriteGroup* animation;

		#pragma region Camera Animation
		// Search - 0
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(19, 19, 24, 32), Vector(4, 16)));
		// See - 1
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(19, 59, 24, 32), Vector(4, 16)));
		// Turn - 2
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(50.5, 19.5, 17, 33), Vector(2, 16)));
		animation->add(new Sprite(AABB(76, 19.5, 12, 33), Vector(1, 16)));
		animation->add(new Sprite(AABB(98, 19.5, 12, 33), Vector(0, 16)));
		#pragma endregion

		Animator* animator = new Animator(66.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(camera);
		entity->add(sight);
		entity->add(animator);
		entity->add(renderer);
		EntitiesManager::get().add(Hash("EI_CAMERA"), entity);
	}

	static int platformID = 0;
	void createPlatform(Shape* shape, SpriteSheet* spritesheet, bool cover = false) 
	{
		Position* position = new Position(Point(shape->getCenterX(), shape->getCenterY()));
		StaticBody* staticBody = new StaticBody(shape, cover);
		AreaRenderer* renderer(new AreaRenderer(*spritesheet, cover ? VisualLayer::COVER : VisualLayer::STATIC));
		Entity* entity = new Entity();
		entity->add(position);
		entity->add(staticBody);
		//entity->add(renderer);
		Grid::get().add(staticBody);
		std::ostringstream id;
		id << "EI_PLATFORM" << platformID++;
		EntitiesManager::get().add(Hash(id.str().c_str()), entity);
	}

	void createPlatforms()
	{
		const Texture* texture = Texture::load("c:\\stuff\\tile.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Orientation::NONE);
		SpriteGroup* animation = new SpriteGroup();
		spritesheet->add(animation);
		const Size TILE_SIZE(32.0f, 32.0f);
		animation->add(new Sprite(AABB(TILE_SIZE / 2.0f, TILE_SIZE), Vector::Zero));

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
	}

	void createBackground()
	{
		const Texture* texture = Texture::load("c:\\stuff\\bg.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Orientation::NONE);
		SpriteGroup* animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(texture->getSize() / 2.0f, texture->getSize()), Vector::Zero));

		Position* position = new Position(texture->getSize() / 2.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::BACKGROUND);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(renderer);
		EntitiesManager::get().add(Hash("EI_BACKGROUND"), entity);
	}
	#pragma endregion

	void TestPanel::createEntities(void)
	{
		const Texture* texture = Texture::load("c:\\stuff\\pop.png");
		SpriteSheet* spritesheet(new SpriteSheet(texture, Orientation::LEFT));
		SpriteGroup* animation;

		#pragma region Player Animations
		// Stand - 0
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(26.5, 48, 21, 84), Vector(6, -42)));
		// Turn - 1
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(60.5, 50, 23, 80), Vector(7, -40)));
		animation->add(new Sprite(AABB(95.5, 50, 25, 80), Vector(8, -40)));
		animation->add(new Sprite(AABB(134.5, 48, 29, 84), Vector(4, -42)));
		animation->add(new Sprite(AABB(177, 49.5, 38, 81), Vector(5, -41)));
		animation->add(new Sprite(AABB(224.5, 50, 43, 80), Vector(-4, -40)));
		animation->add(new Sprite(AABB(276, 52, 36, 76), Vector(-10, -38)));
		animation->add(new Sprite(AABB(316.5, 51, 31, 78), Vector(-8, -39)));
		animation->add(new Sprite(AABB(359, 51.5, 28, 79), Vector(-7, -40)));
		// Drop - 2
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(418.5, 58.5, 23, 109), Vector(-3, 54)));
		animation->add(new Sprite(AABB(462.5, 60, 21, 96), Vector(0, 48)));
		animation->add(new Sprite(AABB(509, 65, 22, 86), Vector(3, 43)));
		animation->add(new Sprite(AABB(546.5, 69.5, 25, 77), Vector(7, 38)));
		animation->add(new Sprite(AABB(584.5, 67.5, 25, 81), Vector(9, 40)));
		// FallStart - 3
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(667.5, 71, 71, 76), Vector(3, -38)));
		animation->add(new Sprite(AABB(737, 68.5, 62, 75), Vector(3, -38)));
		animation->add(new Sprite(AABB(805.5, 67, 51, 72), Vector(10, -36)));
		animation->add(new Sprite(AABB(866, 62, 54, 80), Vector(7, -40)));
		// Fall - 4
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(927, 63, 34, 78), Vector(-2, -39)));
		// JumpUpStart - 5
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(30, 193.5, 22, 83), Vector(2, -42)));
		animation->add(new Sprite(AABB(72.5, 193, 23, 84), Vector(2, -42)));
		animation->add(new Sprite(AABB(114, 194, 22, 82), Vector(2, -41)));
		animation->add(new Sprite(AABB(158, 195, 28, 80), Vector(-1, -40)));
		animation->add(new Sprite(AABB(202, 195, 36, 80), Vector(-5, -40)));
		animation->add(new Sprite(AABB(245.5, 194.5, 43, 81), Vector(-4, -41)));
		animation->add(new Sprite(AABB(296, 194.5, 48, 81), Vector(-7, -41)));
		animation->add(new Sprite(AABB(349.5, 196, 43, 78), Vector(1, -39)));
		animation->add(new Sprite(AABB(396, 198.5, 32, 73), Vector(7, -37)));
		animation->add(new Sprite(AABB(438, 198.5, 32, 73), Vector(7, -37)));
		animation->add(new Sprite(AABB(486, 197.5, 42, 71), Vector(13, -36)));
		animation->add(new Sprite(AABB(536, 195.5, 34, 75), Vector(12, -38)));
		animation->add(new Sprite(AABB(582, 187.5, 32, 95), Vector(11, -48)));
		// JumpUp - 6
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(630, 189.5, 32, 103), Vector(10, -52)));
		// Hang - 7
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(717.5, 190.5, 23, 111), Vector(-1, 55)));
		// Climb - 8
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(24.5, 342, 21, 106), Vector(-7, 49)));
		animation->add(new Sprite(AABB(67, 348, 24, 88), Vector(-9, 40)));
		animation->add(new Sprite(AABB(109, 348.5, 26, 89), Vector(-5, 28)));
		animation->add(new Sprite(AABB(150.5, 346.5, 33, 91), Vector(-4, 20)));
		animation->add(new Sprite(AABB(200.5, 348.5, 45, 81), Vector(5, 13)));
		animation->add(new Sprite(AABB(258, 349, 48, 70), Vector(7, 10)));
		animation->add(new Sprite(AABB(319, 349.5, 46, 59), Vector(7, 0)));
		animation->add(new Sprite(AABB(385.5, 346, 61, 46), Vector(16, -9)));
		animation->add(new Sprite(AABB(459.5, 347.5, 59, 41), Vector(2, -10)));
		animation->add(new Sprite(AABB(528, 346.5, 62, 51), Vector(5, -14)));
		animation->add(new Sprite(AABB(600, 344.5, 56, 45), Vector(14, -23)));
		animation->add(new Sprite(AABB(660, 344.5, 52, 47), Vector(18, -24)));
		animation->add(new Sprite(AABB(727.5, 338.5, 53, 57), Vector(11, -29)));
		animation->add(new Sprite(AABB(787, 332.5, 52, 63), Vector(17, -32)));
		animation->add(new Sprite(AABB(843, 330, 50, 68), Vector(20, -34)));
		animation->add(new Sprite(AABB(896, 324, 30, 78), Vector(4, -39)));
		animation->add(new Sprite(AABB(937, 324.5, 22, 79), Vector(3, -40)));
		// JumpForwardStart - 9
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(20.5, 502, 25, 84), Vector(7, -42)));
		animation->add(new Sprite(AABB(59.5, 503, 27, 82), Vector(8, -41)));
		animation->add(new Sprite(AABB(106, 503, 42, 80), Vector(6, -40)));
		animation->add(new Sprite(AABB(159.5, 508, 47, 68), Vector(12, -34)));
		animation->add(new Sprite(AABB(225, 509, 56, 64), Vector(15, -32)));
		animation->add(new Sprite(AABB(298.5, 507.5, 55, 65), Vector(20, -33)));
		animation->add(new Sprite(AABB(369, 505, 54, 68), Vector(22, -34)));
		// JumpForward - 10
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(449.5, 503, 79, 70), Vector(2, -35)));
		animation->add(new Sprite(AABB(557.5, 501, 99, 70), Vector(1, -35)));
		animation->add(new Sprite(AABB(678.5, 507.5, 103, 61), Vector(-3, -31)));
		animation->add(new Sprite(AABB(788, 506.5, 84, 55), Vector(-2, -28)));
		// JumpForwardEnd - 11
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(52, 620.5, 58, 55), Vector(-8, -28)));
		animation->add(new Sprite(AABB(114.5, 621.5, 41, 51), Vector(-4, -26)));
		animation->add(new Sprite(AABB(189.5, 618, 55, 60), Vector(3, -30)));
		animation->add(new Sprite(AABB(248, 613, 44, 70), Vector(-8, -35)));
		animation->add(new Sprite(AABB(306.5, 609, 35, 80), Vector(-2, -40)));
		animation->add(new Sprite(AABB(352, 608, 30, 82), Vector(0, -41)));
		// Walk - 12
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(34, 735.5, 24, 81), Vector(8, -41)));
		animation->add(new Sprite(AABB(76, 735, 22, 80), Vector(6, -40)));
		animation->add(new Sprite(AABB(117.5, 736.5, 27, 77), Vector(6, -39)));
		animation->add(new Sprite(AABB(164.5, 734.5, 41, 77), Vector(0, -39)));
		animation->add(new Sprite(AABB(226, 734.5, 54, 75), Vector(5, -38)));
		animation->add(new Sprite(AABB(290, 734, 58, 76), Vector(5, -38)));
		animation->add(new Sprite(AABB(358.5, 734, 57, 78), Vector(3, -39)));
		animation->add(new Sprite(AABB(424.5, 735, 53, 76), Vector(0, -38)));
		animation->add(new Sprite(AABB(487, 734, 50, 78), Vector(-6, -39)));
		animation->add(new Sprite(AABB(543.5, 733, 41, 80), Vector(-3, -40)));
		animation->add(new Sprite(AABB(593, 732.5, 26, 81), Vector(0, -41)));
		animation->add(new Sprite(AABB(634.5, 732, 21, 82), Vector(5, -41)));
		// HangingForward - 13
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(32.5, 859.5, 29, 111), Vector(1, 55)));
		animation->add(new Sprite(AABB(91.5, 859.5, 33, 109), Vector(6, 54)));
		animation->add(new Sprite(AABB(152.5, 863.5, 41, 107), Vector(11, 53)));
		animation->add(new Sprite(AABB(206, 861.5, 40, 103), Vector(9, 51)));
		// HangingBackwards - 14
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(AABB(252.5, 864.5, 23, 109), Vector(-6, 54)));
		animation->add(new Sprite(AABB(295.5, 864, 35, 108), Vector(-11, 54)));
		animation->add(new Sprite(AABB(348.5, 863, 45, 106), Vector(-16, 53)));
		animation->add(new Sprite(AABB(408, 860.5, 52, 101), Vector(-22, 50)));
		animation->add(new Sprite(AABB(475.5, 860, 55, 102), Vector(-22, 51)));
		animation->add(new Sprite(AABB(539, 859.5, 56, 101), Vector(-23, 50)));
		animation->add(new Sprite(AABB(611, 858.5, 56, 99), Vector(-22, 49)));


#pragma endregion

		createPlayer(spritesheet);
		createChaser(spritesheet);
		//createSentry(spritesheet);
		createPatrol(spritesheet);
		//createCamera();
		createPlatforms();
		//createBackground();
	}
}
