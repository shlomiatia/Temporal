#ifndef ACTIONCONTROLLERFALL_H
#define ACTIONCONTROLLERFALL_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Fall : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif