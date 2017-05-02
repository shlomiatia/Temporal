#ifndef ACTIONCONTROLLERJUMPEND_H
#define ACTIONCONTROLLERJUMPEND_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class JumpEnd : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
