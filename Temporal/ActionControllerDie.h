#ifndef ACTIONCONTROLLERDIE_H
#define ACTIONCONTROLLERDIE_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Die : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
