#ifndef TEMPORALECHO_H
#define TEMPORALECHO_H

#include "EntitySystem.h"
#include <vector>

namespace Temporal
{
	class Entity;
	class Stream;
	typedef std::vector<Stream*> EchoCollection;
	typedef EchoCollection::const_iterator EchoIterator;
	
	class TemporalEcho : public Component
	{
	public:
		explicit TemporalEcho() : _echo(0), _echoReady(false) {}
		~TemporalEcho();

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }

		Component* clone() const { return new TemporalEcho(); }

		static const Hash TYPE;
	private:
		static const float ECHO_READY_TIME;

		void init();
		void update(float framePeriod);
		void mergeToTemporalEchoes();
		HashCollection getFilter() const;

		EchoCollection _echoesData;
		Entity* _echo;
		bool _echoReady;
	};
}
#endif