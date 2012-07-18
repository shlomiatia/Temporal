#include "TemporalPeriod.h"
#include "Message.h"

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
			_period = *(Period::Enum*)message.getParam();
		}
	}
}