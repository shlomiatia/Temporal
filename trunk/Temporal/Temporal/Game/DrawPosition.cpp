#include "DrawPosition.h"
#include <Temporal\Game\Message.h>

namespace Temporal
{
	void DrawPosition::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_DRAW_POSITION)
		{
			Point* outParam = (Point*)message.getParam();
			if(_override != Point::Zero)
			{
				*outParam = _override;
			}
			else
			{
				const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
				*outParam = position + _offset;
			}
		}
		if(message.getID() == MessageID::SET_DRAW_POSITION_OVERRIDE)
		{
			_override = *(Point*)message.getParam();
		}
	}
}