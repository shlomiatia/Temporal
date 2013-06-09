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
		_fixture->update();
		const YABP& sensorShape = _fixture->getGlobalShape();
		FixtureCollection info = Grid::get().iterateTiles(sensorShape, _categoryMask, _fixture->getFilter().getGroup());
		raiseMessage(Message(MessageID::SENSOR_START, &_id));
		for(FixtureIterator i = info.begin(); i != info.end(); ++i)
		{
			const Fixture& fixture = **i;
			const YABP& shape = fixture.getGlobalShape();

			Contact contact(*_fixture, fixture);
			raiseMessage(Message(MessageID::SENSOR_SENSE, &SensorParams(_id, &contact)));
		}
		raiseMessage(Message(MessageID::SENSOR_END, &_id));
	}

	void Sensor::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_fixture->init(*this);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
	}

	void ContactListener::handleMessage(Component& component, Message& message)
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
				end(component);
		}
	}
}