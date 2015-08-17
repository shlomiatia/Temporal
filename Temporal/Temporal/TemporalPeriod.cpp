#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"
#include "Keyboard.h"
#include "Grid.h"
#include "PhysicsEnums.h"

namespace Temporal
{
	Hash PLAYER_ID("ENT_PLAYER");
	const Hash PlayerPeriod::TYPE = Hash("player-period");
	const Hash TemporalPeriod::TYPE = Hash("temporal-period");

	void PlayerPeriod::changePeriod(Period::Enum period)
	{
		OBB shape = getShape(*this);
		shape.setRadius(shape.getRadius() - 1.0f);
		FixtureList info = getEntity().getManager().getGameState().getGrid().iterateTiles(shape, CollisionCategory::OBSTACLE, period);
		if (info.size() == 0)
		{
			_period = period;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::SET_PLAYER_PERIOD, &period));
		}
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
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::SET_PLAYER_PERIOD, &_period));
		}
		else if(message.getID() == MessageID::ACTION_TEMPORAL_TRAVEL)
		{
			Period::Enum period = _period == Period::PAST ? Period::PRESENT : Period::PAST;
			changePeriod(period);
		}
	}

	void TemporalPeriod::setPlayerPeriod(Period::Enum period)
	{
		float alpha;
		if (_period == period || _period == -1)
			alpha = 1.0f;
		else
			alpha = 0.2f;
		raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
	}

	void TemporalPeriod::setPeriod(Period::Enum period)
	{
		_period = period;
		raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		Period::Enum playerPeriod = *static_cast<Period::Enum*>(getEntity().getManager().sendMessageToEntity(PLAYER_ID, Message(MessageID::GET_COLLISION_GROUP)));
		setPlayerPeriod(playerPeriod);
	}

	void TemporalPeriod::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			setPeriod(_period);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			int period = -1;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &period));
			float alpha = 1.0f;
			raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
		}
		else if (message.getID() == MessageID::SET_PLAYER_PERIOD)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());
			setPlayerPeriod(period);
		}
	}


}