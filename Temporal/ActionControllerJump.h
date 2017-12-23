#ifndef ACTIONCONTROLLERJUMP_H
#define ACTIONCONTROLLERJUMP_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Jump : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
