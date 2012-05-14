#include "EntityOrientation.h"
#include "Message.h"
#include <Temporal\Base\Hash.h>
#include <Temporal\Base\Serialization.h>

namespace Temporal
{
	void EntityOrientation::handleMessage(Message& message)
	{
		static const Hash ORIENTATION_SERIALIZATION = Hash("ORI_SER");

		if(message.getID() == MessageID::GET_ORIENTATION)
		{
			message.setParam(&_orientation);
		}
		else if(message.getID() == MessageID::FLIP_ORIENTATION)
		{
			_orientation = _orientation == Orientation::LEFT ? Orientation::RIGHT : Orientation::LEFT;
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			serialization.serialize(ORIENTATION_SERIALIZATION, _orientation);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			_orientation = (Orientation::Enum)serialization.deserializeInt(ORIENTATION_SERIALIZATION);
		}
	}
}