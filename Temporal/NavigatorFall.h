#ifndef NAVIGATORFALL_H
#define NAVIGATORFALL_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		class Fall : public ComponentState
		{
		public:
			Fall() : _afterLoad(true) {}

			void enter(void* param);
			void handleMessage(Message& message);

		private:
			bool _afterLoad;
		};
	}
}

#endif