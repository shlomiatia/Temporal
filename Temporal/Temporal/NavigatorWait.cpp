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
				const Vector& goalPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(tracked, Message(MessageID::GET_POSITION)));
				Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
				navigator.setTracked(tracked);
				navigator.plotPath(goalPosition);
			}
		}
	}
}