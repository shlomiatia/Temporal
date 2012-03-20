#ifndef STATEMACHINECOMPONENT_H
#define STATEMACHINECOMPONENT_H

#include "Component.h"
#include <vector>

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

		virtual void enter(void) {}
		virtual void exit(void) {}
		virtual void handleMessage(Message& message) = 0;

	protected:
		StateMachineComponent* _stateMachine;

	private:
		ComponentState(const ComponentState&);
		ComponentState& operator=(const ComponentState&);
	};

	typedef std::vector<ComponentState*> StateCollection;
	typedef StateCollection::const_iterator StateIterator;

	class StateMachineComponent : public Component
	{
	public:
		StateMachineComponent(StateCollection states);
		virtual ~StateMachineComponent(void);

		void changeState(int stateID);
		virtual void handleMessage(Message& message);

	protected:
		virtual int getInitialState(void) const = 0;

	private:
		StateCollection _states;
		ComponentState* _currentState;
		int _currentStateID;

		StateMachineComponent(const StateMachineComponent&);
		StateMachineComponent& operator=(const StateMachineComponent&);
	};
}

#endif