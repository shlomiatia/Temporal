#include "DrawPosition.h"
#include "Serialization.h"
#include "MessageUtils.h"

namespace Temporal
{
	const Hash DrawPosition::TYPE = Hash("draw-position");

	void DrawPosition::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_DRAW_POSITION)
		{
			Vector* outParam = static_cast<Vector*>(message.getParam());
			if(_override != Vector::Zero)
			{
				*outParam = _override;
			}
			else
			{
				const Vector& position = getPosition(*this);
				*outParam = position + _offset;
			}
		}
		else if(message.getID() == MessageID::SET_DRAW_POSITION_OVERRIDE)
		{
			_override = *static_cast<Vector*>(message.getParam());
		}
	}
}