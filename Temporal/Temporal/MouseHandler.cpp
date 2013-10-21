#include "MouseHandler.h"
#include "Shapes.h"
#include "ShapeOperations.h"
#include "MessageUtils.h"

namespace Temporal
{
	const Hash MouseHandler::TYPE = Hash("mouse-handler");

	void MouseHandler::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::PREVIEW_MOUSE_DOWN)
		{
			const YABP& shape = *static_cast<YABP*>(raiseMessage(Message(MessageID::GET_SHAPE)));
			const MouseParams& params = getMouseParams(message.getParam());
			if(intersects(shape, params.getPosition()))
			{
				_isClick = true;
				_isDown= true;
				raiseMessage(Message(MessageID::MOUSE_DOWN, message.getParam()));
			}
		}
		else if(message.getID() == MessageID::PREVIEW_MOUSE_UP)
		{
			if(_isClick)
				raiseMessage(Message(MessageID::MOUSE_CLICK, message.getParam()));
			if(_isDown)
				raiseMessage(Message(MessageID::MOUSE_UP, message.getParam()));
			_isClick = false;
			_isDown = false;
		}
		else if(message.getID() == MessageID::PREVIEW_MOUSE_MOVE)
		{
			_isClick= false;
		}
	}
}
