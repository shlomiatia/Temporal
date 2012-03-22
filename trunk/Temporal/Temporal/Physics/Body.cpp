#include "Body.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>
#include <assert.h>

namespace Temporal
{
	Body::Body(const Size& size)
		: _size(size)
	{
		assert(_size.getWidth() > 0.0f);
		assert(_size.getHeight() > 0.0f);
	}

	Rect Body::getBounds(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		return Rect(position, _size);
	}

	void Body::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SIZE)
		{
			message.setParam(&_size);
		}
		else if(message.getID() == MessageID::GET_BOUNDS)
		{
			Rect* outParam = (Rect*)message.getParam();
			*outParam = getBounds();
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().drawRect(getBounds());
		}
	}

}