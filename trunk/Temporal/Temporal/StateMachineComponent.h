#ifndef STATEMACHINECOMPONENT_H
#define STATEMACHINECOMPONENT_H

#include "Timer.h"
#include "Hash.h"
#include "EntitySystem.h"
#include <unordered_map>

namespace Temporal
{
	class Message;
	class StateMachineComponent;

	class ComponentState
	{
	public:
		ComponentState() {};
		virtual ~ComponentState() {};

		void setStateMachine(StateMachineComponent* stateMachine) { _stateMachine = stateMachine; }

		virtual void enter() const {}
		virtual void exit() const {}
		virtual void handleMessage(Message& message) const = 0;

	protected:
		StateMachineComponent* _stateMachine;

	private:
		ComponentState(const ComponentState&);
		ComponentState& operator=(const ComponentState&);
	};

	typedef std::unordered_map<Hash, ComponentState*> StateCollection;
	typedef StateCollection::const_iterator StateIterator;

	class StateMachineComponent : public Component
	{
	public:
		explicit StateMachineComponent(StateCollection states, const char* prefix);
		virtual ~StateMachineComponent();

		void changeState(Hash stateID);
		virtual void handleMessage(Message& message);

		// Temp flags. Used when some messages combinations make something happen, or if during update we need to check if someting didn't happen. 
		// This flags are cleared every update and state change
		bool getFrameFlag1() const { return _frameFlag1; }
		void setFrameFlag1(bool value) { _frameFlag1 = value; }
		bool getFrameFlag2() const { return _frameFlag2; }
		void setFrameFlag2(bool value) { _frameFlag2 = value; }
		bool getStateFlag() const { return _stateFlag; }
		void setStateFlag(bool value) { _stateFlag = value; }
		bool getStateMachineFlag() const { return _stateMachineFlag; }
		void setStateMachineFlag(bool value) { _stateMachineFlag = value; }

		// Record amount of time spent in a single state. Expose non const object to allow state to record sub periods
		Timer& getTimer() { return _timer; }

	protected:
		virtual Hash getInitialState() const = 0;

	private:
		StateCollection _states;

		// Persistent state
		const ComponentState* _currentState;
		Hash _currentStateID;
		Timer _timer;

		// Temp state
		bool _stateMachineFlag;
		bool _stateFlag;
		bool _frameFlag1;
		bool _frameFlag2;

		void resetFrameFlags() { _frameFlag1 = _frameFlag2 = false; }
		void setState(Hash stateID);

		StateMachineComponent(const StateMachineComponent&);
		StateMachineComponent& operator=(const StateMachineComponent&);

		friend class SerializationAccess;
	};
}

#endif