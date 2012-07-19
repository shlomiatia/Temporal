#include "DrawPosition.h"
#include "BaseUtils.h"
#include "Serialization.h"

namespace Temporal
{
	static const NumericPairSerializer OVERRIDE_SERIALIZER("DRW_POS_OVERRIDE");

	void DrawPosition::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_DRAW_POSITION)
		{
			Point* outParam = (Point*)message.getParam();
			if(_override != Point::Zero)
			{
				*outParam = _override;
			}
			else
			{
				const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
				*outParam = position + _offset;
			}
		}
		else if(message.getID() == MessageID::SET_DRAW_POSITION_OVERRIDE)
		{
			_override = *(Point*)message.getParam();
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			OVERRIDE_SERIALIZER.serialize(serialization, _override);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			OVERRIDE_SERIALIZER.deserialize(serialization, _override);
		}
	}

	Component* DrawPosition::clone() const
	{
		return new DrawPosition(_offset);
	}
}