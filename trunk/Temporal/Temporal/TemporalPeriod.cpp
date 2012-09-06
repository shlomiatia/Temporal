#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"
#include "Input.h"

namespace Temporal
{
	void PlayerPeriod::changePeriod(Period::Enum period)
	{
		_period = period;
		raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &period));
	}

	void PlayerPeriod::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		}
		else if(message.getID() == MessageID::LEVEL_CREATED)
		{
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &_period));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(Input::get().isQ())
			{
				changePeriod(Period::PAST);
			}
			if(Input::get().isW())
			{
				changePeriod(Period::PRESENT);
			}
			if(Input::get().isE())
			{
				changePeriod(Period::FUTURE);
			}
		}
	}

	void TemporalPeriod::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		}
		else if(message.getID() == MessageID::SET_PERIOD)
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