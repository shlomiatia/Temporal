#ifndef ACTIONCONTROLLERSFIRE_H
#define ACTIONCONTROLLERSFIRE_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Fire : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
