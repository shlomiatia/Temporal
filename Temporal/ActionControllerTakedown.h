#ifndef ACTIONCONTROLLERTAKEDOWN_H
#define ACTIONCONTROLLERTAKEDOWN_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Takedown : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
