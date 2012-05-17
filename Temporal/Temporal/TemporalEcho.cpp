#include "TemporalEcho.h"
#include "Message.h"
#include "Entity.h"
#include "Serialization.h"

namespace Temporal
{
	const float TemporalEcho::ECHO_READY_TIME_IN_MILLIS = 5000.0f;

	// TODO: Make it more effective. Also do this gradually
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

	void TemporalEcho::mergeToTemporalEchoes(void)
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
			
		}
		else if(message.getID() == MessageID::MERGE_TO_TEMPORAL_ECHOES)
		{
			mergeToTemporalEchoes();
		}
		else if(_echoReady && message.getID() == MessageID::DRAW) 
		{
			_echo->handleMessage(message);
		}
	}
}