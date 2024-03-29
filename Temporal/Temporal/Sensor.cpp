#include "Sensor.h"
#include "Grid.h"
#include "Math.h"
#include "ShapeOperations.h"
#include "MessageUtils.h"
#include "Graphics.h"
#include "Shapes.h"
#include "PhysicsEnums.h"
#include "CollisionFilter.h"

#include <algorithm>

namespace Temporal
{
	Component* Sensor::clone() const
	{
		return new Sensor(_id, _fixture->clone(), _categoryMask);
	}

	void Sensor::update()
	{
		const OBB& sensorShape = _fixture->getGlobalShape();
		FixtureList info;
		getEntity().getManager().getGameState().getGrid().iterateTiles(sensorShape, _categoryMask, _fixture->getGroup(), &info);
		_sensing = false;
		raiseMessage(Message(MessageID::SENSOR_START, &_id));
		for(FixtureIterator i = info.begin(); i != info.end(); ++i)
		{
			const Fixture& fixture = **i;
			const OBB& shape = fixture.getGlobalShape();

			Contact contact(*_fixture, fixture);
			_sensing = true;
			SensorParams params(_id, &contact);
			raiseMessage(Message(MessageID::SENSOR_SENSE, &params));
			if (params.isHandled())
				break;
		}
		raiseMessage(Message(MessageID::SENSOR_END, &_id));
	}

	void Sensor::handleMessage(Message& message)
	{
		handleNonGridFixtureMessage(message, *this, *_fixture);
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().getLinesSpriteBatch().add(_fixture->getGlobalShape(), _sensing ? Color(0.0f, 1.0f, 0.0f, 0.5f) : Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
	}

	void ContactListener::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SENSOR_START)
		{
			Hash sensorId = getHashParam(message.getParam());
			if(_sensorId == sensorId)
				start();
		}
		else if(message.getID() == MessageID::SENSOR_SENSE)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if(_sensorId == params.getSensorId())
				handle(params.getContact());
		}
		else if(message.getID() == MessageID::SENSOR_END)
		{
			Hash sensorId = getHashParam(message.getParam());
			if(_sensorId == sensorId)
				end();
		}
	}
}