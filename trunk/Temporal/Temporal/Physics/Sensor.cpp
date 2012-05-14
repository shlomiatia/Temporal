#include "Sensor.h"
#include "Grid.h"
#include "StaticBody.h"
#include <Temporal\Base\Math.h>
#include <Temporal\Base\Segment.h>
#include <Temporal\Base\ShapeOperations.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageParams.h>
#include <Temporal\Graphics\Graphics.h>
#include <algorithm>

namespace Temporal
{
	AABB Sensor::getBounds(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Side::Enum orientation = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Point center(position.getX() + _offset.getVx() * orientation,
					 position.getY() + _offset.getVy());
		return AABB(center, _size);
	}

	void Sensor::update(void)
	{
		_point = Point::Zero;
		
		AABB bounds = getBounds();
		Grid::get().iterateTiles(bounds, this, NULL, sense);
		if(_point != Point::Zero)
		{
			sendMessageToOwner(Message(MessageID::SENSOR_COLLISION, &SensorCollisionParams(_id, _point == Vector::Zero ? NULL : &_point)));
		}
	}

	void Sensor::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().draw(getBounds(), _point != Point::Zero ? Color::Green : Color::Red);
		}
	}

	bool Sensor::sense(const StaticBody& staticBody)
	{
		const AABB& sensorBounds = getBounds();
		if(!staticBody.isCover())
		{
			const Shape& shape = staticBody.getShape();

			// First check for basic intersection
			if(intersects(sensorBounds, shape))
			{
				bool isSensing = false;
				const Segment& segment = (const Segment&)shape;

				// Then check if contain one of the edges
				Point point = Point::Zero;
				Point leftPoint = segment.getLeftPoint();
				Point rightPoint = segment.getRightPoint();
				if(sensorBounds.contains(leftPoint))
					point = leftPoint;
				else if(sensorBounds.contains(rightPoint))
					point = rightPoint;
				if(point != Point::Zero)
				{
					Vector vector = segment.getNaturalVector();
					
					// Modify the angle according to relative position
					if((vector.getVx() == 0.0f && shape.getTop() == point.getY()) ||
					   (vector.getVx() != 0.0f && shape.getRight() == point.getX())) 
					{
					   vector = -vector;
					}
					float angle = vector.getAngle();
					Side::Enum orientation = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

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
					_point = Point::Zero;
					return false;
				}
			}
		}
		return true;
	}

	bool Sensor::sense(void* caller, void* data, const StaticBody& staticBody)
	{
		Sensor& sensor = *(Sensor*)caller;
		return sensor.sense(staticBody);
	}
}