#ifndef TEMPORALECHO_H
#define TEMPORALECHO_H

#include "Component.h"
#include <vector>

namespace Temporal
{
	class Entity;
	class Serialization;
	typedef std::vector<Serialization*> EchoCollection;
	typedef EchoCollection::const_iterator EchoIterator;
	
	class TemporalEcho : public Component
	{
	public:
		explicit TemporalEcho(Entity* echo) : _echo(echo), _echoReady(false) {}
		~TemporalEcho(void);

		ComponentType::Enum getType(void) const { return ComponentType::TEMPORAL_ECHO; }
		void handleMessage(Message& message);
	private:
		static const float ECHO_READY_TIME_IN_MILLIS;

		void update(float framePeriodInMillis);
		void mergeToTemporalEchoes(void);

		EchoCollection _echoesData;
		Entity* _echo;
		bool _echoReady;
	};
}
#endif