#include "Transform.h"
#include "Serialization.h"
#include "BaseUtils.h"
#include "MessageUtils.h"

namespace Temporal
{
	static const VectorSerializer POSITION_SERIALIZER("POS_SER");
	static const Hash ORIENTATION_SERIALIZATION = Hash("ORI_SER");

	void Transform::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_POSITION)
		{
			message.setParam(&_position);
		}
		else if(message.getID() == MessageID::SET_POSITION)
		{
			_position = *static_cast<Vector*>(message.getParam());			
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			POSITION_SERIALIZER.serialize(serialization, _position);
			serialization.serialize(ORIENTATION_SERIALIZATION, _orientation);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			POSITION_SERIALIZER.deserialize(serialization, _position);
			_orientation = static_cast<Side::Enum>(serialization.deserializeInt(ORIENTATION_SERIALIZATION));
		}
		else if(message.getID() == MessageID::GET_ORIENTATION)
		{
			message.setParam(&_orientation);
		}
		else if(message.getID() == MessageID::FLIP_ORIENTATION)
		{
			_orientation = _orientation == Side::LEFT ? Side::RIGHT : Side::LEFT;
		}
	}

	Component* Transform::clone() const
	{
		return new Transform(_position, _orientation);
	}
}