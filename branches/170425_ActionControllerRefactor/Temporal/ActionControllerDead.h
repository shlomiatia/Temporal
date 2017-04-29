#ifndef ACTIONCONTROLLERDEAD_H
#define ACTIONCONTROLLERSDEAD_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Dead : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message) {};
		};
	}
}

#endif
