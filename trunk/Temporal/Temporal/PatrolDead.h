#pragma once
#include "StateMachineComponent.h"

namespace Temporal
{
	namespace PatrolStates
	{
		class Dead : public ComponentState
		{
		public:
			void handleMessage(Message& message) {};
		};
	}
}
