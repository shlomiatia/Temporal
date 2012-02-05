#include "EntityOrientation.h"

namespace Temporal
{
	void EntityOrientation::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_ORIENTATION)
		{
			message.setParam(&_orientation);
		}
		else if(message.getID() == MessageID::FLIP_ORIENTATION)
		{
			_orientation = _orientation == Orientation::LEFT ? Orientation::RIGHT : Orientation::LEFT;
		}
	}
}