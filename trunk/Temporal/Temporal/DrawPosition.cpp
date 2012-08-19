#include "DrawPosition.h"
#include "BaseUtils.h"
#include "Serialization.h"
#include "MessageUtils.h"

namespace Temporal
{
	static const VectorSerializer OVERRIDE_SERIALIZER("DRW_POS_OVERRIDE");

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
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			OVERRIDE_SERIALIZER.serialize(serialization, _override);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			OVERRIDE_SERIALIZER.deserialize(serialization, _override);
		}
	}

	Component* DrawPosition::clone() const
	{
		return new DrawPosition(_offset);
	}
}