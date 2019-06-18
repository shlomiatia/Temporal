#include "PatrolNavigate.h"
#include "Patrol.h"
#include "PhysicsEnums.h"
#include "CommonMessagesHandler.h"

namespace Temporal
{
	static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");

	namespace PatrolStates
	{
		void Navigate::handleMessage(Message& message)
		{
			Patrol& patrol = *static_cast<Patrol*>(_stateMachine);

			patrol.handleFireMessage(message);
			patrol.handleTakedownMessage(message);

			if (message.getID() == MessageID::NAVIGATION_SUCCESS)
			{
				Hash id = getHashParam(message.getParam());

				CollisionCategory::Enum category = *static_cast<CollisionCategory::Enum*>(_stateMachine->getEntity().getManager().sendMessageToEntity(id, Message(MessageID::GET_COLLISION_CATEGORY)));
				if (category & CollisionCategory::DEAD)
				{
					_stateMachine->getEntity().getManager().sendMessageToEntity(id, Message(MessageID::INVESTIGATE));
					_stateMachine->raiseMessage(Message(MessageID::ACTION_INVESTIGATE));
				}
				_stateMachine->changeState(WAIT_STATE);
			}
			else if (message.getID() == MessageID::NAVIGATION_FAILURE)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
		}
	}
}
