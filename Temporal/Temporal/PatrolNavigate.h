#pragma once
#include "StateMachineComponent.h"

namespace Temporal
{
	namespace PatrolStates
	{
		class Navigate : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};
	}
}
