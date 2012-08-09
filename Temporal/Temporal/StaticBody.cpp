#include "StaticBody.h"
#include "Graphics.h"
#include "CollisionInfo.h"

namespace Temporal
{
	StaticBody::~StaticBody()
	{
		delete _collisionInfo;
	}

	void StaticBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().draw(_collisionInfo->getGlobalShape(), Color(0.5f, 0.25f, 0.125f));
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			Shape* shape = const_cast<Shape*>(&_collisionInfo->getGlobalShape());
			message.setParam(shape);
		}
	}
}