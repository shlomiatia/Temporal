#include "Sensor.h"
#include "Grid.h"
#include "StaticBody.h"
#include <Temporal\Base\Math.h>
#include <Temporal\Base\Segment.h>
#include <Temporal\Base\ShapeOperations.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>
#include <algorithm>

namespace Temporal
{
	Rectangle Sensor::getBounds(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum orientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Point center(position.getX() + _offset.getVx() * orientation,
					 position.getY() + _offset.getVy());
		return Rectangle(center, _size);
	}

	void Sensor::update(void)
	{
		_point = NULL;
		
		Rectangle bounds = getBounds();
		Grid::get().iterateTiles(bounds, this, NULL, sense);
		if(_point != NULL)
		{
			sendMessageToOwner(Message(MessageID::SENSOR_COLLISION, this));
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
			Graphics::get().draw(getBounds(), getPoint() != NULL ? Color::Green : Color::Red);
		}
	}

	bool Sensor::sense(const StaticBody& staticBody)
	{
		const Rectangle& sensorBounds = getBounds();
		if(!staticBody.isCover())
		{
			const Shape& shape = staticBody.getShape();

			// First check for basic intersection
			if(intersects(sensorBounds, shape))
			{
				bool isSensing = false;
				const Segment& segment = (const Segment&)shape;

				// Then check if contain one of the edges
				if(sensorBounds.contains(segment.getPoint1()) || sensorBounds.contains(segment.getPoint2()))
				{
					Vector vector = segment.getPoint1().getX() <= segment.getPoint2().getX() ? 
																  segment.getPoint2() - segment.getPoint1() :
																  segment.getPoint1() - segment.getPoint2();
					float angle = vector.getAngle();

					// Modify the angle according to relative position
					if(sensorBounds.getTop() >= shape.getTop()) angle = -angle;
					if(sensorBounds.getRight() >= shape.getRight()) angle = PI - angle;
					Orientation::Enum orientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

					// Flip the range if looking backwards
					float rangeCenter = orientation == Orientation::RIGHT ? _rangeCenter : PI - _rangeCenter;

					// Check distance
					float clockwiseDistance = abs(rangeCenter - angle);
					float distance = std::min(clockwiseDistance,  2*PI - clockwiseDistance);
					if(distance <= _rangeSize / 2.0f)
					{
						isSensing = true;
					}
				}
				if(isSensing)
				{
					_point = sensorBounds.contains(segment.getPoint1()) ? &(segment.getPoint1()) : &(segment.getPoint2());
				}
				else
				{
					_point = NULL;
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