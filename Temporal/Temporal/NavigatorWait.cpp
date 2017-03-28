#include "NavigatorWait.h""
#include "Navigator.h""
#include "MessageUtils.h"
#include "NavigationGraph.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");

		void Wait::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::NAVIGATE)
			{
				Hash tracked = *static_cast<const Hash*>(message.getParam());
				Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
				navigator.setTracked(tracked);
				navigator.plotPath();
			}
		}
	}
}