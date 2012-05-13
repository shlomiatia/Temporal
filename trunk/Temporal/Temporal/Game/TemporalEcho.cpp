#include "TemporalEcho.h"
#include "Message.h"
#include "Entity.h"
#include <Temporal\Base\Serialization.h>

namespace Temporal
{
	const float TemporalEcho::ECHO_READY_TIME_IN_MILLIS = 5000.0f;

	void TemporalEcho::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			if(!_echoReady)
			{
				float framePeriodInMillis = *(float*)message.getParam();
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
		else if(message.getID() == MessageID::MERGE_TO_TEMPORAL_ECHOES)
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
		else if(_echoReady && (message.getID() == MessageID::DRAW || message.getID() == MessageID::DEBUG_DRAW)) 
		{
			_echo->handleMessage(message);
		}
	}
}