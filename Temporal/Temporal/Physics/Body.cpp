#include "Body.h"
#include <Temporal/Graphics/Graphics.h>
#include <assert.h>

namespace Temporal
{
	Body::Body(const Vector& size)
		: _size(size)
	{
		assert(_size.getWidth() > 0.0f);
		assert(_size.getHeight() > 0.0f);
	}

	Rect Body::getBounds(void) const
	{
		const Vector& position = sendQueryMessageToOwner<Vector>(Message(MessageID::GET_POSITION));
		return Rect(position, _size);
	}

	void Body::handleMessage( Message& message )
	{
		if(message.getID() == MessageID::GET_SIZE)
		{
			message.setParam(&_size);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			Graphics::get().drawRect(getBounds());
		}
	}

}