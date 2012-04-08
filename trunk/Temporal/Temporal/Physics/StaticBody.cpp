#include "StaticBody.h"
#include <Temporal\Base\Shape.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	StaticBody::~StaticBody(void)
	{
		delete _shape;
	}

	void StaticBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().draw(*_shape);
		}
		else if(message.getID() == MessageID::IS_COVER)
		{
			message.setParam(&_cover);
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			message.setParam(_shape);
		}
	}
}