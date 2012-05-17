#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "Component.h"

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController(void) {}

		ComponentType::Enum getType(void) const { return ComponentType::INPUT_CONTROLLER; }

		void handleMessage(Message& message);
	};
}

#endif