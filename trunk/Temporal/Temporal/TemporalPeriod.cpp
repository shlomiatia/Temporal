#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"

namespace Temporal
{
	void TemporalPeriod::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_PERIOD)
		{
			message.setParam(&_period);
		}
		else if(message.getID() == MessageID::ENTITY_CREATED)
		{
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		}
		else if(message.getID() == MessageID::SET_PERIOD)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());
			_period = period;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		}
		else if(message.getID() == MessageID::SET_CURRENT_PERIOD)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());

			float alpha;
			if(_period == period)
				alpha = 1.0f;
			else
				alpha = 0.2f;
			raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
		}
	}


}