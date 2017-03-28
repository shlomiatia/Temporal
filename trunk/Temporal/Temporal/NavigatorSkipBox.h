#ifndef NAVIGATORSKIPBOX_H
#define NAVIGATORSKIPBOX_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		class SkipBox : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};
	}
}

#endif