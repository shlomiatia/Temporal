#ifndef NAVIGATORWAIT_H
#define NAVIGATORWAIT_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		class Wait : public ComponentState
		{
		public:
			void enter(void* param){};
			void handleMessage(Message& message);
		};
	}
}

#endif