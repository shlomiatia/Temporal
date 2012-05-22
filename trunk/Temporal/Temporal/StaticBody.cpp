#include "StaticBody.h"
#include "Shape.h"
#include "Message.h"
#include "Graphics.h"

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
		else if(message.getID() == MessageID::GET_BOUNDS)
		{
			message.setParam(_shape);
		}
	}
}