#include "Position.h"

namespace Temporal
{
	void Position::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_POSITION)
		{
			message.setParam(&_position);
		}
		else if(message.getID() == MessageID::SET_POSITION)
		{
			_position = *(const Vector* const)message.getParam();
		}
	}
}