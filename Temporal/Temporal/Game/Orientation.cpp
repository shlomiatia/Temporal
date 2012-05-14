#include "Orientation.h"
#include "Message.h"
#include <Temporal\Base\Hash.h>
#include <Temporal\Base\Serialization.h>

namespace Temporal
{
	void Orientation::handleMessage(Message& message)
	{
		static const Hash ORIENTATION_SERIALIZATION = Hash("ORI_SER");

		if(message.getID() == MessageID::GET_ORIENTATION)
		{
			message.setParam(&_orientation);
		}
		else if(message.getID() == MessageID::FLIP_ORIENTATION)
		{
			_orientation = _orientation == Side::LEFT ? Side::RIGHT : Side::LEFT;
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			serialization.serialize(ORIENTATION_SERIALIZATION, _orientation);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			_orientation = (Side::Enum)serialization.deserializeInt(ORIENTATION_SERIALIZATION);
		}
	}
}