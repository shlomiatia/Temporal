#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "EntitySystem.h"
#include "Timer.h"
#include "Ids.h"

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController() : _isJump(false) {}

		void handleMessage(Message& message);
		Hash getType() const { return ComponentsIds::INPUT_CONTROLLER; }

		Component* clone() const { return new InputController(); }
		
	private:
		Timer _timer;
		bool _isJump;

		void startJump();
		void temporalAction();
		void mouseDown(Message& message);
		void keyDown(Message& message);
		void gamepadButtonDown(Message& message);
		void update(float time);
	};
}

#endif