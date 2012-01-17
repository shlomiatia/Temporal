#include "TestPanel.h"
#include "DebugInfo.h"

#include <Temporal\Graphics\Graphics.h>
#include <Temporal\Input\Input.h>
#include <Temporal\Physics\Physics.h>
#include <Temporal\Game\InputEntityController.h>
#include <math.h>

namespace Temporal
{
	void TestPanel::init(void)
	{
		Graphics::get().init(Vector(1024.0f, 768.0f), Vector(1024.0f, 768.0f));
		DebugInfo::get().setShowingFPS(false);

		Body* body = new Body(true, Vector(512.0f, 1024.0f), Vector(20.0f, 80.0f), Orientation::RIGHT);
		const EntityController* const controller = new InputEntityController();
		_entity = new Entity(*body, controller);
		Physics::get().add(body);

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
		const float F = _entity->JUMP_FORCE;
		const float G = Physics::GRAVITY;
		const float A = toRadians(45);
		float playerWidth = body->getBounds().getWidth();
		float playerHeight = body->getBounds().getHeight();
		float halfOfPlayerWidth = playerWidth / 2.0f;
		float jumpSensorWidth = sin(A)*cos(A)*pow(F, 2)/G + halfOfPlayerWidth; 
		float jumpSensorHeight = (F*(F-G))/(2*G);
		float sensorOffsetX = (jumpSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		float sensorOffsetY =  (playerHeight -1.0f + jumpSensorHeight - 1.0f) / 2.0f;
		Vector sensorOffset(sensorOffsetX, sensorOffsetY);
		Vector sensorSize(jumpSensorWidth, jumpSensorHeight);
		Sensor* sensor(new Sensor(*body, sensorOffset, sensorSize));
		
		// Hang Sensor
		const float HANG_SENSOR_SIZE = 20.0f;
		sensorSize = Vector(HANG_SENSOR_SIZE, HANG_SENSOR_SIZE);
		sensorOffsetX = (playerWidth -1.0f + HANG_SENSOR_SIZE - 1.0f) / 2.0f;
		sensorOffsetY = (playerHeight -1.0f + HANG_SENSOR_SIZE - 1.0f) / 2.0f;
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensor = new Sensor(*body, sensorOffset, sensorSize);

		float edgeSensorWidth = playerWidth * 2;

		// Back Edge Sensor
		sensorOffsetX = -(edgeSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		sensorOffsetY = -body->getBounds().getOffsetY();
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensorSize = Vector(playerWidth * 2, 2.0f);
		sensor = new Sensor(*body, sensorOffset, sensorSize);

		// Front edge sensor
		sensorOffsetX = (edgeSensorWidth -1.0f - (playerWidth - 1.0f)) / 2.0f;
		sensorOffsetY = -body->getBounds().getOffsetY();
		sensorOffset = Vector(sensorOffsetX, sensorOffsetY);
		sensorSize = Vector(playerWidth * 2, 2.0f);
		sensor = new Sensor(*body, sensorOffset, sensorSize);

		body = new Body(false, Vector(512.0f, 10.0f), Vector(1024.0f, 21.0f));
		Physics::get().add(body);
		body = new Body(false, Vector(10.0f, 384.0f), Vector(21.0f, 768.0f));
		Physics::get().add(body);
		body = new Body(false, Vector(256.0f, 149.0f), Vector(256.0f, 21.0f));
		Physics::get().add(body);
		body = new Body(false, Vector(1014.0f, 45.0f), Vector(21.0f, 90.0f));
		Physics::get().add(body);
	}

	void TestPanel::update(void)
	{
		Input::get().update();
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		_entity->update();
		Physics::get().update();
	}

	void TestPanel::draw(void)
	{
		DebugInfo::get().draw();
		for(int i = 0; i < Physics::get()._elementsCount; ++i)
		{
			const Body& body = *Physics::get()._elements[i];
			Graphics::get().drawRect(body.getBounds());
			for(int j = 0; j < body._elementsCount; ++j)
			{
				const Sensor& sensor = *body._elements[j];
				Graphics::get().drawRect(sensor.getBounds(), sensor.getSensedBody() == NULL ? Color::Yellow : Color::Red);
			}
		}
	}

	void TestPanel::dispose(void)
	{
		delete _entity;
		Graphics::get().dispose();
	}
}
