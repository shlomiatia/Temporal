#include "TemporalEcho.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Color.h"
#include "MessageUtils.h"
#include "Transform.h"
#include "Renderer.h"
#include "DrawPosition.h"
#include "Animator.h"

namespace Temporal
{
	const Hash TemporalEcho::TYPE = Hash("temporal-echo");
	const float TemporalEcho::ECHO_READY_TIME = 4.5f;

	HashCollection TemporalEcho::getFilter() const
	{
		HashCollection result;
		result.push_back(Transform::TYPE);
		result.push_back(DrawPosition::TYPE);
		result.push_back(Renderer::TYPE);
		result.push_back(Animator::TYPE);
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
		}
		Stream* serialization = new MemoryStream();
		BinarySerializer serializer(serialization);
		serializer.serialize("entity", getEntity());
		_echoesData.push_back(serialization);
	}

	void TemporalEcho::mergeToTemporalEchoes()
	{
		if(_echoReady)
		{
			EchoIterator first = _echoesData.begin();
			Stream* deserialization = *first;
			BinaryDeserializer deserializer(deserialization);
			deserializer.serialize("entity", getEntity());
			for(EchoIterator i = _echoesData.begin(); i != _echoesData.end(); )
			{
				delete *i;
				i = _echoesData.erase(i);
			}
			_echoReady = false;
		}
	}

	void TemporalEcho::init()
	{
		_echo = getEntity().clone();
		float alpha = 0.2f;
		_echo->handleMessage(Message(MessageID::SET_ALPHA, &alpha));
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
			_echoReady = false;
		}
		else if(message.getID() == MessageID::DRAW) 
		{
			if(_echoReady)
				_echo->handleMessage(message, &getFilter());
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
	}
}