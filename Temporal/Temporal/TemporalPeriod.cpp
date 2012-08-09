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
		else if(message.getID() == MessageID::SET_PERIOD)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());
			_period = period;
		}
		else if(message.getID() == MessageID::SET_CURRENT_PERIOD)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());

			if(_period  == period)
				sendMessageToOwner(Message(MessageID::SET_COLOR, const_cast<Color*>(&Color::White)));
			else
				sendMessageToOwner(Message(MessageID::SET_COLOR, &Color(1.0f, 1.0f, 1.0f, 0.1f)));
		}
	}


}