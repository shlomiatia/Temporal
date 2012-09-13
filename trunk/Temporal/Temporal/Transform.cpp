#include "Transform.h"
#include "Serialization.h"
#include "MessageUtils.h"

namespace Temporal
{

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
		else if(message.getID() == MessageID::GET_ORIENTATION)
		{
			message.setParam(&_orientation);
		}
		else if(message.getID() == MessageID::FLIP_ORIENTATION)
		{
			_orientation = _orientation == Side::LEFT ? Side::RIGHT : Side::LEFT;
		}
	}
}