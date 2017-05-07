#ifndef ACTIONCONTROLLERDRAGBACKWARDS_H
#define ACTIONCONTROLLERDRAGBACKWARDS_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class DragBackwards : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
