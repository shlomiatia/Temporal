#include "TemporalEcho.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Color.h"
#include "MessageUtils.h"
#include "Utils.h"

namespace Temporal
{
	const Hash TemporalEcho::TYPE = Hash("temporal-echo");
	const Hash TRANSFORM_TYPE = Hash("transform");
	const Hash RENDERER_TYPE = Hash("renderer");
	const Hash ANIMATOR_TYPE = Hash("animator");
	const float TemporalEcho::ECHO_READY_TIME = 4.5f;

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
		for(EchoIterator i = _echoesData.begin(); i != _echoesData.end(); ++i)
		{
			delete *i;
		}
	}

	void TemporalEcho::update(float framePeriod)
	{
		if(!_echoReady)
		{
			float echoLifetime = _echoesData.size() * framePeriod;
			if(echoLifetime > ECHO_READY_TIME)
				_echoReady = true;
		}
		if(_echoReady)
		{
			EchoIterator first = _echoesData.begin();
			Stream* deserialization = *first;
			BinaryDeserializer deserializer(deserialization);
			deserializer.serialize("entity", _echo);
			delete deserialization;
			_echoesData.erase(first);
			float alpha = 0.2f;
			_echo->handleMessage(Message(MessageID::SET_ALPHA, &alpha));
		}
		Stream* serialization = new MemoryStream();
		BinarySerializer serializer(serialization);
		serializer.serialize("entity", getEntity());
		_echoesData.push_back(serialization);
	}

	void TemporalEcho::disableEcho()
	{
		float alpha = 0.0f;
		_echo->handleMessage(Message(MessageID::SET_ALPHA, &alpha));
		_echoReady = false;
	}

	void TemporalEcho::mergeToTemporalEchoes()
	{
		if(_echoReady)
		{
			getEntity().handleMessage(Message(MessageID::PRE_LOAD));
			EchoIterator first = _echoesData.begin();
			Stream* deserialization = *first;
			BinaryDeserializer deserializer(deserialization);
			deserializer.serialize("entity", getEntity());
			for(EchoIterator i = _echoesData.begin(); i != _echoesData.end(); )
			{
				delete *i;
				i = _echoesData.erase(i);
			}
			disableEcho();
			getEntity().handleMessage(Message(MessageID::POST_LOAD));
		}
	}

	void TemporalEcho::init()
	{
		_echo = getEntity().clone();
		std::string idString = Utils::format("%s_TEMPORAL_ECHO", getEntity().getId().getString());
		_echo->setId(Hash(idString.c_str()));
		disableEcho();
	}

	void TemporalEcho::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
			if(_echoReady)
				_echo->handleMessage(message, &getFilter());
		}
		else if(message.getID() == MessageID::MERGE_TO_TEMPORAL_ECHOES)
		{
			mergeToTemporalEchoes();
		}
		else if(message.getID() == MessageID::LOAD)
		{
			for(EchoIterator i = _echoesData.begin(); i != _echoesData.end(); )
			{
				delete *i;
				i = _echoesData.erase(i);
			}
			disableEcho();
		}
		else if(message.getID() == MessageID::ENTITY_PRE_INIT)
		{
			init();
		}
		else if(message.getID() == MessageID::ENTITY_INIT)
		{
			_echo->handleMessage(message, &getFilter());
		}
		else if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_echo->handleMessage(message, &getFilter());
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			_echo->handleMessage(message, &getFilter());
		}
	}
}