#include "TestPanel.h"
#include "DebugInfo.h"

#include <Temporal\Graphics\Graphics.h>
#include <Temporal\Input\Input.h>
#include <Temporal\Physics\Body.h>
#include <Temporal\Physics\Physics.h>
#include <Temporal\Game\InputEntityController.h>
#include <Temporal\Game\BackgroundEntity.h>
#include <Temporal\Game\StaticEntity.h>
#include <Temporal\Game\DynamicEntity.h>
#include <math.h>

namespace Temporal
{
	bool _crappy(false);
	void addSensors(DynamicEntity& entity)
	{
		DynamicBody& staticBody = entity.getBody();
		// Jump Sensor
		/* y = Time*(Force-Time*Gravity)
		 * Sum = N*(A1+AN)/2
		 *
		 * 0 = T*(F-T*G)
		 * T*F - G*T^2 = 0
		 * T*F = G*T^2
		 * F = T*G
		 * T = F/G
		 *
		 * Sum = F/G(F-G+0)/2
		 * Sum = F*(F-G)/2*G
		 *
		 * T = (sin(45)*F)/G
		 * Sum = (2*(sin(45)*F)/G)*(2*cos(45)*F)/2
		 * Sum = (2*sin(45)*cos(45)*F^2)/G
		 */
		const float F = entity.JUMP_FORCE;
		const float G = Physics::GRAVITY;
		const float A = toRadians(45);
		float playerWidth = staticBody.getBounds().getWidth();
		float jumpSensorBackOffset = (playerWidth - 1.0f) / 2.0f;
		float playerHeight = staticBody.getBounds().getHeight();
		float jumpSensorWidth = sin(A)*cos(A)*pow(F, 2)/G + jumpSensorBackOffset; 
		float jumpSensorHeight = (F*(F-G))/(2*G);
		float sensorOffsetX = (jumpSensorWidth - 1.0f) / 2.0f - (jumpSensorBackOffset - 1.0f);
		float sensorOffsetY =  (playerHeight -1.0f + jumpSensorHeight - 1.0f) / 2.0f;
		Vector sensorOffset(sensorOffsetX, sensorOffsetY);
		Vector sensorSize(jumpSensorWidth, jumpSensorHeight);
		Sensor* sensor(new Sensor(staticBody, sensorOffset, sensorSize, Direction::BOTTOM | Direction::FRONT, Direction::NONE));
		
		// Hang Sensor
		const float HANG_SENSOR_SIZE = 20.0f;
		sensorSize = Vector(HANG_SENSOR_SIZE, HANG_SENSOR_SIZE);
		sensorOffsetX = (playerWidth -1.0f + HANG_SENSOR_SIZE - 1.0f) / 2.0f;
		sensorOffsetY = (playerHeight -1.0f + HANG_SENSOR_SIZE - 1.0f) / 2.0f;
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensor = new Sensor(staticBody, sensorOffset, sensorSize, Direction::BOTTOM | Direction::FRONT, Direction::NONE);

		float edgeSensorWidth = playerWidth * 2.0f;

		// Back Edge Sensor
		sensorOffsetX = -(edgeSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		sensorOffsetY = -staticBody.getBounds().getOffsetY();
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensorSize = Vector(playerWidth * 2.0f, 2.0f);
		sensor = new Sensor(staticBody, sensorOffset, sensorSize, Direction::BOTTOM | Direction::FRONT, Direction::NONE);

		// Front edge sensor
		sensorOffsetX = (edgeSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		sensorOffsetY = -staticBody.getBounds().getOffsetY();
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensorSize = Vector(playerWidth * 2.0f, 2.0f);
		sensor = new Sensor(staticBody, sensorOffset, sensorSize, Direction::BOTTOM | Direction::BACK, Direction::NONE);
	}

	void TestPanel::init(void)
	{
		Vector screenSize = Vector(1024.0f, 768.0f);
		Graphics::get().init(screenSize, screenSize);
		DebugInfo::get().setShowingFPS(true);

		const EntityController* const controller = new InputEntityController();
		DynamicBody* dynamicBody = new DynamicBody(Vector(512.0f, 768.0f), Vector(20.0f, 80.0f), Orientation::LEFT);
		const Texture* texture = Texture::load("c:\\pop.png");
		SpriteSheet* spritesheet = new SpriteSheet(texture);
		Animation* animation;
		
#pragma region Crap
// Stand - 0
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(26.5, 48, 22, 85), Vector(5, -2)));
// Turn - 1
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(60.5, 50, 24, 81), Vector(6, 0)));
animation->add(new Frame(Rect(95.5, 50, 26, 81), Vector(7, 0)));
animation->add(new Frame(Rect(134.5, 48, 30, 85), Vector(5, -2)));
animation->add(new Frame(Rect(177, 49.5, 39, 82), Vector(0, -1)));
animation->add(new Frame(Rect(224.5, 50, 44, 81), Vector(-5, 0)));
animation->add(new Frame(Rect(276, 52, 37, 77), Vector(-11, 2)));
animation->add(new Frame(Rect(316.5, 51, 32, 79), Vector(-8, 1)));
animation->add(new Frame(Rect(359, 51.5, 29, 80), Vector(-8, 0)));
// Drop - 2
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(418.5, 58.5, 24, 110), Vector(-5, -15)));
animation->add(new Frame(Rect(462.5, 60, 22, 97), Vector(1, -8)));
animation->add(new Frame(Rect(509, 65, 23, 87), Vector(3, -3)));
animation->add(new Frame(Rect(546.5, 69.5, 26, 78), Vector(6, 1)));
animation->add(new Frame(Rect(584.5, 67.5, 26, 82), Vector(8, -1)));
// FallStart - 3
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(667.5, 71, 72, 77), Vector(4, 2)));
animation->add(new Frame(Rect(737, 68.5, 63, 76), Vector(4, 2)));
animation->add(new Frame(Rect(805.5, 67, 52, 73), Vector(9, 4)));
animation->add(new Frame(Rect(866, 62, 55, 81), Vector(9, 0)));
// Fall - 4
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(927, 63, 35, 79), Vector(-1, 1)));
// JumpUpStart - 5
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(30, 193.5, 23, 84), Vector(2, -2)));
animation->add(new Frame(Rect(72.5, 193, 24, 85), Vector(3, -2)));
animation->add(new Frame(Rect(114, 194, 23, 83), Vector(3, -1)));
animation->add(new Frame(Rect(158, 195, 29, 81), Vector(0, 0)));
animation->add(new Frame(Rect(202, 195, 37, 81), Vector(-2, 0)));
animation->add(new Frame(Rect(245.5, 194.5, 44, 82), Vector(-6, -1)));
animation->add(new Frame(Rect(296, 194.5, 49, 82), Vector(-8, -1)));
animation->add(new Frame(Rect(349.5, 196, 44, 79), Vector(2, 1)));
animation->add(new Frame(Rect(396, 198.5, 33, 74), Vector(8, 3)));
animation->add(new Frame(Rect(438, 198.5, 33, 74), Vector(7, 3)));
animation->add(new Frame(Rect(486, 197.5, 43, 72), Vector(13, 4)));
animation->add(new Frame(Rect(536, 195.5, 35, 76), Vector(13, 2)));
animation->add(new Frame(Rect(582, 187.5, 33, 96), Vector(8, -8)));
// JumpUp - 6
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(630, 189.5, 33, 104), Vector(3, 12)));
// Hang - 7
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(717.5, 190.5, 24, 112), Vector(-3, 16)));
// Climbe - 8
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(24.5, 342, 22, 107), Vector(-6, 49)));
animation->add(new Frame(Rect(67, 348, 25, 89), Vector(-8, 40)));
animation->add(new Frame(Rect(109, 348.5, 27, 90), Vector(-3, 31)));
animation->add(new Frame(Rect(150.5, 346.5, 34, 92), Vector(-2, 20)));
animation->add(new Frame(Rect(200.5, 348.5, 46, 82), Vector(5, 14)));
animation->add(new Frame(Rect(258, 349, 49, 71), Vector(7, 10)));
animation->add(new Frame(Rect(319, 349.5, 47, 60), Vector(6, 3)));
animation->add(new Frame(Rect(385.5, 346, 62, 47), Vector(15, -8)));
animation->add(new Frame(Rect(459.5, 347.5, 60, 42), Vector(20, -10)));
animation->add(new Frame(Rect(528, 346.5, 63, 52), Vector(10, -13)));
animation->add(new Frame(Rect(600, 344.5, 57, 46), Vector(15, -21)));
animation->add(new Frame(Rect(660, 344.5, 53, 48), Vector(20, -22)));
animation->add(new Frame(Rect(727.5, 338.5, 54, 58), Vector(10, -26)));
animation->add(new Frame(Rect(787, 332.5, 53, 64), Vector(17, -32)));
animation->add(new Frame(Rect(843, 330, 51, 69), Vector(19, -31)));
animation->add(new Frame(Rect(896, 324, 31, 79), Vector(3, -37)));
animation->add(new Frame(Rect(937, 324.5, 23, 80), Vector(1, -39)));
// JumpForwardStart - 9
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(20.5, 502, 26, 85), Vector(4, -2)));
animation->add(new Frame(Rect(59.5, 503, 28, 83), Vector(7, -1)));
animation->add(new Frame(Rect(106, 503, 43, 81), Vector(8, 0)));
animation->add(new Frame(Rect(159.5, 508, 48, 69), Vector(12, 6)));
animation->add(new Frame(Rect(225, 509, 57, 65), Vector(16, 8)));
animation->add(new Frame(Rect(298.5, 507.5, 56, 66), Vector(24, 7)));
animation->add(new Frame(Rect(369, 505, 55, 69), Vector(22, 6)));
// JumpForward - 10
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(449.5, 503, 80, 71), Vector(0, 5)));
animation->add(new Frame(Rect(557.5, 501, 100, 71), Vector(0, 5)));
animation->add(new Frame(Rect(678.5, 507.5, 104, 62), Vector(0, 9)));
animation->add(new Frame(Rect(788, 506.5, 85, 56), Vector(0, 12)));
// JumpForwardEnd - 11
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(52, 620.5, 59, 56), Vector(-8, 12)));
animation->add(new Frame(Rect(114.5, 621.5, 42, 52), Vector(-4, 14)));
animation->add(new Frame(Rect(189.5, 618, 56, 61), Vector(-13, 10)));
animation->add(new Frame(Rect(248, 613, 45, 71), Vector(-10, 5)));
animation->add(new Frame(Rect(306.5, 609, 36, 81), Vector(-5, 0)));
animation->add(new Frame(Rect(352, 608, 31, 83), Vector(-3, -1)));
// Walk - 12
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(34, 735.5, 25, 82), Vector(0, -1)));
animation->add(new Frame(Rect(76, 735, 23, 81), Vector(0, 0)));
animation->add(new Frame(Rect(117.5, 736.5, 28, 78), Vector(0, 1)));
animation->add(new Frame(Rect(164.5, 734.5, 42, 78), Vector(0, 1)));
animation->add(new Frame(Rect(226, 734.5, 55, 76), Vector(0, 2)));
animation->add(new Frame(Rect(290, 734, 59, 77), Vector(0, 2)));
animation->add(new Frame(Rect(358.5, 734, 58, 79), Vector(0, 1)));
animation->add(new Frame(Rect(424.5, 735, 54, 77), Vector(0, 2)));
animation->add(new Frame(Rect(487, 734, 51, 79), Vector(0, 1)));
animation->add(new Frame(Rect(543.5, 733, 42, 81), Vector(0, 0)));
animation->add(new Frame(Rect(593, 732.5, 27, 82), Vector(0, -1)));
animation->add(new Frame(Rect(634.5, 732, 22, 83), Vector(0, -1)));
// HangingForward - 13
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(32.5, 859.5, 30, 112), Vector(3, 16)));
animation->add(new Frame(Rect(91.5, 859.5, 34, 110), Vector(6, 15)));
animation->add(new Frame(Rect(152.5, 863.5, 42, 108), Vector(11, 14)));
animation->add(new Frame(Rect(206, 861.5, 41, 104), Vector(9, 12)));
// HangingBackwards - 14
animation = new Animation();
spritesheet->add(animation);
animation->add(new Frame(Rect(252.5, 864.5, 24, 110), Vector(-6, 15)));
animation->add(new Frame(Rect(295.5, 864, 36, 109), Vector(-14, 14)));
animation->add(new Frame(Rect(348.5, 863, 46, 107), Vector(-19, 13)));
animation->add(new Frame(Rect(408, 860.5, 53, 102), Vector(-22, 11)));
animation->add(new Frame(Rect(475.5, 860, 56, 103), Vector(-22, 11)));
animation->add(new Frame(Rect(539, 859.5, 57, 102), Vector(-24, 11)));
animation->add(new Frame(Rect(611, 858.5, 57, 100), Vector(-26, 10)));

#pragma endregion Crap

		_elements[_elementsCount++] = new DynamicEntity(controller, *dynamicBody, *spritesheet, VisualLayer::DYNAMIC, EntityStateID::STAND);
		addSensors(*(DynamicEntity*)_elements[_elementsCount - 1]);
		Physics::get().add(dynamicBody);
		
		dynamicBody = new DynamicBody(Vector(512.0f, 768.0f), Vector(20.0f, 80.0f), Orientation::LEFT);
		_elements[_elementsCount++] = new DynamicEntity(new CrappyEntityController(), *dynamicBody, *spritesheet, VisualLayer::DYNAMIC, EntityStateID::STAND);
		addSensors(*(DynamicEntity*)_elements[_elementsCount - 1]);
		Physics::get().add(dynamicBody);

		texture = Texture::load("c:\\tile.png");
		spritesheet = new SpriteSheet(texture);
		animation = new Animation();
		spritesheet->add(animation);
		const Vector TILE_SIZE(32.0f, 32.0f);
		animation->add(new Frame(Rect(TILE_SIZE / 2.0f, TILE_SIZE), Vector::Zero));

		StaticBody* staticBody = new StaticBody(Vector(512.0f, 8.0f), Vector(1024.0f, 16.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(8.0f, 384.0f), Vector(16.0f, 768.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(1016.0f, 384.0f), Vector(16.0f, 768.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(896.0f, 128.0f), Vector(256.0f, 16.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(128.0f, 128.0f), Vector(256.0f, 16.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(776.0f, 68.0f), Vector(16.0f, 136.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(64.0f, 256.0f), Vector(128.0f, 16.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(120.0f, 312.0f), Vector(16.0f, 128.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(120.0f, 68.0f), Vector(16.0f, 136.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(512.0f, 128.0f), Vector(300.0f, 16.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);
		staticBody = new StaticBody(Vector(960.0f, 184.0f), Vector(128.0f, 128.0f));
		_elements[_elementsCount++] = new StaticEntity(*staticBody, *spritesheet, VisualLayer::STATIC);
		Physics::get().add(staticBody);

		texture = Texture::load("c:\\bg.png");
		spritesheet = new SpriteSheet(texture);
		animation = new Animation();
		spritesheet->add(animation);
		animation->add(new Frame(Rect(screenSize / 2.0f, screenSize), Vector::Zero));
		_elements[_elementsCount++] = new BackgroundEntity(screenSize / 2.0f, *spritesheet, VisualLayer::BACKGROUND);
	}

	void TestPanel::update(void)
	{
		Input::get().update();
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		for(int i = 0; i < _elementsCount; ++i)
			_elements[i]->update();
		Physics::get().update();
	}

	void TestPanel::draw(void)
	{
		DebugInfo::get().draw();
		
		for(int layer = VisualLayer::FARTHEST; layer <= VisualLayer::NEAREST; ++layer)
			for(int i = 0; i < _elementsCount; ++i)
				if(_elements[i]->getVisualLayer() == layer)
					_elements[i]->draw();
		
		/*for(int i = 0; i < Physics::get()._staticBodiesCount; ++i)
		{
			const StaticBody& staticBody = *Physics::get()._staticBodies[i];
			Graphics::get().drawRect(staticBody.getBounds());
		}
		for(int i = 0; i < Physics::get()._dynamicBodiesCount; ++i)
		{
			const DynamicBody& staticBody = *Physics::get()._dynamicBodies[i];
			Graphics::get().drawRect(staticBody.getBounds());
			for(int j = 0; j < staticBody._elementsCount; ++j)
			{
				const Sensor& sensor = *staticBody._elements[j];
				Graphics::get().drawRect(sensor.getBounds(), sensor.isSensing() ? Color::Green : Color::Red);
			}
		}*/
	}

	void TestPanel::dispose(void)
	{
		for(int i = 0; i < _elementsCount; ++i)
			delete _elements[i];
		Graphics::get().dispose();
	}
}
