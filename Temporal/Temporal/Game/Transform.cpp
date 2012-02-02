#include "Transform.h"

namespace Temporal
{
	void Transform::handleMessage(Message& message)
	{
		switch(message.getID())
		{
			case(MessageID::GET_POSITION):
			{
				message.setParam(&_position);
				break;
			}
			case(MessageID::SET_POSITION):
			{
				_position = message.getParam<Vector>();
				break;
			}
			case(MessageID::GET_ORIENTATION):
			{
				message.setParam(&_orientation);
				break;
			}
			case(MessageID::FLIP_ORIENTATION):
			{
				_orientation = _orientation == Orientation::LEFT ? Orientation::RIGHT : Orientation::LEFT;
				break;
			}
		}
	}
}