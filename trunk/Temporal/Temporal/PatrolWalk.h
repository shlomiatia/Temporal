#pragma once
#include "StateMachineComponent.h"

namespace Temporal {
	class Message;
}

namespace Temporal
{
	namespace PatrolStates
	{
		class Walk : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};
	}
}
