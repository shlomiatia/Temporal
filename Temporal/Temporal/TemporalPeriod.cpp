#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"
#include "Keyboard.h"

namespace Temporal
{
	const Hash PlayerPeriod::TYPE = Hash("player-period");
	const Hash TemporalPeriod::TYPE = Hash("temporal-period");

	void PlayerPeriod::changePeriod(Period::Enum period)
	{
		_period = period;
		raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		
		getEntity().getManager().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &period));
	}

	void PlayerPeriod::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if(message.getID() == MessageID::LEVEL_INIT)
		{
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &_period));
		}
		else if(message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if(key == Key::D1)
			{
				changePeriod(Period::PAST);
			}
			if(key == Key::D2)
			{
				changePeriod(Period::PRESENT);
			}
			if(key == Key::D3)
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