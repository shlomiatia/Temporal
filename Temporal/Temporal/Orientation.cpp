#include "Orientation.h"
#include "Hash.h"
#include "Serialization.h"

namespace Temporal
{
	static const Hash ORIENTATION_SERIALIZATION = Hash("ORI_SER");

	void Orientation::handleMessage(Message& message)
	{
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
			Serialization& serialization = *static_cast<Serialization*>(message.getParam());
			serialization.serialize(ORIENTATION_SERIALIZATION, _orientation);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *static_cast<const Serialization*>(message.getParam());
			_orientation = (Side::Enum)serialization.deserializeInt(ORIENTATION_SERIALIZATION);
		}
	}

	Component* Orientation::clone() const
	{
		return new Orientation(_orientation);
	}
}