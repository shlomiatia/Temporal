#pragma once
#include "StateMachineComponent.h"

namespace Temporal
{
	namespace PatrolStates
	{
		class Fire : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

	}
}
