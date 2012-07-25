#include "StaticBody.h"
#include "Shapes.h"
#include "Graphics.h"
#include "MessageUtils.h"

namespace Temporal
{
	StaticBody::~StaticBody()
	{
		delete _shape;
	}

	void StaticBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().draw(*_shape, Color(0.5f, 0.25f, 0.125f));
		}
		else if(message.getID() == MessageID::GET_BOUNDS)
		{
			message.setParam(_shape);
		}
	}

	int StaticBody::getCollisionFilter() const
	{
		return getPeriod(*this);
	}
}