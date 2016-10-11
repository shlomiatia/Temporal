#ifndef NAVIGATORWALK_H
#define NAVIGATORWALK_H

#include "StateMachineComponent.h"

namespace Temporal
{
	class Serialization;

	namespace NavigatorStates
	{
		class Walk : public ComponentState
		{
		public:
			void enter(void* param) { update(); };
			void handleMessage(Message& message);
			void update();
			void updateNext();
		};
	}
}

#endif