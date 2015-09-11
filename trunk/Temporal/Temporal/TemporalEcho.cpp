#include "TemporalEcho.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Color.h"
#include "MessageUtils.h"
#include "Utils.h"

namespace Temporal
{
	static const Hash PLAYER_ID("ENT_PLAYER");
	const Hash TemporalEchoManager::TYPE = Hash("temporal-echo-manager");
	const float TemporalEchoManager::ECHO_READY_TIME = 1.0f;
	const float TemporalEchoManager::ECHO_COOLDOWN_TIME = 1.0f;
	const Hash TemporalEcho::TYPE = Hash("temporal-echo");
	
	const Hash TRANSFORM_TYPE = Hash("transform");
	const Hash RENDERER_TYPE = Hash("renderer");
	const Hash ANIMATOR_TYPE = Hash("animator");

	/**********************************************************************************************
	* Temporal Echo
	*********************************************************************************************/
	HashList TemporalEcho::getFilter() const
	{
		HashList result;
		result.push_back(TRANSFORM_TYPE);
		result.push_back(RENDERER_TYPE);
		result.push_back(ANIMATOR_TYPE);
		return result;
	}

	TemporalEcho::~TemporalEcho()
	{
		delete _echo;
	}

	void TemporalEcho::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::UPDATE)
		{
			if(_echoReady)
				_echo->handleMessage(message, &getFilter());
		}
		else if (message.getID() == MessageID::ENTITY_PRE_INIT)
		{
			init();
		}
		else if (message.getID() == MessageID::ENTITY_INIT)
		{
			_echo->handleMessage(message, &getFilter());
		}
		else if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_echo->handleMessage(message, &getFilter());
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			_echo->handleMessage(message, &getFilter());
		}
	}

	void TemporalEcho::init()
	{
		_echo = getEntity().clone();
		_echo->init(&getEntity().getManager());
		std::string idString = Utils::format("%s_TEMPORAL_ECHO", getEntity().getId().getString());
		_echo->setId(Hash(idString.c_str()));
	}

	void TemporalEcho::setEchoReady(bool ready)
	{
		_echoReady = ready;
		float alpha = _echoReady ? 0.1f : 0.0f;
		_echo->handleMessage(Message(MessageID::SET_ALPHA, &alpha));
	}

	/**********************************************************************************************
	* Temporal Echo Manager
	*********************************************************************************************/
	TemporalEchoManager::~TemporalEchoManager()
	{
		for (TemporalEchoDataIterator i = _echoesData.begin(); i != _echoesData.end(); ++i)
		{
			delete *i;
		}
	}

	void TemporalEchoManager::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::LEVEL_INIT)
		{
			init();
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
		else if (message.getID() == MessageID::ACTION_TEMPORAL_TRAVEL)
		{
			mergeToTemporalEchoes();
		}
		else if(message.getID() == MessageID::LOAD)
		{
			disableEchos();
		}
	}

	void TemporalEchoManager::init()
	{
		HashEntityMap& entities = getEntity().getManager().getEntities();
		Hash temporalEchoFilter = Hash("dynamic-body");
		for (HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
		{
			Entity& entity = *i->second;
			if (entity.getId() != PLAYER_ID && entity.get(temporalEchoFilter))
			{
				TemporalEcho* echo = new TemporalEcho();
				echo->setBypassSave(true);
				entity.add(echo);
				_ecohoes.push_back(echo);
			}
		}
		disableEchos();
	}

	void TemporalEchoManager::update(float framePeriod)
	{
		updateCooldown(framePeriod);
		if (!_cooldown)
		{
			updateEchoReady(framePeriod);
			if (_echoReady)
			{
				updateEchoDeserialization(framePeriod);
			}
			updateEchoSerilization(framePeriod);
		}
	}

	void TemporalEchoManager::updateCooldown(float framePeriod)
	{
		if (_cooldown)
		{
			_cooldownTimer.update(framePeriod);
			if (_cooldownTimer.getElapsedTime() > ECHO_COOLDOWN_TIME)
			{
				_cooldown = false;
			}
		}
	}

	void TemporalEchoManager::updateEchoReady(float framePeriod)
	{
		if (!_echoReady)
		{
			if (_saveTimer.getElapsedTime() > ECHO_READY_TIME)
			{
				_echoReady = true;
				_loadTimer.reset();
			}
		}
	}

	void TemporalEchoManager::updateEchoSerilization(float framePeriod)
	{
		_saveTimer.update(framePeriod);
		Stream* serialization = new MemoryStream();
		BinarySerializer serializer(serialization);
		for (TemporalEchoIterator i = _ecohoes.begin(); i != _ecohoes.end(); ++i)
		{
			TemporalEcho& echo = **i;
			serializer.serialize("entity", echo.getEntity());
		}
		TemporalEchoData* data = new TemporalEchoData(_saveTimer.getElapsedTime(), serialization);
		_echoesData.push_back(data);
	}

	void TemporalEchoManager::updateEchoDeserialization(float framePeriod)
	{
		_loadTimer.update(framePeriod);
		TemporalEchoDataIterator first = _echoesData.begin();
		if ((**first).getTime() <= _loadTimer.getElapsedTime())
		{
			const Stream* deserialization = (**first).getStream();
			BinaryDeserializer deserializer(deserialization);
			
			for (TemporalEchoIterator i = _ecohoes.begin(); i != _ecohoes.end(); ++i)
			{
				TemporalEcho& echo = **i;
				deserializer.serialize("entity", echo.getEcho());
				echo.setEchoReady(true);
			}
			delete deserialization;
			_echoesData.erase(first);
		}
	}

	void TemporalEchoManager::mergeToTemporalEchoes()
	{
		if (_echoReady)
		{
			TemporalEchoDataIterator first = _echoesData.begin();
			const Stream* deserialization = (**first).getStream();
			BinaryDeserializer deserializer(deserialization);
			for (TemporalEchoIterator i = _ecohoes.begin(); i != _ecohoes.end(); ++i)
			{
				Entity& entity = (**i).getEntity();
				entity.handleMessage(Message(MessageID::PRE_LOAD));
				deserializer.serialize("entity", entity);
				entity.handleMessage(Message(MessageID::POST_LOAD));
			}
			disableEchos();
			_cooldown = true;
			_cooldownTimer.reset();
		}
	}

	void TemporalEchoManager::disableEchos()
	{
		for (TemporalEchoDataIterator i = _echoesData.begin(); i != _echoesData.end();)
		{
			delete *i;
			i = _echoesData.erase(i);
		}
		_echoReady = false;
		_saveTimer.reset();
		for (TemporalEchoIterator i = _ecohoes.begin(); i != _ecohoes.end(); ++i)
		{
			TemporalEcho& echo = **i;
			echo.setEchoReady(false);
		}
	}
}