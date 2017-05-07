#ifndef ACTIONCONTROLLERCLIMB_H
#define ACTIONCONTROLLERCLIMB_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Climb : public ComponentState
		{
		public:
			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
