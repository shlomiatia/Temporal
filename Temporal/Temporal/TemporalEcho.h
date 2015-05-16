#ifndef TEMPORALECHO_H
#define TEMPORALECHO_H

#include "EntitySystem.h"
#include "Timer.h"
#include <vector>

namespace Temporal
{
	class Entity;
	class Stream;

	class TemporalEchoData
	{
	public:
		TemporalEchoData(float time, const Stream* stream) : _time(time), _stream(stream) {};

		float getTime() const { return _time; }
		const Stream* getStream() { return _stream; }
	private:
		float _time;
		const Stream* _stream;

		TemporalEchoData(const TemporalEchoData&);
		TemporalEchoData& operator=(const TemporalEchoData&);
	};

	typedef std::vector<TemporalEchoData*> TemporalEchoDataList;
	typedef TemporalEchoDataList::const_iterator TemporalEchoDataIterator;
	
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
		void disableEcho();
		HashList getFilter() const;

		TemporalEchoDataList _echoesData;
		Entity* _echo;
		bool _echoReady;
		Timer _saveTimer;
		Timer _loadTimer;
	};
}
#endif