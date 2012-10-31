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
		return new Sensor(_fixture->clone(), _listener->clone(), _categoryMask);
	}

	void Sensor::update()
	{
		_fixture->update();
		const YABP& sensorShape = _fixture->getGlobalShape();
		FixtureCollection info = Grid::get().iterateTiles(sensorShape, _categoryMask, _fixture->getFilter().getGroup());
		_listener->start();
		for(FixtureIterator i = info.begin(); i != info.end(); ++i)
		{
			const Fixture& fixture = **i;
			const YABP& shape = fixture.getGlobalShape();

			// First check for basic intersection
			if(intersects(sensorShape, shape))
			{
				Contact contact(*_fixture, fixture);
				_listener->handle(contact);
			}
		}
		_listener->end();
	}

	void Sensor::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_fixture->init(*this);
			_listener->setOwner(*this);
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

	void LedgeDetector::start()
	{
		_platform = 0;
	}

	void LedgeDetector::end()
	{
		if(_platform)
		{
			getOwner().raiseMessage(Message(MessageID::LEDGE_DETECTED, &LedgeDetectionParams(_id, _platform)));
		}
	}

	void LedgeDetector::handle(const Contact& contact)
	{
		const YABP& platform = static_cast<const YABP&>(contact.getTarget().getGlobalShape());
		if(platform.getHeight() <= 1.0f)
			_platform = &platform;
	}
}