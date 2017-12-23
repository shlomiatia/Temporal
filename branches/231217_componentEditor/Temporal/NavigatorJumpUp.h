#ifndef NAVIGATORJUMPUP_H
#define NAVIGATORJUMPUP_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		class JumpUp : public ComponentState
		{
		public:
			JumpUp() : _afterLoad(true) {}

			void enter(void* param);
			void handleMessage(Message& message);
		private:
			bool _afterLoad;
		};
	}
}

#endif