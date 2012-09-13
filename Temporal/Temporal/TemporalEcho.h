#ifndef TEMPORALECHO_H
#define TEMPORALECHO_H

#include "EntitySystem.h"
#include <vector>

namespace Temporal
{
	class Entity;
	class MemoryStream;
	typedef std::vector<MemoryStream*> EchoCollection;
	typedef EchoCollection::const_iterator EchoIterator;
	
	class TemporalEcho : public Component
	{
	public:
		explicit TemporalEcho() : _echo(NULL), _echoReady(false) {}
		~TemporalEcho();

		ComponentType::Enum getType() const { return ComponentType::TEMPORAL_ECHO; }
		void handleMessage(Message& message);

		Component* clone() const { return new TemporalEcho(); }

	private:
		static const float ECHO_READY_TIME;

		void init();
		void update(float framePeriod);
		void mergeToTemporalEchoes();

		EchoCollection _echoesData;
		Entity* _echo;
		bool _echoReady;
	};
}
#endif