#include "Sensor.h"
#include "Grid.h"
#include "StaticBody.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Segment& boundsB)
	{
		Direction::Enum collision(Direction::NONE);

		if(boundsA.intersects(boundsB))
		{
			float frontA = boundsA.getSide(orientationA);
			float frontB = boundsB.getSide(orientationA);
			float backA = boundsA.getOppositeSide(orientationA);
			float backB = boundsB.getOppositeSide(orientationA);
			bool overlapsX = boundsA.getLeft() < boundsB.getRight() && boundsA.getRight() > boundsB.getLeft();
			bool overlapsY = boundsA.getBottom() < boundsB.getTop() && boundsA.getTop() > boundsB.getBottom();
			if(boundsA.getTop() >= boundsB.getTop() && overlapsX) collision = collision | Direction::BOTTOM;
			if(boundsA.getBottom() <= boundsB.getBottom() && overlapsX) collision = collision | Direction::TOP;
			if((frontA - frontB) * orientationA >= 0 && overlapsY) collision = collision | Direction::BACK;
			if((backB - backA) * orientationA >= 0 && overlapsY) collision = collision | Direction::FRONT;
		}

		return collision;
	}

	Rect Sensor::getBounds(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum orientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return Rect(position.getX() + _offset.getVx() * orientation,
					position.getY() + _offset.getVy(),
					_size.getWidth(),
					_size.getHeight());
	}

	void Sensor::update(void)
	{
		_sensedBody = NULL;
		
		Rect bounds = getBounds();
		Grid::get().iterateTiles(bounds, this, NULL, sense);
		if(_sensedBody != NULL)
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
			Graphics::get().drawRect(getBounds(), getSensedBody() != NULL ? Color::Green : Color::Red);
		}
	}

	bool Sensor::sense(const StaticBody& staticBody)
	{
		Orientation::Enum orientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Rect& sensorBounds = getBounds();
		if(!staticBody.isCover())
		{
			const Segment& segment = staticBody.getSegment();
			Direction::Enum collision = calculateCollision(sensorBounds, orientation, segment);
			if(match(collision, _positive, _negative))
			{
				_sensedBody = &staticBody;
			}
			else if(collision != Direction::NONE)
			{
				_sensedBody = NULL;
				return false;
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