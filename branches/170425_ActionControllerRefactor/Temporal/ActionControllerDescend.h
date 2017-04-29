#ifndef ACTIONCONTROLLERDESCEND_H
#define ACTIONCONTROLLERDESCEND_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Descend : public ComponentState
		{
		public:
			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
