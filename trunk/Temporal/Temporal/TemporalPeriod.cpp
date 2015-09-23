#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"
#include "Grid.h"
#include "PhysicsEnums.h"
#include "Utils.h"
#include "Fixture.h"

namespace Temporal
{
	static const int MASK = CollisionCategory::OBSTACLE | CollisionCategory::CHARACTER;
	static const Hash PLAYER_ID("ENT_PLAYER");
	static const Hash TEMPORAL_ACTIVATION_NOTIFICATION_ID("ENT_TEMPORAL_NOTIFICATION");
	static const Hash PARTICLE_EMITTER_ID("particle-emitter");
	const Hash PlayerPeriod::TYPE = Hash("player-period");
	const Hash TemporalPeriod::TYPE = Hash("temporal-period");
	const ColorList PlayerPeriod::COLORS = {
		Color(0.6392156862745098f, 0.2862745098039216f, 0.6431372549019608f),
		Color(0.2470588235294118f, 0.2823529411764706f, 0.8f),
		Color(0.1333333333333333f, 0.6941176470588235f, 0.2980392156862745f),
		Color(1.0f, 0.4980392156862745f, 0.1529411764705882f),
		Color(1.0f, 0.6823529411764706f, 0.7882352941176471f),
		Color(0.7254901960784314f, 0.4784313725490196f, 0.3411764705882353f)
	};

	/**********************************************************************************************
	* Player Period
	*********************************************************************************************/
	const Color& PlayerPeriod::getNextColor()
	{
		if (_colorIterator == COLORS.end())
			abort();
		const Color& color = *_colorIterator;
		_colorIterator++;
		return color;
	}

	void PlayerPeriod::changePeriod(Period::Enum period)
	{
		OBB shape = getShape(*this);
		shape.setRadius(shape.getRadius() - 1.0f);
		FixtureList info = getEntity().getManager().getGameState().getGrid().iterateTiles(shape, MASK, period);
		if (info.size() == 0)
		{
			Hash draggableId = getHashParam(raiseMessage(Message(MessageID::GET_DRAGGABLE)));
			if (draggableId != Hash::INVALID)
			{
				Entity& draggable = *getEntity().getManager().getEntity(draggableId);
				TemporalPeriod& temporalPeriod = *static_cast<TemporalPeriod*>(draggable.get(TemporalPeriod::TYPE));
				shape = getShape(draggable);
				shape.setRadius(shape.getRadius() - 1.0f);
				info = getEntity().getManager().getGameState().getGrid().iterateTiles(shape, MASK, period);
				for (FixtureIterator i = info.begin(); i != info.end(); ++i)
				{
					if (temporalPeriod.getFutureSelfId() != (**i).getEntityId())
					{
						return;
					}
				}
				temporalPeriod.setPeriod(period);
			}
			
			_period = period;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::TEMPORAL_PERIOD_CHANGED, &_period));
			
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
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::TEMPORAL_PERIOD_CHANGED, &_period));
		}
		else if(message.getID() == MessageID::ACTION_TEMPORAL_TRAVEL)
		{
			Period::Enum period = _period == Period::PRESENT ? Period::PAST : Period::PRESENT;
			changePeriod(period);
		}
	}

	/**********************************************************************************************
	* Temporal Period
	*********************************************************************************************/
	void TemporalPeriod::temporalPeriodChanged(Period::Enum period)
	{
		float alpha;
		if (_period == period)
			alpha = 1.0f;
		else
			alpha = 0.1f;
		raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
	}

	void TemporalPeriod::setPeriod(Period::Enum period)
	{
		_period = period;
		raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		Period::Enum playerPeriod = *static_cast<Period::Enum*>(getEntity().getManager().sendMessageToEntity(PLAYER_ID, Message(MessageID::GET_COLLISION_GROUP)));
		temporalPeriodChanged(playerPeriod);

		if (_period == Period::PRESENT)
		{
			if (_createFutureSelf && _futureSelfId)
			{
				killFuture();
			}
		}
		else
		{
			if (_createFutureSelf)
			{
				createFuture();
			}
			else if (_futureSelfId != Hash::INVALID)
			{
				PlayerPeriod& playerPeriod = *static_cast<PlayerPeriod*>(getEntity().getManager().getEntity(PLAYER_ID)->get(PlayerPeriod::TYPE));
				const Color& color = playerPeriod.getNextColor();
				Message message(MessageID::SET_COLOR, const_cast<Color*>(&color));
				raiseMessage(message);
				getEntity().getManager().sendMessageToEntity(_futureSelfId, message);
			}
		}
		
	}

	void TemporalPeriod::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_READY)
		{
			Entity* particleEmitterTemplate = getEntity().getManager().getGameState().getEntityTemplatesManager().get(TEMPORAL_ACTIVATION_NOTIFICATION_ID);
			Component* particleEmitter = particleEmitterTemplate->get(PARTICLE_EMITTER_ID)->clone();
			particleEmitter->setBypassSave(true);
			getEntity().add(particleEmitter);
			setPeriod(_period);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			int period = -1;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &period));
			float alpha = 1.0f;
			raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
		}
		else if (message.getID() == MessageID::TEMPORAL_PERIOD_CHANGED)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());
			temporalPeriodChanged(period);
		}
		else if (message.getID() == MessageID::ACTIVATE)
		{
			Period::Enum playerPeriod = *static_cast<Period::Enum*>(getEntity().getManager().sendMessageToEntity(PLAYER_ID, Message(MessageID::GET_COLLISION_GROUP)));
			if (_period != playerPeriod)
			{
				raiseMessage(Message(MessageID::START_EMITTER));
			}
		}
		else if (message.getID() == MessageID::DIE)
		{
			
			if (_futureSelfId != Hash::INVALID)
			{
				bool t = true;
				getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::DIE, &t));
			}
			void* temporalDeath = message.getParam();
			if (temporalDeath && getBoolParam(temporalDeath))
			{
				raiseMessage(Message(MessageID::START_EMITTER));
				bool f = false;
				raiseMessage(Message(MessageID::SET_BODY_ENABLED, &f));
				raiseMessage(Message(MessageID::SET_VISIBILITY, &f));
				_destroy = true;
			}
		}
		else if (message.getID() == MessageID::EMITTER_FINISHED)
		{
			if (_destroy)
				getEntity().getManager().remove(getEntity().getId());
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			if (_createFutureSelf && _futureSelfId != Hash::INVALID)
			{
				Vector position = getPosition(*this);
				const Vector& futurePosition = getVectorParam(getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::GET_POSITION)));
				if (_previousPosition == futurePosition)
				{
					getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_POSITION, &position));
				}

				_previousPosition = position;
			}
		}
		else if (message.getID() == MessageID::SET_IMPULSE)
		{
			Vector position = getPosition(*this);
			if (_createFutureSelf && _futureSelfId != Hash::INVALID && position != getVectorParam(getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::GET_POSITION))))
			{
				TemporalPeriod& future = *static_cast<TemporalPeriod*>(getEntity().getManager().getEntity(_futureSelfId)->get(TemporalPeriod::TYPE));
				future.setPeriod(Period::PRESENT);
				getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_POSITION, &position));
				getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::START_EMITTER));
			}
		}
	}

	void TemporalPeriod::killFuture()
	{
		bool param = true;
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::DIE, &param));
		_futureSelfId = Hash::INVALID;
	}

	void TemporalPeriod::createFuture()
	{
		_futureSelfId = Hash(Utils::format("%s_PRESENT", getEntity().getId().getString()).c_str());
		Entity* existing = getEntity().getManager().getEntity(_futureSelfId);
		if (existing)
			getEntity().getManager().remove(_futureSelfId);
		Entity* clone = getEntity().clone();
		clone->remove(PARTICLE_EMITTER_ID);
		clone->setBypassSave(true);
		TemporalPeriod& period = *static_cast<TemporalPeriod*>(clone->get(TemporalPeriod::TYPE));
		period._period = Period::PRESENT;
		period._futureSelfId = Hash::INVALID;
		period._createFutureSelf = true;
		clone->setId(_futureSelfId);
		getEntity().getManager().add(clone);
	}
}