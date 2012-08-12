#include "MessageUtils.h"
#include "EntitySystem.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	const Point& getPosition(const Component& component)
	{
		return *static_cast<Point*>(component.sendMessageToOwner(Message(MessageID::GET_POSITION)));
	}

	Side::Enum getOrientation(const Component& component)
	{
		return *static_cast<Side::Enum*>(component.sendMessageToOwner(Message(MessageID::GET_ORIENTATION)));
	}

	const Hash& getHashParam(void* data)
	{
		return *static_cast<const Hash*>(data);
	}

	Serialization& getSerializationParam(void* data)
	{
		return *static_cast<Serialization*>(data);
	}

	const Serialization& getConstSerializationParam(void* data)
	{
		return *static_cast<const Serialization*>(data);
	}

	const ResetAnimationParams& getResetAnimationParams(void* data)
	{
		return *static_cast<const ResetAnimationParams*>(data);
	}

	const SensorCollisionParams& getSensorCollisionParams(void* data)
	{
		return *static_cast<const SensorCollisionParams*>(data);
	}

	const Vector& getVectorParam(void* data)
	{
		return *static_cast<const Vector*>(data);
	}

	void sendDirectionAction(const Component& component, Side::Enum direction)
	{
		Side::Enum orientation = getOrientation(component);
		if(direction == orientation)
			component.sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
		else
			component.sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
	}

	bool isSensorCollisionMessage(Message& message, const Hash& sensorID)
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const SensorCollisionParams& params = *static_cast<SensorCollisionParams*>(message.getParam());
			if(params.getSensorID() == sensorID)
				return true;
		}
		return false;
	}
}