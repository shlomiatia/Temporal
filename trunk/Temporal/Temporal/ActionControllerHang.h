#ifndef ACTIONCONTROLLERHANG_H
#define ACTIONCONTROLLERHANG_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		class Hang : public ComponentState
		{
		public:
			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif
