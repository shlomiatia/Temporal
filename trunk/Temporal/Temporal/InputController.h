#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "EntitySystem.h"
#include "Timer.h"

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController() : _isJump(false) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }

		Component* clone() const { return new InputController(); }

		static const Hash TYPE;
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