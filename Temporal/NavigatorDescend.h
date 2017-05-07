#ifndef NAVIGATORDESCEND_H
#define NAVIGATORDESCEND_H

#include "StateMachineComponent.h"

namespace Temporal
{
	class Serialization;

	namespace NavigatorStates
	{
		class Descend : public ComponentState
		{
		public:
			Descend() : _afterLoad(false) {}

			void enter(void* param);
			void handleMessage(Message& message);

		private:
			bool _afterLoad;
		};
	}
}

#endif