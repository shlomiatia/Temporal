#include "NavigatorWait.h""
#include "Navigator.h""
#include "MessageUtils.h"
#include "NavigationGraph.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");

		static const Hash TIME_MACHINE_ID = Hash("ENT_TIME_MACHINE0");

		void Wait::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::NAVIGATE)
			{
				Hash tracked = *static_cast<const Hash*>(message.getParam());
				int sourceCollisionGroup = getIntParam(_stateMachine->raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
				int targetCollisionGroup = getIntParam(_stateMachine->getEntity().getManager().sendMessageToEntity(tracked, Message(MessageID::GET_COLLISION_GROUP)));
				Hash destinationId = tracked;
				Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
				if (sourceCollisionGroup != targetCollisionGroup)
				{
					destinationId = TIME_MACHINE_ID;
					navigator.setTimeMachine(true);
				}

				const Vector& goalPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(destinationId, Message(MessageID::GET_POSITION)));
				navigator.plotPath(goalPosition, tracked);
			}
		}
	}
}