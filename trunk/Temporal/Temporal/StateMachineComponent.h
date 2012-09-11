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
		bool getTempFlag1() const { return _tempFlag1; }
		void setTempFlag1(bool value) { _tempFlag1 = value; }
		bool getTempFlag2() const { return _tempFlag2; }
		void setTempFlag2(bool value) { _tempFlag2 = value; }
		bool getTempFlag3() const { return _tempFlag3; }
		void setTempFlag3(bool value) { _tempFlag3 = value; }

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
		bool _tempFlag1;
		bool _tempFlag2;
		bool _tempFlag3;

		void resetTempState() { _tempFlag1 = _tempFlag2 = _tempFlag3 = false; }
		void setState(Hash stateID);

		StateMachineComponent(const StateMachineComponent&);
		StateMachineComponent& operator=(const StateMachineComponent&);
	};
}

#endif