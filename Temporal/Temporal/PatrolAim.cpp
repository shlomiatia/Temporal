#include "PatrolAim.h"
#include "Grid.h"
#include "Patrol.h"
#include "PhysicsEnums.h"

namespace Temporal
{
	namespace PatrolStates
	{
		static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");
		static const Hash FIRE_STATE = Hash("PAT_STT_FIRE");

		void Aim::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_AIM));
			// TempFlag 1 - LOS
			_stateMachine->setFrameFlag1(true);
		}

		void Aim::handleMessage(Message& message)
		{
			Patrol& patrol = *static_cast<Patrol*>(_stateMachine);
			if (patrol.handleTakedownMessage(message))
			{

			}
			else if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				if (result.getFixture().getCategory() == CollisionCategory::PLAYER)
					_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (_stateMachine->getTimer().getElapsedTime() >= patrol.getAimTime())
				{
					if (!_stateMachine->getFrameFlag1())
					{
						_stateMachine->changeState(WAIT_STATE);
					}
					else
					{
						_stateMachine->changeState(FIRE_STATE);
					}
				}
			}
		}
	}
}
