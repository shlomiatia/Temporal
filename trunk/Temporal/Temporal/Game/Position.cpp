#include "Position.h"
#include "Message.h"
#include <Temporal\Base\Serialization.h>

namespace Temporal
{
	static const Hash X_SERIALIZATION = Hash("SER_POS_X");
	static const Hash Y_SERIALIZATION = Hash("SER_POS_Y");

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
			serialization.serialize(X_SERIALIZATION, _position.getX());
			serialization.serialize(Y_SERIALIZATION, _position.getY());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			_position.setX(serialization.deserializeFloat(X_SERIALIZATION));
			_position.setY(serialization.deserializeFloat(Y_SERIALIZATION));
		}
	}
}