#ifndef ACTIONCONTROLLERINVESTIGATE_H
#define ACTIONCONTROLLERINVESTIGATE_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Investigate : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
