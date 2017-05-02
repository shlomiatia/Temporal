#ifndef ACTIONCONTROLLERDRAGFORWARD_H
#define ACTIONCONTROLLERDRAGFORWARD_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class DragForward : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
