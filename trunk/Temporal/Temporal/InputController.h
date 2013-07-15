#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "EntitySystem.h"

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController() {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }

		Component* clone() const { return new InputController(); }

		static const Hash TYPE;
	private:
	};
}

#endif