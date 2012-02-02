#pragma once

#include <Temporal/Game/Component.h>

namespace Temporal
{
	class InputController : public Component
	{
	public:
		InputController(void) {}

		virtual ComponentType::Type getType(void) const { return ComponentType::INPUT_CONTROLLER; }

		virtual void handleMessage(Message& message);

	private:
		InputController(const InputController&);
		InputController& operator=(const InputController&);
	};
}
