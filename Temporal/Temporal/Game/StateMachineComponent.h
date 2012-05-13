#ifndef STATEMACHINECOMPONENT_H
#define STATEMACHINECOMPONENT_H

#include <Temporal\Base\Timer.h>
#include <Temporal\Base\Hash.h>
#include "Component.h"
#include <unordered_map>

namespace Temporal
{
	class Message;
	class StateMachineComponent;

	class ComponentState
	{
	public:
		ComponentState(void) {};
		virtual ~ComponentState(void) {};

		void setStateMachine(StateMachineComponent* stateMachine) { _stateMachine = stateMachine; }

		virtual void enter(void) const {}
		virtual void exit(void) const {}
		virtual void handleMessage(Message& message) const = 0;

	protected:
		StateMachineComponent* _stateMachine;

	private:
		ComponentState(const ComponentState&);
		ComponentState& operator=(const ComponentState&);
	};

	typedef std::unordered_map<const Hash, ComponentState*> StateCollection;
	typedef StateCollection::const_iterator StateIterator;

	class StateMachineComponent : public Component
	{
	public:
		explicit StateMachineComponent(StateCollection states, const char* prefix);
		virtual ~StateMachineComponent(void);

		void changeState(const Hash& stateID);
		virtual void handleMessage(Message& message);

		bool getFlag1(void) const { return _flag1; }
		void setFlag1(bool value) { _flag1 = value; }
		bool getFlag2(void) const { return _flag2; }
		void setFlag2(bool value) { _flag2 = value; }
		bool getFlag3(void) const { return _flag3; }
		void setFlag3(bool value) { _flag3 = value; }

		Timer& getTimer(void) { return _timer; }

	protected:
		virtual Hash getInitialState(void) const = 0;

	private:
		const Hash STATE_SERIALIZATION;
		const Hash TIMER_SERIALIZATION;
		StateCollection _states;

		const ComponentState* _currentState;
		Hash _currentStateID;

		// Temp state
		bool _flag1;
		bool _flag2;
		bool _flag3;

		Timer _timer;

		void resetTempState(void) { _flag1 = _flag2 = _flag3 = false; }

		StateMachineComponent(const StateMachineComponent&);
		StateMachineComponent& operator=(const StateMachineComponent&);
	};
}

#endif