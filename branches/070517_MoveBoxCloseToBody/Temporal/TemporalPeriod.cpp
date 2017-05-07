#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"
#include "Grid.h"
#include "PhysicsEnums.h"
#include "Utils.h"
#include "Transform.h"
#include "PlayerPeriod.h"
#include "TemporalNotification.h"

namespace Temporal
{
	static const int MASK = CollisionCategory::OBSTACLE | CollisionCategory::CHARACTER;
	static const Hash PLAYER_ID("ENT_PLAYER");
	static const Hash TEMPORAL_ACTIVATION_NOTIFICATION_ID("ENT_TEMPORAL_NOTIFICATION");
	static const Hash PARTICLE_EMITTER_ID("particle-emitter");

	void TemporalPeriod::temporalPeriodChanged(Period::Enum period)
	{
		float alpha;
		if (_period == period)
			alpha = 1.0f;
		else
			alpha = 0.3f;
		raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
	}

	void TemporalPeriod::setPeriod(Period::Enum period)
	{
		_period = period;
		raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		Period::Enum playerPeriod = *static_cast<Period::Enum*>(getEntity().getManager().sendMessageToEntity(PLAYER_ID, Message(MessageID::GET_COLLISION_GROUP)));
		temporalPeriodChanged(playerPeriod);

		if (_period == Period::PAST &&  _createFutureSelf && _futureSelfId == Hash::INVALID)
		{
			createFuture();
		}
		else if (_period == Period::PRESENT && _createFutureSelf && _futureSelfId != Hash::INVALID)
		{
			destroyFuture();
		}
	}

	void TemporalPeriod::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::GET_TEMPORAL_FUTURE_ID)
		{
			message.setParam(&_futureSelfId);
		}
		else if (message.getID() == MessageID::ENTITY_READY)
		{
			entityReady();
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			int period = -1;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &period));
			float alpha = 1.0f;
			raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
		}
		else if (message.getID() == MessageID::SET_TEMPORAL_PERIOD)
		{
			Period::Enum period = *static_cast<Period::Enum*>(message.getParam());
			setPeriod(period);
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
			void* temporalDeathParam = message.getParam();
			bool temporalDeath = temporalDeathParam && getBoolParam(temporalDeathParam);
			die(temporalDeath);
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if (message.getID() == MessageID::SET_IMPULSE)
		{
			setImpulse();
		}
	}

	void TemporalPeriod::entityReady()
	{
		if (_editorFutureSelfId != Hash::INVALID)
			_futureSelfId = _editorFutureSelfId;
		addParticleEmitter(getEntity());
		setPeriod(_period);
		if (_period == Period::PAST && !_createFutureSelf && _futureSelfId != Hash::INVALID)
		{
			PlayerPeriod& playerPeriod = *static_cast<PlayerPeriod*>(getEntity().getManager().getEntity(PLAYER_ID)->get(ComponentsIds::PLAYER_PERIOD));
			const Color& color = playerPeriod.getNextColor();
			Message message(MessageID::SET_COLOR, const_cast<Color*>(&color));
			raiseMessage(message);
			getEntity().getManager().sendMessageToEntity(_futureSelfId, message);
		}
	}

	void TemporalPeriod::die(bool temporalDeath)
	{
		setCreateFutureSelf(true);
		setSyncFutureSelf(true);
		if (_futureSelfId != Hash::INVALID)
		{
			destroyFuture();
		}
		if (_period == Period::PAST &&  _createFutureSelf && _futureSelfId == Hash::INVALID)
		{
			createFuture();
		}
		if (temporalDeath)
		{
			Hash id = Hash(Utils::format("%s_NOTIFICATION", getEntity().getId().getString()).c_str());
			Entity* existing = getEntity().getManager().getEntity(id);
			if (existing)
				getEntity().getManager().remove(id);
			Entity* entity = new Entity();
			entity->add(new Transform());
			entity->add(new TemporalNotification());
			addParticleEmitter(*entity);
			entity->setId(id);
			getEntity().getManager().add(entity);
			Vector position = getPosition(getEntity());
			getEntity().getManager().sendMessageToEntity(id, Message(MessageID::SET_POSITION, &position));

			getEntity().getManager().remove(getEntity().getId());
		}
	}

	void TemporalPeriod::update()
	{
		if (_futureSelfId != Hash::INVALID && _syncFutureSelf && _isMoving)
		{
			Vector presentPosition = getPosition(*this);
			if (_previousFramePosition == presentPosition)
			{
				endMovement(presentPosition);
			}
			_previousFramePosition = presentPosition;
		}
	}

	void TemporalPeriod::endMovement(Vector presentPosition)
	{
		bool isFutureBlocked = iterateTiles(getEntity(), MASK, Period::PRESENT, _futureSelfId);
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_BODY_ENABLED, &isFutureBlocked));
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_VISIBILITY, &isFutureBlocked));
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_POSITION, &presentPosition));
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::START_EMITTER));
		_isMoving = false;
	}

	void TemporalPeriod::startMovement()
	{
		_isMoving = true;
		bool falseValue = false;
		TemporalPeriod& future = *static_cast<TemporalPeriod*>(getEntity().getManager().getEntity(_futureSelfId)->get(ComponentsIds::TEMPORAL_PERIOD));
		future.setPeriod(Period::PRESENT);
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_BODY_ENABLED, &falseValue));
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::SET_VISIBILITY, &falseValue));
		getEntity().getManager().sendMessageToEntity(_futureSelfId, Message(MessageID::START_EMITTER));
	}

	void TemporalPeriod::setImpulse()
	{
		Vector position = getPosition(*this);
		if (_syncFutureSelf && _futureSelfId != Hash::INVALID && !_isMoving)
		{
			startMovement();
		}
	}

	void TemporalPeriod::addParticleEmitter(Entity& entity)
	{
		Entity* particleEmitterTemplate = getEntity().getManager().getGameState().getEntityTemplatesManager().get(TEMPORAL_ACTIVATION_NOTIFICATION_ID);
		Component* particleEmitter = particleEmitterTemplate->get(PARTICLE_EMITTER_ID)->clone();
		particleEmitter->setBypassSave(true);
		entity.add(particleEmitter);
	}

	void TemporalPeriod::destroyFuture()
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
		TemporalPeriod& period = *static_cast<TemporalPeriod*>(clone->get(ComponentsIds::TEMPORAL_PERIOD));
		period._period = Period::PRESENT;
		period._futureSelfId = Hash::INVALID;
		period._createFutureSelf = true;
		period._syncFutureSelf = _syncFutureSelf;
		clone->setId(_futureSelfId);
		getEntity().getManager().add(clone);
	}
}