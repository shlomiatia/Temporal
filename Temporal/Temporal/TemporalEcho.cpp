#include "TemporalEcho.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Color.h"
#include "MessageUtils.h"

namespace Temporal
{
	const static ComponentType::Enum filter = ComponentType::TRANSFORM | ComponentType::DRAW_POSITION | ComponentType::RENDERER | ComponentType::ANIMATOR;
	const float TemporalEcho::ECHO_READY_TIME = 4.5f;

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
			MemoryStream* deserialization = *first;
			MemoryDeserializer deserializer(deserialization);
			deserializer.serialize("entity", _echo);
			delete deserialization;
			_echoesData.erase(first);
		}
		MemoryStream* serialization = new MemoryStream();
		MemorySerializer serializer(serialization);
		serializer.serialize("entity", getEntity());
		_echoesData.push_back(serialization);
	}

	void TemporalEcho::mergeToTemporalEchoes()
	{
		if(_echoReady)
		{
			EchoIterator first = _echoesData.begin();
			MemoryStream* deserialization = *first;
			MemoryDeserializer deserializer(deserialization);
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
		/*const Component* transform = getEntity().get(ComponentType::TRANSFORM);
		const Component* drawPosition = getEntity().get(ComponentType::DRAW_POSITION);
		const Component* renderer = getEntity().get(ComponentType::RENDERER);
		const Component* animator = getEntity().get(ComponentType::ANIMATOR);
		
		_echo = new Entity();
		if(transform != NULL)
			_echo->add(transform->clone());
		if(drawPosition != NULL)
			_echo->add(drawPosition->clone());
		if(renderer != NULL)
			_echo->add(renderer->clone());
		if(animator != NULL)
			_echo->add(animator->clone());*/
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
				_echo->handleMessage(message, filter);
		}
		else if(message.getID() == MessageID::MERGE_TO_TEMPORAL_ECHOES)
		{
			mergeToTemporalEchoes();
		}
		else if(message.getID() == MessageID::DRAW) 
		{
			if(_echoReady)
				_echo->handleMessage(message, filter);
		}
		else if(message.getID() == MessageID::ENTITY_PRE_INIT)
		{
			init();
		}
		else if(message.getID() == MessageID::ENTITY_INIT)
		{
			_echo->handleMessage(message, filter);
		}
		else if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_echo->handleMessage(message, filter);
		}
	}
}