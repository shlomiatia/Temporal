#include "StaticBody.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	void StaticBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().drawSegment(_segment);
		}
		else if(message.getID() == MessageID::IS_COVER)
		{
			message.setParam(&_cover);
		}
		else if(message.getID() == MessageID::GET_BOUNDS)
		{
			message.setParam(&_segment);
		}
	}
}