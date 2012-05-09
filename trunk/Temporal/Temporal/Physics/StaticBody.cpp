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
	}
}