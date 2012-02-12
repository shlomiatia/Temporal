#include "Sensor.h"
#include "Utils.h"
#include <Temporal/Game/World.h>
#include <Temporal/Graphics/Graphics.h>

namespace Temporal
{
	Rect Sensor::getBounds(void) const
	{
		const Vector& position = *(const Vector* const)sendQueryMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum orientation = *(const Orientation::Enum* const)sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return Rect(position.getX() + _offset.getX() * orientation,
					position.getY() + _offset.getY(),
					_size.getWidth(),
					_size.getHeight());
	}

	void Sensor::update(void)
	{
		_sensedBody = NULL;
		Orientation::Enum orientation = *(const Orientation::Enum* const)sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Rect& sensorBounds = getBounds();
		ComponentOfTypeIteraor iterator = World::get().getComponentOfTypeIteraor(ComponentType::STATIC_BODY);
		while(iterator.next())
		{
			StaticBody& staticBody = (StaticBody&)iterator.current();
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
					break;
				}
			}
		}
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
}