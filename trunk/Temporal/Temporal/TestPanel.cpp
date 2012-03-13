#include "TestPanel.h"
#include "DebugInfo.h"

#include <Temporal\Base\Math.h>
#include <Temporal\Input\Input.h>
#include <Temporal\Input\InputController.h>
#include <Temporal\Physics\StaticBody.h>
#include <Temporal\Physics\DynamicBody.h>
#include <Temporal\Physics\Sensor.h>
#include <Temporal\Physics\Sight.h>
#include <Temporal\Physics\Grid.h>
#include <Temporal\Graphics\Texture.h>
#include <Temporal\Graphics\Sprite.h>
#include <Temporal\Graphics\SpriteGroup.h>
#include <Temporal\Graphics\SpriteSheet.h>
#include <Temporal\Graphics\Renderer.h>
#include <Temporal\Graphics\Animator.h>
#include <Temporal\Graphics\Graphics.h>
#include <Temporal\Graphics\ViewManager.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\Entity.h>
#include <Temporal\Game\Position.h>
#include <Temporal\Game\EntityOrientation.h>
#include <Temporal\Game\DrawPosition.h>
#include <Temporal\Game\ActionController.h>
#include <Temporal\Game\EntitiesManager.h>
#include <Temporal\Game\Game.h>
#include <Temporal\AI\NavigationGraph.h>
#include <Temporal\AI\Sentry.h>
#include <Temporal\AI\Camera.h>
#include <Temporal\AI\Patrol.h>

namespace Temporal
{
	#pragma region Helper Methods
	class AreaRenderer : public Component
	{
	public: 
		AreaRenderer(const SpriteSheet& spritesheet, VisualLayer::Enum layer, int spriteGroupID = 0, int spriteID = 0) : _spritesheet(spritesheet), _layer(layer), _spriteGroupID(spriteGroupID), _spriteID(spriteID) {};

		virtual ComponentType::Enum getType(void) const { return ComponentType::OTHER; }

		virtual void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::DRAW)
			{
				VisualLayer::Enum layer = *(const VisualLayer::Enum* const)message.getParam();
				if(_layer == layer)
				{
					Rect bounds(Vector::Zero, Vector(1.0f, 1.0f));
					sendMessageToOwner(Message(MessageID::GET_BOUNDS, &bounds));
					const Sprite& sprite = _spritesheet.get(_spriteGroupID).get(_spriteID);
					const Texture& texture = _spritesheet.getTexture();
					float textureWidth = texture.getSize().getWidth();
					float textureHeight = texture.getSize().getWidth();
					for(float x = bounds.getLeft(); x <= bounds.getRight(); x += (textureWidth - 1.0f))
					{
						if(bounds.getWidth() < textureWidth)
							x = bounds.getCenterX();
						for(float y = bounds.getBottom(); y <= bounds.getTop(); y += (textureHeight - 1.0f))
						{
							if(bounds.getHeight() < textureHeight)
								y = bounds.getCenterY();
							Graphics::get().drawTexture(_spritesheet.getTexture(), sprite.getBounds(), Vector(x, y), false);
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

	void addSensors(Entity& entity, DynamicBody& body)
	{
		// Jump Sensor
		/* v = F - G*T
		 * m = F*T - (G*T^2)/2 
		 *
		 * 0 = F - G*T
		 * G*T = F
		 * T = F/G
		 *
		 * y = F*(F/G) - (G*(F/G)^2)/2
		 * y = (F^2)/G - (G*F^2)/(2*G^2)
		 * y = (F^2)/G - (F^2)/(2*G)
		 * y = (2*F^2 - F^2)/(2*G)
		 * y = (F^2)/(2*G)
		 *
		 * x = F*T
		 * x = F*(F/G)
		 * x = (F^2)/G
		 */
		const float F = JUMP_FORCE_PER_SECOND;
		const float G = DynamicBody::GRAVITY;
		const float A = ANGLE_45_IN_RADIANS;
		float playerWidth = body.getSize().getWidth();
		float jumpSensorBackOffset = (playerWidth - 1.0f) / 2.0f;
		float playerHeight = body.getSize().getHeight();
		float jumpSensorWidth = pow(cos(A)*F, 2.0f) / (G) + jumpSensorBackOffset; 
		float jumpSensorHeight = pow(F, 2.0f) / (2*G);
		float sensorOffsetX = (jumpSensorWidth - 1.0f) / 2.0f - (jumpSensorBackOffset - 1.0f);
		float sensorOffsetY =  (playerHeight -1.0f + jumpSensorHeight - 1.0f) / 2.0f;
		Vector sensorOffset(sensorOffsetX, sensorOffsetY);
		Vector sensorSize(jumpSensorWidth, jumpSensorHeight);
		Sensor* sensor(new Sensor(SensorID::JUMP, sensorOffset, sensorSize, Direction::BOTTOM | Direction::FRONT, Direction::NONE));
		entity.add(sensor);

		// Hang Sensor
		const float HANG_SENSOR_SIZE = 20.0f;
		sensorSize = Vector(HANG_SENSOR_SIZE, HANG_SENSOR_SIZE);
		sensorOffsetX = (playerWidth -1.0f + HANG_SENSOR_SIZE - 1.0f) / 2.0f;
		sensorOffsetY = (playerHeight -1.0f + HANG_SENSOR_SIZE - 1.0f) / 2.0f;
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensor = new Sensor(SensorID::HANG, sensorOffset, sensorSize, Direction::BOTTOM | Direction::FRONT, Direction::NONE);
		entity.add(sensor);

		float edgeSensorWidth = playerWidth * 2.0f;

		// Back Edge Sensor
		sensorOffsetX = -(edgeSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		sensorOffsetY = -((playerHeight - 1.0f) / 2.0f);
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensorSize = Vector(playerWidth * 2.0f, 2.0f);
		sensor = new Sensor(SensorID::BACK_EDGE, sensorOffset, sensorSize, Direction::BOTTOM | Direction::FRONT, Direction::NONE);
		entity.add(sensor);

		// Front edge sensor
		sensorOffsetX = (edgeSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		sensorOffsetY = -((playerHeight - 1.0f) / 2.0f);
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensorSize = Vector(playerWidth * 2.0f, 2.0f);
		sensor = new Sensor(SensorID::FRONT_EDGE, sensorOffset, sensorSize, Direction::BOTTOM | Direction::BACK, Direction::NONE);
		entity.add(sensor);
	}

	std::vector<const Rect> _crap;
	Entity* CreatePlatformFromCenter(const Vector& center, const Vector& size, SpriteSheet* spritesheet, bool cover = false) 
	{
		if(!cover)
			_crap.push_back(Rect(center, size));
		Position* position = new Position(center);
		StaticBody* staticBody = new StaticBody(size, cover);
		AreaRenderer* renderer(new AreaRenderer(*spritesheet, cover ? VisualLayer::COVER : VisualLayer::STATIC));
		Entity* entity = new Entity();
		entity->add(position);
		entity->add(staticBody);
		entity->add(renderer);
		Grid::get().add(staticBody);
		return entity;
	}

	Entity* CreatePlatformFromBottomCenter(const Vector& bottomCenter, const Vector&  size, SpriteSheet* spritesheet, bool cover = false)
	{
		Vector center(bottomCenter.getX(), bottomCenter.getY() + (size.getHeight() - 1.0f) / 2.0f);
		return CreatePlatformFromCenter(center, size, spritesheet, cover);
	}

	Entity* CreatePlatformFromBottomLeft(const Vector& bottomLeft, const Vector&  size, SpriteSheet* spritesheet, bool cover = false)
	{
		Vector center(bottomLeft.getX() + (size.getWidth() - 1.0f) / 2.0f, bottomLeft.getY() + (size.getHeight() - 1.0f) / 2.0f);
		return CreatePlatformFromCenter(center, size, spritesheet, cover);
	}

	#pragma endregion
	#pragma region Creation Methods

	static const float ENTITY_WIDTH = 20.0f;
	static const float ENTITY_HEIGHT = 80.0f;

	void CreatePlayer(SpriteSheet* spritesheet)
	{
		// TODO: Central resources container FILES
		Position* position = new Position(Vector(512.0f, 768.0f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_HEIGHT - 1.0f) / 2.0f));
		InputController* controller = new InputController();
		DynamicBody* dynamicBody = new DynamicBody(Vector(ENTITY_WIDTH, ENTITY_HEIGHT));
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(66.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(drawPosition);
		entity->add(controller);
		entity->add(dynamicBody);
		addSensors(*entity, *dynamicBody);
		entity->add(actionController);
		entity->add(animator);
		entity->add(renderer);
		EntitiesManager::get().add(entity);
	}

	void CreateSentry(SpriteSheet* spritesheet)
	{
		Position* position = new Position(Vector(200.0f, 50.f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::RIGHT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_HEIGHT - 1.0f) / 2.0f));
		Sentry* sentry = new Sentry();
		Sight* sight = new Sight(ANGLE_30_IN_RADIANS, -ANGLE_30_IN_RADIANS);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::NPC);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(drawPosition);
		entity->add(sentry);
		entity->add(sight);
		entity->add(renderer);
		EntitiesManager::get().add(entity);
	}

	void CreatePatrol(SpriteSheet* spritesheet)
	{
		Position* position = new Position(Vector(512.0f, 768.0f));
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		DrawPosition* drawPosition = new DrawPosition(Vector(0.0f, -(ENTITY_HEIGHT - 1.0f) / 2.0f));
		Patrol* patrol = new Patrol();
		DynamicBody* dynamicBody = new DynamicBody(Vector(ENTITY_WIDTH, ENTITY_HEIGHT));
		ActionController* actionController = new ActionController();
		Animator* animator = new Animator(66.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::PC);
		Sight* sight = new Sight(ANGLE_30_IN_RADIANS, -ANGLE_30_IN_RADIANS);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(orientation);
		entity->add(drawPosition);
		entity->add(patrol);
		entity->add(dynamicBody);
		entity->add(sight);
		addSensors(*entity, *dynamicBody);
		entity->add(actionController);
		entity->add(animator);
		entity->add(renderer);
		EntitiesManager::get().add(entity);
	}

	void CreateCamera()
	{
		Position* position = new Position(Vector(200.0f, 120.f));
		const Texture* texture = Texture::load("c:\\camera.png");
		Camera* camera = new Camera();
		Sight* sight = new Sight(-ANGLE_15_IN_RADIANS, -ANGLE_45_IN_RADIANS);
		SpriteSheet* spritesheet = new SpriteSheet(texture, Orientation::LEFT);
		EntityOrientation* orientation = new EntityOrientation(Orientation::LEFT);
		SpriteGroup* animation;

#pragma region Camera Animation
		// Search - 0
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(19, 19, 25, 33), Vector(4, 16)));
		// See - 1
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(19, 59, 25, 33), Vector(4, 16)));
		// Turn - 2
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(50.5, 19.5, 18, 34), Vector(1, 16)));
		animation->add(new Sprite(Rect(76, 19.5, 13, 34), Vector(1, 16)));
		animation->add(new Sprite(Rect(98, 19.5, 13, 34), Vector(0, 16)));
		
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
		EntitiesManager::get().add(entity);
	}

	NavigationGraph* _crappy;
	void CreatePlatforms()
	{
		const Texture* texture = Texture::load("c:\\tile.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Orientation::NONE);
		SpriteGroup* animation = new SpriteGroup();
		spritesheet->add(animation);
		const Vector TILE_SIZE(32.0f, 32.0f);
		animation->add(new Sprite(Rect(TILE_SIZE / 2.0f, TILE_SIZE), Vector::Zero));

		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(0.0f, 0.0f), Vector(1024.0f, 16.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(0.0f, 0.0f), Vector(16.0f, 768.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(1009.0f, 0.0f), Vector(16.0f, 768.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(768.0f, 128.0f), Vector(256.0f, 16.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(0.0f, 128.0f), Vector(256.0f, 16.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(768.0f, 0.0f), Vector(16.0f, 144.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(0.0f, 256.0f), Vector(128.0f, 16.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(112.0f, 256.0f), Vector(16.0f, 128.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(128, 0.0f), Vector(16.0f, 144.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomCenter(Vector(512.0f, 128.0f), Vector(256.0f, 16.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomCenter(Vector(512.0f, 256.0f), Vector(256.0f, 16.0f), spritesheet));
		EntitiesManager::get().add(CreatePlatformFromBottomCenter(Vector(512.0f, 16.0f), Vector(256.0f, 64.0f), spritesheet, true));
		EntitiesManager::get().add(CreatePlatformFromBottomLeft(Vector(896.0f, 128.0f), Vector(128.0f, 144.0f), spritesheet));
		_crappy = new NavigationGraph(_crap);

	}

	void CreateBackground()
	{
		const Texture* texture = Texture::load("c:\\bg.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture, Orientation::NONE);
		SpriteGroup* animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(texture->getSize() / 2.0f, texture->getSize()), Vector::Zero));

		Position* position = new Position(texture->getSize() / 2.0f);
		Renderer* renderer = new Renderer(*spritesheet, VisualLayer::BACKGROUND);

		Entity* entity = new Entity();
		entity->add(position);
		entity->add(renderer);
		EntitiesManager::get().add(entity);
	}
#pragma endregion

	void TestPanel::init(void)
	{
		Vector screenSize = Vector(1024.0f, 768.0f);
		ViewManager::get().init(screenSize, 768.0f);
		Vector worldSize(1024.0f, 1536.0f);
		ViewManager::get().setLevelBounds(worldSize);
		Grid::get().init(worldSize, 32.0f);
		DebugInfo::get().setShowingFPS(true);

		const Texture* texture = Texture::load("c:\\pop.png");
		SpriteSheet* spritesheet(new SpriteSheet(texture, Orientation::LEFT));
		SpriteGroup* animation;

		#pragma region Player Animations
		// Stand - 0
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(26.5, 48, 22, 85), Vector(4, -42)));
		// Turn - 1
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(60.5, 50, 24, 81), Vector(6, -40)));
		animation->add(new Sprite(Rect(95.5, 50, 26, 81), Vector(6, -40)));
		animation->add(new Sprite(Rect(134.5, 48, 30, 85), Vector(4, -42)));
		animation->add(new Sprite(Rect(177, 49.5, 39, 82), Vector(4, -41)));
		animation->add(new Sprite(Rect(224.5, 50, 44, 81), Vector(-5, -40)));
		animation->add(new Sprite(Rect(276, 52, 37, 77), Vector(-11, -38)));
		animation->add(new Sprite(Rect(316.5, 51, 32, 79), Vector(-9, -39)));
		animation->add(new Sprite(Rect(359, 51.5, 29, 80), Vector(-9, -40)));
		// Drop - 2
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(418.5, 58.5, 24, 110), Vector(-4, 54)));
		animation->add(new Sprite(Rect(462.5, 60, 22, 97), Vector(-3, 48)));
		animation->add(new Sprite(Rect(509, 65, 23, 87), Vector(-3, 43)));
		animation->add(new Sprite(Rect(546.5, 69.5, 26, 78), Vector(1, 38)));
		animation->add(new Sprite(Rect(584.5, 67.5, 26, 82), Vector(7, 40)));
		// FallStart - 3
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(667.5, 71, 72, 77), Vector(3, -38)));
		animation->add(new Sprite(Rect(737, 68.5, 63, 76), Vector(3, -38)));
		animation->add(new Sprite(Rect(805.5, 67, 52, 73), Vector(8, -36)));
		animation->add(new Sprite(Rect(866, 62, 55, 81), Vector(8, -40)));
		// Fall - 4
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(927, 63, 35, 79), Vector(-2, -39)));
		// JumpUpStart - 5
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(30, 193.5, 23, 84), Vector(3, -42)));
		animation->add(new Sprite(Rect(72.5, 193, 24, 85), Vector(2, -42)));
		animation->add(new Sprite(Rect(114, 194, 23, 83), Vector(2, -41)));
		animation->add(new Sprite(Rect(158, 195, 29, 81), Vector(-1, -40)));
		animation->add(new Sprite(Rect(202, 195, 37, 81), Vector(-5, -40)));
		animation->add(new Sprite(Rect(245.5, 194.5, 44, 82), Vector(-5, -41)));
		animation->add(new Sprite(Rect(296, 194.5, 49, 82), Vector(-7, -41)));
		animation->add(new Sprite(Rect(349.5, 196, 44, 79), Vector(1, -39)));
		animation->add(new Sprite(Rect(396, 198.5, 33, 74), Vector(6, -37)));
		animation->add(new Sprite(Rect(438, 198.5, 33, 74), Vector(7, -37)));
		animation->add(new Sprite(Rect(486, 197.5, 43, 72), Vector(13, -36)));
		animation->add(new Sprite(Rect(536, 195.5, 35, 76), Vector(11, -38)));
		animation->add(new Sprite(Rect(582, 187.5, 33, 96), Vector(9, -48)));
		// JumpUp - 6
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(630, 189.5, 33, 104), Vector(9, -52)));
		// Hang - 7
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(717.5, 190.5, 24, 112), Vector(0, 55)));
		// Climbe - 8
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(24.5, 342, 22, 107), Vector(-4, 53)));
		animation->add(new Sprite(Rect(67, 348, 25, 89), Vector(-8, 44)));
		animation->add(new Sprite(Rect(109, 348.5, 27, 90), Vector(-3, 29)));
		animation->add(new Sprite(Rect(150.5, 346.5, 34, 92), Vector(-2, 20)));
		animation->add(new Sprite(Rect(200.5, 348.5, 46, 82), Vector(4, 13)));
		animation->add(new Sprite(Rect(258, 349, 49, 71), Vector(7, 10)));
		animation->add(new Sprite(Rect(319, 349.5, 47, 60), Vector(6, 3)));
		animation->add(new Sprite(Rect(385.5, 346, 62, 47), Vector(15, -8)));
		animation->add(new Sprite(Rect(459.5, 347.5, 60, 42), Vector(20, -10)));
		animation->add(new Sprite(Rect(528, 346.5, 63, 52), Vector(10, -13)));
		animation->add(new Sprite(Rect(600, 344.5, 57, 46), Vector(14, -23)));
		animation->add(new Sprite(Rect(660, 344.5, 53, 48), Vector(19, -24)));
		animation->add(new Sprite(Rect(727.5, 338.5, 54, 58), Vector(11, -29)));
		animation->add(new Sprite(Rect(787, 332.5, 53, 64), Vector(17, -32)));
		animation->add(new Sprite(Rect(843, 330, 51, 69), Vector(19, -34)));
		animation->add(new Sprite(Rect(896, 324, 31, 79), Vector(4, -39)));
		animation->add(new Sprite(Rect(937, 324.5, 23, 80), Vector(2, -40)));
		// JumpForwardStart - 9
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(20.5, 502, 26, 85), Vector(5, -42)));
		animation->add(new Sprite(Rect(59.5, 503, 28, 83), Vector(6, -41)));
		animation->add(new Sprite(Rect(106, 503, 43, 81), Vector(6, -40)));
		animation->add(new Sprite(Rect(159.5, 508, 48, 69), Vector(11, -34)));
		animation->add(new Sprite(Rect(225, 509, 57, 65), Vector(15, -32)));
		animation->add(new Sprite(Rect(298.5, 507.5, 56, 66), Vector(22, -33)));
		animation->add(new Sprite(Rect(369, 505, 55, 69), Vector(20, -34)));
		// JumpForward - 10
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(449.5, 503, 80, 71), Vector(2, -35)));
		animation->add(new Sprite(Rect(557.5, 501, 100, 71), Vector(1, -35)));
		animation->add(new Sprite(Rect(678.5, 507.5, 104, 62), Vector(-1, -31)));
		animation->add(new Sprite(Rect(788, 506.5, 85, 56), Vector(-4, -28)));
		// JumpForwardEnd - 11
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(52, 620.5, 59, 56), Vector(-8, -28)));
		animation->add(new Sprite(Rect(114.5, 621.5, 42, 52), Vector(-4, -26)));
		animation->add(new Sprite(Rect(189.5, 618, 56, 61), Vector(-13, -30)));
		animation->add(new Sprite(Rect(248, 613, 45, 71), Vector(-7, -35)));
		animation->add(new Sprite(Rect(306.5, 609, 36, 81), Vector(-3, -40)));
		animation->add(new Sprite(Rect(352, 608, 31, 83), Vector(-1, -41)));
		// Walk - 12
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(34, 735.5, 25, 82), Vector(7, -41)));
		animation->add(new Sprite(Rect(76, 735, 23, 81), Vector(7, -40)));
		animation->add(new Sprite(Rect(117.5, 736.5, 28, 78), Vector(5, -39)));
		animation->add(new Sprite(Rect(164.5, 734.5, 42, 78), Vector(0, -39)));
		animation->add(new Sprite(Rect(226, 734.5, 55, 76), Vector(7, -38)));
		animation->add(new Sprite(Rect(290, 734, 59, 77), Vector(6, -38)));
		animation->add(new Sprite(Rect(358.5, 734, 58, 79), Vector(4, -39)));
		animation->add(new Sprite(Rect(424.5, 735, 54, 77), Vector(2, -38)));
		animation->add(new Sprite(Rect(487, 734, 51, 79), Vector(-5, -39)));
		animation->add(new Sprite(Rect(543.5, 733, 42, 81), Vector(-1, -40)));
		animation->add(new Sprite(Rect(593, 732.5, 27, 82), Vector(0, -41)));
		animation->add(new Sprite(Rect(634.5, 732, 22, 83), Vector(5, -41)));
		// HangingForward - 13
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(32.5, 859.5, 30, 112), Vector(1, 55)));
		animation->add(new Sprite(Rect(91.5, 859.5, 34, 110), Vector(5, 54)));
		animation->add(new Sprite(Rect(152.5, 863.5, 42, 108), Vector(10, 53)));
		animation->add(new Sprite(Rect(206, 861.5, 41, 104), Vector(8, 51)));
		// HangingBackwards - 14
		animation = new SpriteGroup();
		spritesheet->add(animation);
		animation->add(new Sprite(Rect(252.5, 864.5, 24, 110), Vector(-6, 54)));
		animation->add(new Sprite(Rect(295.5, 864, 36, 109), Vector(-14, 54)));
		animation->add(new Sprite(Rect(348.5, 863, 46, 107), Vector(-18, 53)));
		animation->add(new Sprite(Rect(408, 860.5, 53, 102), Vector(-21, 50)));
		animation->add(new Sprite(Rect(475.5, 860, 56, 103), Vector(-21, 51)));
		animation->add(new Sprite(Rect(539, 859.5, 57, 102), Vector(-22, 50)));
		animation->add(new Sprite(Rect(611, 858.5, 57, 100), Vector(-23, 49)));

#pragma endregion

		CreatePlayer(spritesheet);
		CreateSentry(spritesheet);
		CreatePatrol(spritesheet);
		CreateCamera();
		CreatePlatforms();
		CreateBackground();
	}

	void TestPanel::update(float framePeriodInMillis)
	{
		Input::get().update();
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));

		const Vector& position = *(const Vector* const)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));
		ViewManager::get().setCameraCenter(position);
	}

	void TestPanel::draw(void) const
	{
		DebugInfo::get().draw();
		for(int i = VisualLayer::FARTHEST; i <= VisualLayer::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));

		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DEBUG_DRAW));
		_crappy->draw();
	}

	void TestPanel::dispose(void)
	{
		EntitiesManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
	}
}
