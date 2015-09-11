#ifndef TEMPORALECHO_H
#define TEMPORALECHO_H

#include "EntitySystem.h"
#include "Timer.h"
#include <vector>

namespace Temporal
{
	class Entity;
	class Stream;

	/**********************************************************************************************
	* Temporal Echo Data
	*********************************************************************************************/
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

	/**********************************************************************************************
	* Temporal Echo
	*********************************************************************************************/
	class TemporalEcho : public Component
	{
	public:
		explicit TemporalEcho() : _echo(0), _echoReady(false){}
		~TemporalEcho();

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }
		Component* clone() const { return new TemporalEcho(); }

		Entity& getEcho() { return *_echo; }
		const Entity& getEcho() const { return *_echo; }
		void setEchoReady(bool ready);

		static const Hash TYPE;
	private:
		Entity* _echo;
		bool _echoReady;

		void init();
		HashList getFilter() const;
	};

	typedef std::vector<TemporalEcho*> TemporalEchoList;
	typedef TemporalEchoList::const_iterator TemporalEchoIterator;
	
	/**********************************************************************************************
	* Temporal Echo Manager
	*********************************************************************************************/
	class TemporalEchoManager : public Component
	{
	public:
		TemporalEchoManager() : _echoReady(false), _cooldown(false) {}
		~TemporalEchoManager();

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }
		Component* clone() const { return new TemporalEchoManager(); }

		static const Hash TYPE;
	private:
		static const float ECHO_READY_TIME;
		static const float ECHO_COOLDOWN_TIME;

		TemporalEchoDataList _echoesData;
		TemporalEchoList _ecohoes;
		bool _echoReady;
		bool _cooldown;
		Timer _cooldownTimer;
		Timer _saveTimer;
		Timer _loadTimer;

		void init();
		void update(float framePeriod);
		void updateCooldown(float framePeriod);
		void updateEchoReady(float framePeriod);
		void updateEchoSerilization(float framePeriod);
		void updateEchoDeserialization(float framePeriod);
		void mergeToTemporalEchoes();
		void disableEchos();
	};
}
#endif