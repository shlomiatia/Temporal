#ifndef NAVIGATORJUMPFORWARD_H
#define NAVIGATORJUMPFORWARD_H

#include "StateMachineComponent.h"

namespace Temporal
{
	class Serialization;

	namespace NavigatorStates
	{
		class JumpForward : public ComponentState
		{
		public:
			JumpForward() : _afterLoad(true) {}

			void enter(void* param);
			void handleMessage(Message& message);
		private:
			bool _afterLoad;
		};
	}
}

#endif