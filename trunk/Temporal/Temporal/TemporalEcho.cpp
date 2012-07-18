#include "TemporalEcho.h"
#include "Message.h"
#include "Entity.h"
#include "Serialization.h"
#include "Color.h"

namespace Temporal
{
	const float TemporalEcho::ECHO_READY_TIME_IN_MILLIS = 4500.0f;

	TemporalEcho::~TemporalEcho()
	{
		delete _echo;
		for(EchoIterator i = _echoesData.begin(); i != _echoesData.end(); ++i)
		{
			delete *i;
		}
	}

	void TemporalEcho::update(float framePeriodInMillis)
	{
		if(!_echoReady)
		{
			float echoLifetimeInMillis = _echoesData.size() * framePeriodInMillis;
			if(echoLifetimeInMillis > ECHO_READY_TIME_IN_MILLIS)
				_echoReady = true;
		}
		if(_echoReady)
		{
			EchoIterator first = _echoesData.begin();
			Serialization* deserialization = *first;
			_echo->handleMessage(Message(MessageID::DESERIALIZE, deserialization));
			delete deserialization;
			_echoesData.erase(first);
		}
		Serialization* serialization = new Serialization();
		sendMessageToOwner(Message(MessageID::SERIALIZE, serialization));
		_echoesData.push_back(serialization);
	}

	void TemporalEcho::mergeToTemporalEchoes()
	{
		if(_echoReady)
		{
			EchoIterator first = _echoesData.begin();
			Serialization* deserialization = *first;
			sendMessageToOwner(Message(MessageID::DESERIALIZE, deserialization));
			for(EchoIterator i = _echoesData.begin(); i != _echoesData.end(); )
			{
				delete *i;
				i = _echoesData.erase(i);
			}
			_echoReady = false;
		}
	}

	void TemporalEcho::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(float*)message.getParam();
			update(framePeriodInMillis);
			if(_echoReady)
				_echo->handleMessage(message);
		}
		else if(message.getID() == MessageID::MERGE_TO_TEMPORAL_ECHOES)
		{
			mergeToTemporalEchoes();
		}
		else if(message.getID() == MessageID::DRAW) 
		{
			if(_echoReady)
				_echo->handleMessage(message);
		}
		else if(message.getID() == MessageID::ENTITY_CREATED)
		{
			_echo->handleMessage(Message(MessageID::SET_COLOR, &Color(1.0f, 1.0f, 1.0f, 0.1f)));
		}
	}
}