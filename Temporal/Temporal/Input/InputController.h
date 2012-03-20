#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <Temporal\Game\Component.h>

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController(void) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::INPUT_CONTROLLER; }

		virtual void handleMessage(Message& message);
	};
}

#endif