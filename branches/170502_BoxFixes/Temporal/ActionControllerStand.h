#ifndef ACTIONCONTROLLERSTAND_H
#define ACTIONCONTROLLERSTAND_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Stand : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
