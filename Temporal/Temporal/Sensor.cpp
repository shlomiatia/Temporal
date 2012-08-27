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
	void Sensor::update()
	{
		_fixture->update();
		const AABB& sensorShape = static_cast<const AABB&>(_fixture->getGlobalShape());
		FixtureCollection info = Grid::get().iterateTiles(sensorShape, _collisionMask, _fixture->getFilter().getGroup());
		_listener->start();
		for(FixtureIterator i = info.begin(); i != info.end(); ++i)
		{
			const Fixture& fixture = **i;
			const Shape& shape = fixture.getGlobalShape();

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
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			_fixture->init(*this);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape());
		}
	}

	void LedgeDetector::start()
	{
		_point = Vector::Zero;
		_isBlocked = false;
	}

	void LedgeDetector::end()
	{
		if(_point != Vector::Zero)
		{
			getOwner().raiseMessage(Message(MessageID::SENSOR_COLLISION, &SensorCollisionParams(_id, _point == Vector::Zero ? NULL : &_point)));
		}
	}

	void LedgeDetector::handle(const Contact& contact)
	{
		if(_isBlocked)
			return;

		bool isSensing = false;

		const AABB& sensorShape = static_cast<const AABB&>(contact.getSource().getGlobalShape());
		const Segment& segment = static_cast<const Segment&>(contact.getTarget().getGlobalShape());

		// Then check if contain one of the edges
		Vector point = Vector::Zero;
		Vector leftPoint = segment.getLeftPoint();
		Vector rightPoint = segment.getRightPoint();
		if(sensorShape.contains(leftPoint))
			point = leftPoint;
		else if(sensorShape.contains(rightPoint))
			point = rightPoint;
		if(point != Vector::Zero)
		{
			Vector vector = segment.getNaturalVector();
					
			// Modify the angle according to relative position
			if((vector.getX() == 0.0f && segment.getTop() == point.getY()) ||
				(vector.getX() != 0.0f && segment.getRight() == point.getX())) 
			{
				vector = -vector;
			}
			float angle = vector.getAngle();
			Side::Enum orientation = getOrientation(getOwner());

			// Flip the range if looking backwards
			float rangeCenter = orientation == Side::RIGHT ? _rangeCenter : mirroredAngle( _rangeCenter);

			// Check distance
			float distance = minAnglesDistance(rangeCenter, angle);
			if(distance <= _rangeSize / 2.0f)
			{
				isSensing = true;
			}
		}
		if(isSensing)
		{
			_point = point;
		}
		else
		{
			_isBlocked = true;
			_point = Vector::Zero;
		}		
	}
}