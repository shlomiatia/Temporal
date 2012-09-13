#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "EntitySystem.h"

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController() {}

		ComponentType::Enum getType() const { return ComponentType::INPUT_CONTROLLER; }

		void handleMessage(Message& message);
		Component* clone() const { return new InputController(); }
	};
}

#endif