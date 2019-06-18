#ifndef TEMPORALECHO_H
#define TEMPORALECHO_H

#include "EntitySystem.h"
#include "Timer.h"
#include <vector>
#include "Ids.h"

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
		Hash getType() const { return ComponentsIds::TEMPORAL_ECHO; }
		Component* clone() const { return new TemporalEcho(); }

		Entity& getEcho() { return *_echo; }
		const Entity& getEcho() const { return *_echo; }
		void setEchoReady(bool ready);

		
	private:
		Entity* _echo;
		bool _echoReady;

		void init();
	};

	typedef std::vector<TemporalEcho*> TemporalEchoList;
	typedef TemporalEchoList::const_iterator TemporalEchoIterator;
	
	/**********************************************************************************************
	* Temporal Echo Manager
	*********************************************************************************************/
	class TemporalEchoManager : public Component
	{
	public:
		TemporalEchoManager() : _echoReady(false), _cooldown(true) {}
		~TemporalEchoManager();

		void handleMessage(Message& message);
		Hash getType() const { return ComponentsIds::TEMPORAL_ECHO_MANAGER; }
		Component* clone() const { return new TemporalEchoManager(); }

		
	private:
		static const float ECHO_READY_TIME;
		static const float ECHO_COOLDOWN_TIME;

		TemporalEchoDataList _echoesData;
		TemporalEchoList _echoes;
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