#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"

namespace Temporal
{
	void TemporalPeriod::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::LEVEL_CREATED)
		{
			if(_isPlayer)
				EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &_period));
		}
		else if(message.getID() == MessageID::GET_PERIOD)
		{
			message.setParam(&_period);
		}
		else if(message.getID() == MessageID::SET_PERIOD)
		{
			Period::Enum playerPeriod = *(Period::Enum*)message.getParam();
			if(_isPlayer)
				_period = playerPeriod;
			else if(_period  == playerPeriod)
				sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::White));
			else
				sendMessageToOwner(Message(MessageID::SET_COLOR, &Color(1.0f, 1.0f, 1.0f, 0.1f)));

		}
	}
}