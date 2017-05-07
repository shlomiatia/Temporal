#ifndef ACTIONCONTROLLERDRAGSTAND_H
#define ACTIONCONTROLLERDRAGSTAND_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class DragStand : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
