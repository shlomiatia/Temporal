#include "Sensor.h"
#include "Utils.h"
#include "StaticBodiesIndex.h"
#include <Temporal/Game/QueryManager.h>
#include <Temporal/Graphics/Graphics.h>

namespace Temporal
{
	Rect Sensor::getBounds(void) const
	{
		const Vector& position = *(const Vector* const)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum orientation = *(const Orientation::Enum* const)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return Rect(position.getX() + _offset.getX() * orientation,
					position.getY() + _offset.getY(),
					_size.getWidth(),
					_size.getHeight());
	}

	void Sensor::update(void)
	{
		_sensedBody = NULL;
		
		Rect bounds = getBounds();
		StaticBodiesIndex::get().iterateTiles(bounds, this, NULL, sense);
		if(_sensedBody != NULL)
		{
			sendMessageToOwner(Message(MessageID::SENSOR_COLLISION, this));
		}
	}

	void Sensor::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SENSOR_SIZE)
		{
			SensorID::Enum sensorID = *(const SensorID::Enum* const)message.getParam();
			if(sensorID == _id)
			{
				message.setParam(&_size);
			}
		}
		else if(message.getID() == MessageID::UPDATE)
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
		Orientation::Enum orientation = *(const Orientation::Enum* const)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Rect& sensorBounds = getBounds();
		if(!staticBody.isCover())
		{
			const Rect& staticBodyBounds = staticBody.getBounds();
			Direction::Enum collision = calculateCollision(sensorBounds, orientation, staticBodyBounds);
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
		Sensor* sensor = (Sensor*)caller;
		return sensor->sense(staticBody);
	}
}