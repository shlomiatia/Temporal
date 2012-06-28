#include "Position.h"
#include "Message.h"
#include "Serialization.h"
#include "BaseUtils.h"

namespace Temporal
{
	static const NumericPairSerializer POSITION_SERIALIZER("POS_SER");

	void Position::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_POSITION)
		{
			message.setParam(&_position);
		}
		else if(message.getID() == MessageID::SET_POSITION)
		{
			_position = *(Point*)message.getParam();
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			POSITION_SERIALIZER.serialize(serialization, _position);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			POSITION_SERIALIZER.deserialize(serialization, _position);
		}
	}

	Component* Position::clone(void) const
	{
		return new Position(_position);
	}
}