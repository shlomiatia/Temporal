#include "DrawPosition.h"
#include <Temporal\Game\Message.h>

namespace Temporal
{
	// TODO: Sprite named positions GRAPHICS
	void DrawPosition::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_DRAW_POSITION)
		{
			Vector* outParam = (Vector*)message.getParam();
			if(_override != Vector::Zero)
			{
				*outParam = _override;
			}
			else
			{
				const Vector& position = *(Vector*)sendMessageToOwner(Message(MessageID::GET_POSITION));
				*outParam = position + _offset;
			}
		}
		if(message.getID() == MessageID::SET_DRAW_POSITION_OVERRIDE)
		{
			_override = *(Vector*)message.getParam();
		}
	}
}