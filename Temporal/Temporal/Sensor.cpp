#include "Sensor.h"
#include "Grid.h"
#include "Math.h"
#include "ShapeOperations.h"
#include "MessageParams.h"
#include "MessageUtils.h"
#include "Graphics.h"
#include "CollisionInfo.h"
#include "Shapes.h"
#include "PhysicsEnums.h"
#include <algorithm>

namespace Temporal
{
	static const int COLLISION_MASK1 = Filter1::OBSTACLE;

	void Sensor::update()
	{
		_point = Point::Zero;
		const AABB& sensorShape = (const AABB&)_collisionInfo->getGlobalShape();
		CollisionInfoCollection info = Grid::get().iterateTiles(sensorShape, COLLISION_MASK1);
		for(CollisionInfoIterator i = info.begin(); i != info.end(); ++i)
		{
			const Shape& shape = (**i).getGlobalShape();

			// First check for basic intersection
			if(intersects(sensorShape, shape))
			{
				bool isSensing = false;
				const Segment& segment = (const Segment&)shape;

				// Then check if contain one of the edges
				Point point = Point::Zero;
				Point leftPoint = segment.getLeftPoint();
				Point rightPoint = segment.getRightPoint();
				if(sensorShape.contains(leftPoint))
					point = leftPoint;
				else if(sensorShape.contains(rightPoint))
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
					break;
				}
			}
		}
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
			Graphics::get().draw(_collisionInfo->getGlobalShape(), _point != Point::Zero ? Color::Green : Color::Red);
		}
	}
}